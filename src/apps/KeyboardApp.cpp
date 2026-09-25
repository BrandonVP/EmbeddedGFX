/*
===========================================================================
Name        : KeyboardApp.cpp
Author      : Brandon Van Pelt
Description : On-screen keyboard app (see KeyboardApp.h).

              The key grid is always 10 x 4; only the labels change between the
              lowercase, uppercase and symbol layers, so the button rects are
              built once and relabelled in place.

              Every coordinate is a fraction of the real body region (read from
              the display adapter, not the GFX_SCREEN_* macros — those compile
              to their defaults inside the library), so the same code lays out
              on a 480x480 or a 480x320 panel.
===========================================================================
*/
#include "KeyboardApp.h"
#include "../Gui.h"
#include "../App.h"
#include "../Theme.h"

#include <string.h>

// --- Layers ----------------------------------------------------------------
enum { LAYER_LOWER = 0, LAYER_UPPER, LAYER_SYMBOL };

static const uint8_t KEY_COLS = 10;
static const uint8_t KEY_ROWS = 4;
static const uint8_t KEY_COUNT = KEY_COLS * KEY_ROWS;

// 40 characters per layer, row major.
static const char LAYER_CHARS[3][KEY_COUNT + 1] = {
    "1234567890"
    "qwertyuiop"
    "asdfghjkl@"
    "zxcvbnm.-_",

    "1234567890"
    "QWERTYUIOP"
    "ASDFGHJKL@"
    "ZXCVBNM.-_",

    "1234567890"
    "!#$%^&*()~"
    "-_=+[]{}\\|"
    ";:'\",.<>/?"
};

// --- Button indices --------------------------------------------------------
enum {
    IDX_TITLE = 0,
    IDX_FIELD,
    IDX_REVEAL,
    IDX_KEY0,
    IDX_SHIFT = IDX_KEY0 + KEY_COUNT,
    IDX_LAYER,
    IDX_SPACE,
    IDX_DEL,
    IDX_CANCEL,
    IDX_ACCEPT,
    IDX_COUNT
};

// --- Click returns ---------------------------------------------------------
static const int CR_KEY_BASE = 1;    // 1..40
static const int CR_SHIFT    = 50;
static const int CR_LAYER    = 51;
static const int CR_SPACE    = 52;
static const int CR_DEL      = 53;
static const int CR_REVEAL   = 54;
static const int CR_CANCEL   = 60;
static const int CR_ACCEPT   = 61;

// --- Session state ---------------------------------------------------------
static char           s_text[KEYBOARDAPP_MAX_TEXT + 1];
static char           s_original[KEYBOARDAPP_MAX_TEXT + 1];
static char           s_title[32];
static uint8_t        s_len = 0;
static uint8_t        s_maxLen = KEYBOARDAPP_MAX_TEXT;
static bool           s_maskable = false;   // caller asked for a masked entry
static bool           s_mask = false;       // masked right now (SHOW/HIDE)
static uint8_t        s_layer = LAYER_LOWER;
static gfx_app_id_t   s_returnApp = 0;
static KeyboardDoneFn s_onDone = nullptr;

// The field label can only hold what UserInterfaceClass::textBuffer takes, so a
// long entry shows its tail.
static const uint8_t FIELD_VISIBLE = 28;

void KeyboardApp_open(const char* title, const char* initialText, uint8_t maxLen,
                      bool maskInput, gfx_app_id_t returnApp, KeyboardDoneFn onDone)
{
    s_title[0] = '\0';
    if (title)
    {
        strncpy(s_title, title, sizeof(s_title) - 1);
        s_title[sizeof(s_title) - 1] = '\0';
    }

    s_text[0] = '\0';
    if (initialText)
    {
        strncpy(s_text, initialText, KEYBOARDAPP_MAX_TEXT);
        s_text[KEYBOARDAPP_MAX_TEXT] = '\0';
    }
    strncpy(s_original, s_text, sizeof(s_original));
    s_original[sizeof(s_original) - 1] = '\0';

    s_len       = (uint8_t)strlen(s_text);
    s_maxLen    = (maxLen == 0 || maxLen > KEYBOARDAPP_MAX_TEXT) ? KEYBOARDAPP_MAX_TEXT : maxLen;
    s_maskable  = maskInput;
    s_mask      = maskInput;     // starts hidden; SHOW reveals it
    s_layer     = LAYER_LOWER;
    s_returnApp = returnApp;
    s_onDone    = onDone;
}

