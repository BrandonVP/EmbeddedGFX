/*
===========================================================================
Name        : ThemeApp.cpp
Author      : Brandon Van Pelt
Description : Optional theme-picker app (see ThemeApp.h). Palettes carried over
              from ScanToolFD's themes.cpp.
===========================================================================
*/
#include "ThemeApp.h"
#include "../Gui.h"
#include "../App.h"
#include "../Theme.h"

struct ThemeColors {
    uint16_t background;
    uint16_t btnText;
    uint16_t btnTextColor;
    uint16_t btnBorder;
    uint16_t btnColor;
    uint16_t menuBg;
    uint16_t menuBorderColor;
    uint16_t frameBorderColor;
    uint16_t orangeBtn;
    uint16_t blackBtn;
    const char* name;
};

static const ThemeColors themes[THEMEAPP_COUNT] = {
    { 0x424B, 0xFFFF, 0xBE18, 0x869B, 0x0516, 0x5B0E, 0x39E8, 0x8452, 0xFC00, 0x0000, "Dark Blue" },
    { 0x2945, 0xFFFF, 0xC618, 0x8410, 0x3187, 0x18C3, 0xFD20, 0x8410, 0xFD20, 0x0000, "Carbon"    },
    { 0x2082, 0xFFFF, 0xFFFF, 0xFFFF, 0xA240, 0x4120, 0xFC80, 0x8410, 0xFC80, 0x0000, "Blaze"     },
    { 0x0200, 0xFFFF, 0x87F0, 0x0400, 0x0320, 0x0140, 0x07E0, 0x8410, 0x07E0, 0x0000, "Forest"    },
    { 0x3A0A, 0xFFFF, 0xCE7B, 0x6B90, 0x2967, 0x1905, 0x4C97, 0x73D0, 0xFC00, 0x0000, "Steel"     },
    { 0x424B, 0xFFFF, 0xBE18, 0x81E0, 0xA240, 0x5B0E, 0x39E8, 0x8452, 0xFC00, 0x0000, "Industrial"},
};

static uint8_t activeTheme = 0;

static uint8_t (*s_loadIndex)(void) = nullptr;
static void    (*s_saveIndex)(uint8_t) = nullptr;
static void    (*s_menuRedraw)(void) = nullptr;

void ThemeApp_setStorage(uint8_t (*loadIndex)(void), void (*saveIndex)(uint8_t))
{
    s_loadIndex = loadIndex;
    s_saveIndex = saveIndex;
}

void ThemeApp_setMenuRedraw(void (*redraw)(void))
{
    s_menuRedraw = redraw;
}

// Copy a palette into the live gfxTheme (no drawing).
static void setThemeColors(uint8_t index)
{
    const ThemeColors& t = themes[index];
    gfxTheme.background   = t.background;
    gfxTheme.btnText      = t.btnText;
    gfxTheme.btnTextColor = t.btnTextColor;
    gfxTheme.btnBorder    = t.btnBorder;
    gfxTheme.btnColor     = t.btnColor;
    gfxTheme.menuBg       = t.menuBg;
    gfxTheme.menuBorder   = t.menuBorderColor;
    gfxTheme.frameBorder  = t.frameBorderColor;
    gfxTheme.orangeBtn    = t.orangeBtn;
    gfxTheme.blackBtn     = t.blackBtn;
}

void ThemeApp_begin(void)
{
    if (s_loadIndex)
    {
        uint8_t idx = s_loadIndex();
        if (idx < THEMEAPP_COUNT)
            activeTheme = idx;
    }
    setThemeColors(activeTheme);
}

// Apply a palette and repaint. Menu strip is recolored generically; the
// project's own menu chrome is repainted via the optional redraw hook.
static void applyTheme(uint8_t index)
{
    setThemeColors(index);

    // Generic menu bar recolor.
    GUI_I.drawSquareBtn(0, 0, GFX_SCREEN_WIDTH, 45, "", gfxTheme.menuBg, gfxTheme.menuBg, gfxTheme.menuBg, ALIGN_CENTER);
    GUI_I.drawSquareBtn(0, 45, GFX_SCREEN_WIDTH, GFX_MENU_BAR_HEIGHT, "", gfxTheme.menuBorder, gfxTheme.menuBorder, gfxTheme.menuBorder, ALIGN_CENTER);

    if (s_menuRedraw)
        s_menuRedraw();

    GUI_I.updateScreen();

    // Re-render the active app body (the theme page) with the new colors.
    App* app = GUI_I.getApp();
    if (app)
        app->renderState = App::APP_STATE_INIT;
}

// 2x3 grid of palette swatches.
uint8_t ThemeApp_createBtns(void)
{
    static const uint16_t coords[THEMEAPP_COUNT][4] = {
        {  10,  62, 157, 172 }, { 167,  62, 314, 172 }, { 324,  62, 471, 172 },
        {  10, 184, 157, 294 }, { 167, 184, 314, 294 }, { 324, 184, 471, 294 },
    };

    UserInterfaceClass* buttons = GUI_I.appButtons();
    for (uint8_t i = 0; i < THEMEAPP_COUNT; i++)
    {
        uint16_t border = (i == activeTheme) ? 0xFFFF : themes[i].btnBorder;
        buttons[i].setButton(coords[i][0], coords[i][1], coords[i][2], coords[i][3], i, true, 10, themes[i].name, ALIGN_CENTER, themes[i].btnColor, border, themes[i].btnColor, themes[i].btnText);
    }
    return THEMEAPP_COUNT;
}

void ThemeApp_handler(int userInput)
{
    if (userInput < 0 || userInput >= THEMEAPP_COUNT)
        return;

    uint8_t selected = (uint8_t)userInput;
    if (selected == activeTheme)
        return;

    activeTheme = selected;
    applyTheme(selected);

    if (s_saveIndex)
        s_saveIndex(activeTheme);
}
