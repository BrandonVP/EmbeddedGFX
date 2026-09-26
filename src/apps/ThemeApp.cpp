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
    uint16_t menuText;
    const char* name;
};

// menuText is the tab-label color over the menu bar. For dark bars it equals
// btnTextColor; Snow uses a rich blue bar with white tabs over a light body.
//
// Forest and Industrial are both built the same way: neutral surfaces stepping
// cleanly from background through card and button to border, with the theme's
// own hue kept to the menu bar, its underline and the label text. That leaves
// orangeBtn — the "selected" accent for segmented rows, SAVE and ACCEPT — as
// the only saturated colour in the body, so a selection always reads.
//
// Both used to fail that. Forest was pure #00xx00 in nine of eleven slots with
// orangeBtn set to #00FF00, so a selected button was green among unselected
// greens. Industrial was Dark Blue's chrome with rust buttons, which put the
// orange accent in the same hue family as the buttons around it.
//
// Steel took its body from Dark Blue and its buttons from Carbon, so it read as
// a blend of its neighbours. It is now the coldest and darkest of the set, and
// the only one with bright chrome button borders.
//
// A caution when adding a theme: orangeBtn sits at luminance ~152, so a palette
// whose card fill lands near that (a mid-grey body) leaves a selected button
// with almost no contrast against the card behind it. Cards want to be clearly
// darker than the accent, or near-white as Snow's are.
static const ThemeColors themes[THEMEAPP_COUNT] = {
    { 0x424B, 0xFFFF, 0xBE18, 0x869B, 0x0516, 0x5B0E, 0x39E8, 0x8452, 0xFC00, 0x0000, 0xBE18, "Dark Blue" },
    { 0x2945, 0xFFFF, 0xC618, 0x8410, 0x3187, 0x18C3, 0xFD20, 0x8410, 0xFD20, 0x0000, 0xC618, "Carbon"    },
    { 0xFFFF, 0xFFFF, 0x0A4B, 0xC618, 0x2C3A, 0x2C3A, 0x22D1, 0xC618, 0xFC00, 0x0000, 0xFFFF, "Snow"      },
    { 0x10A1, 0xFFFF, 0xAED6, 0x5B8C, 0x3A68, 0x1A65, 0x3D0B, 0x8410, 0xFC00, 0x0000, 0xDF7C, "Forest"    },
    { 0x18E4, 0xFFFF, 0xD6FC, 0xBE3A, 0x4ACD, 0x2166, 0x9E5C, 0x8410, 0xFC00, 0x0000, 0xF7BF, "Steel"     },
    { 0x1081, 0xFFFF, 0xEED7, 0x6B2B, 0x4A27, 0x8A22, 0xF4C5, 0x8410, 0xFC00, 0x0000, 0xF75B, "Industrial"},
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
    gfxTheme.menuText     = t.menuText;
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

// 3-column x 2-row grid of palette swatches, computed to fill the body region.
uint8_t ThemeApp_createBtns(void)
{
    UserInterfaceClass* buttons = GUI_I.appButtons();
    for (uint8_t i = 0; i < THEMEAPP_COUNT; i++)
    {
        uint16_t x1, y1, x2, y2;
        gfxGridRect(i, 3, 2, 10, 12, 10, 12, x1, y1, x2, y2);
        uint16_t border = (i == activeTheme) ? 0xFFFF : themes[i].btnBorder;
        buttons[i].setButton(x1, y1, x2, y2, i, true, 10, themes[i].name, ALIGN_CENTER, themes[i].btnColor, border, themes[i].btnColor, themes[i].btnText);
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