const char* KeyboardApp_text(void) { return s_text; }

// --- Layout ----------------------------------------------------------------
// Fractions of the body region.
static const float F_TITLE_TOP  = 0.010f, F_TITLE_BOT  = 0.070f;
static const float F_FIELD_TOP  = 0.085f, F_FIELD_BOT  = 0.185f;
static const float F_KEY_TOP    = 0.200f, F_KEY_PITCH  = 0.140f, F_KEY_H = 0.125f;
static const float F_FUNC_TOP   = 0.775f, F_FUNC_BOT   = 0.875f;
static const float F_ACT_TOP    = 0.895f, F_ACT_BOT    = 0.995f;

static int s_bodyTop = 0, s_bodyH = 0, s_width = 0;

static int fy(float f) { return s_bodyTop + (int)(f * (float)s_bodyH); }

static void measure(void)
{
    s_width   = GUI_I.screenWidth();
    s_bodyTop = GFX_MENU_BAR_HEIGHT;
    s_bodyH   = GUI_I.screenHeight() - s_bodyTop;
}

// --- Content ---------------------------------------------------------------
static void setFieldLabel(void)
{
    UserInterfaceClass& f = GUI_I.appButtons()[IDX_FIELD];

    char shown[FIELD_VISIBLE + 2];
    uint8_t visible = (s_len > FIELD_VISIBLE) ? FIELD_VISIBLE : s_len;
    const char* tail = s_text + (s_len - visible);

    for (uint8_t i = 0; i < visible; i++)
        shown[i] = s_mask ? '*' : tail[i];

    // Trailing caret, so an empty field still shows where typing lands.
    shown[visible]     = '_';
    shown[visible + 1] = '\0';

    f.setTextFormat("%s", shown);
}

static void styleToggle(uint8_t index, bool active)
{
    UserInterfaceClass& b = GUI_I.appButtons()[index];
    b.setBgColor(active ? gfxTheme.orangeBtn : gfxTheme.btnColor);
    b.setBorderColor(active ? gfxTheme.orangeBtn : gfxTheme.btnBorder);
    b.setTextColor(active ? 0x0000 : gfxTheme.btnText);
}

// Relabel the 40 keys for the active layer. Each label is its own 2-byte
// string, held here because setText() keeps the pointer rather than copying.
static char s_keyLabel[KEY_COUNT][2];

static void applyLayer(void)
{
    UserInterfaceClass* b = GUI_I.appButtons();
    const char* chars = LAYER_CHARS[s_layer];

    for (uint8_t i = 0; i < KEY_COUNT; i++)
    {
        s_keyLabel[i][0] = chars[i];
        s_keyLabel[i][1] = '\0';
        b[IDX_KEY0 + i].setText(s_keyLabel[i]);
    }

    b[IDX_LAYER].setText((s_layer == LAYER_SYMBOL) ? "abc" : "?#[]");
    styleToggle(IDX_SHIFT, s_layer == LAYER_UPPER);
    styleToggle(IDX_LAYER, s_layer == LAYER_SYMBOL);
}

// --- Page ------------------------------------------------------------------
uint8_t KeyboardApp_createBtns(void)
{
    measure();

    UserInterfaceClass* b = GUI_I.appButtons();
    const uint16_t fill = gfxShade(gfxTheme.background, 15);

    const int margin = 8;
    const int gap    = 4;
    const int keyW   = (s_width - 2 * margin - (KEY_COLS - 1) * gap) / KEY_COLS;
    const int keyH   = (int)(F_KEY_H * (float)s_bodyH);
    const uint8_t keyText = (keyH >= 44) ? 24 : 16;

    // Centre the grid: the key width is truncated, so the leftover would
    // otherwise all pile up on the right.
    const int gridX = (s_width - (KEY_COLS * keyW + (KEY_COLS - 1) * gap)) / 2;

    b[IDX_TITLE].setButton(margin, fy(F_TITLE_TOP), s_width - margin, fy(F_TITLE_BOT),
                           0, true, 8, s_title, ALIGN_CENTER,
                           gfxTheme.background, gfxTheme.background, gfxTheme.btnTextColor);
    b[IDX_TITLE].setTextSize(16);
    b[IDX_TITLE].setClickable(false);

    // A masked entry gets a SHOW/HIDE toggle beside the field, so the user can
    // check a long password before committing to it.
    const int revealW = 84;
    const int fieldRight = s_maskable ? (s_width - margin - revealW - gap) : (s_width - margin);

    b[IDX_FIELD].setButton(margin, fy(F_FIELD_TOP), fieldRight, fy(F_FIELD_BOT),
                           0, true, 10, "", ALIGN_LEFT,
                           fill, gfxTheme.btnBorder, gfxTheme.btnTextColor);
    b[IDX_FIELD].setTextSize(16);
    b[IDX_FIELD].setClickable(false);

    b[IDX_REVEAL].setButton(s_width - margin - revealW, fy(F_FIELD_TOP), s_width - margin, fy(F_FIELD_BOT),
                            CR_REVEAL, true, 10, s_mask ? "SHOW" : "HIDE", ALIGN_CENTER,
                            gfxTheme.btnColor, gfxTheme.btnBorder, gfxTheme.btnText);
    b[IDX_REVEAL].setTextSize(16);
    if (!s_maskable)
    {
        // Nothing to reveal: keep the slot, but neither draw nor hit-test it.
        b[IDX_REVEAL].setPrintable(false);
        b[IDX_REVEAL].setClickable(false);
    }

    for (uint8_t r = 0; r < KEY_ROWS; r++)
    {
        int y1 = fy(F_KEY_TOP + r * F_KEY_PITCH);
        for (uint8_t c = 0; c < KEY_COLS; c++)
        {
            uint8_t i  = (uint8_t)(r * KEY_COLS + c);
            int     x1 = gridX + c * (keyW + gap);
            b[IDX_KEY0 + i].setButton(x1, y1, x1 + keyW, y1 + keyH,
                                      (uint16_t)(CR_KEY_BASE + i), true, 8, "", ALIGN_CENTER,
                                      gfxTheme.btnColor, gfxTheme.btnBorder, gfxTheme.btnText);
            b[IDX_KEY0 + i].setTextSize(keyText);
        }
    }

    // Function row: shift | layer | space | delete.
    {
        const int y1 = fy(F_FUNC_TOP), y2 = fy(F_FUNC_BOT);
        const int usable = s_width - 2 * margin - 3 * gap;
        const int wShift = (int)(usable * 0.22f);
        const int wLayer = (int)(usable * 0.22f);
        const int wDel   = (int)(usable * 0.24f);
        const int wSpace = usable - wShift - wLayer - wDel;

        int x = margin;
        b[IDX_SHIFT].setButton(x, y1, x + wShift, y2, CR_SHIFT, true, 10, "SHIFT", ALIGN_CENTER,
                               gfxTheme.btnColor, gfxTheme.btnBorder, gfxTheme.btnText);
        b[IDX_SHIFT].setTextSize(16);
        x += wShift + gap;

        b[IDX_LAYER].setButton(x, y1, x + wLayer, y2, CR_LAYER, true, 10, "?#[]", ALIGN_CENTER,
                               gfxTheme.btnColor, gfxTheme.btnBorder, gfxTheme.btnText);
        b[IDX_LAYER].setTextSize(16);
        x += wLayer + gap;

        b[IDX_SPACE].setButton(x, y1, x + wSpace, y2, CR_SPACE, true, 10, "SPACE", ALIGN_CENTER,
                               gfxTheme.btnColor, gfxTheme.btnBorder, gfxTheme.btnText);
        b[IDX_SPACE].setTextSize(16);
        x += wSpace + gap;

        b[IDX_DEL].setButton(x, y1, x + wDel, y2, CR_DEL, true, 10, "DEL", ALIGN_CENTER,
                             gfxTheme.btnColor, gfxTheme.btnBorder, gfxTheme.btnText);
        b[IDX_DEL].setTextSize(16);
    }

    // Action row: cancel | accept.
    {
        const int y1 = fy(F_ACT_TOP), y2 = fy(F_ACT_BOT);
        const int half = (s_width - 2 * margin - gap) / 2;

        b[IDX_CANCEL].setButton(margin, y1, margin + half, y2, CR_CANCEL, true, 10, "CANCEL", ALIGN_CENTER,
                                gfxTheme.btnColor, gfxTheme.btnBorder, gfxTheme.btnText);
        b[IDX_CANCEL].setTextSize(16);

        b[IDX_ACCEPT].setButton(margin + half + gap, y1, s_width - margin, y2, CR_ACCEPT, true, 10,
                                "ACCEPT", ALIGN_CENTER,
                                gfxTheme.orangeBtn, gfxTheme.orangeBtn, gfxTheme.blackBtn);
        b[IDX_ACCEPT].setTextSize(16);
    }

    applyLayer();
    setFieldLabel();

    return IDX_COUNT;
}

static void refreshField(void)
{
    setFieldLabel();
    GUI_I.updateButton(IDX_FIELD);
    GUI_I.updateScreen();
}

static void finish(bool accepted)
{
    if (!accepted)
    {
        strncpy(s_text, s_original, sizeof(s_text));
        s_text[sizeof(s_text) - 1] = '\0';
        s_len = (uint8_t)strlen(s_text);
    }

    if (s_onDone)
        s_onDone(s_text, accepted);

    App* app = GUI_I.getApp();
    if (app) app->newApp(s_returnApp);
}

void KeyboardApp_handler(int userInput)
{
    if (userInput < 0)
        return;

    // Character keys
    if (userInput >= CR_KEY_BASE && userInput < CR_KEY_BASE + KEY_COUNT)
    {
        if (s_len >= s_maxLen)
            return;

        s_text[s_len++] = LAYER_CHARS[s_layer][userInput - CR_KEY_BASE];
        s_text[s_len] = '\0';
        refreshField();
        return;
    }

    switch (userInput)
    {
        case CR_SHIFT:
            // Sticky, not one-shot: passwords often have runs of capitals, and a
            // shift that silently expires is worse than one you can see.
            s_layer = (s_layer == LAYER_UPPER) ? LAYER_LOWER : LAYER_UPPER;
            applyLayer();
            for (uint8_t i = 0; i < KEY_COUNT; i++)
                GUI_I.updateButton(IDX_KEY0 + i);
            GUI_I.updateButton(IDX_SHIFT);
            GUI_I.updateButton(IDX_LAYER);
            GUI_I.updateScreen();
            break;

        case CR_LAYER:
            s_layer = (s_layer == LAYER_SYMBOL) ? LAYER_LOWER : LAYER_SYMBOL;
            applyLayer();
            for (uint8_t i = 0; i < KEY_COUNT; i++)
                GUI_I.updateButton(IDX_KEY0 + i);
            GUI_I.updateButton(IDX_SHIFT);
            GUI_I.updateButton(IDX_LAYER);
            GUI_I.updateScreen();
            break;

        case CR_SPACE:
            if (s_len < s_maxLen)
            {
                s_text[s_len++] = ' ';
                s_text[s_len] = '\0';
                refreshField();
            }
            break;

        case CR_DEL:
            if (s_len > 0)
            {
                s_text[--s_len] = '\0';
                refreshField();
            }
            break;

        case CR_REVEAL:
            if (!s_maskable)
                break;
            s_mask = !s_mask;
            GUI_I.appButtons()[IDX_REVEAL].setText(s_mask ? "SHOW" : "HIDE");
            GUI_I.updateButton(IDX_REVEAL);
            refreshField();
            break;

        case CR_CANCEL: finish(false); break;
        case CR_ACCEPT: finish(true);  break;
        default: break;
    }
}
