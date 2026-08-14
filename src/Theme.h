/*
===========================================================================
Name        : Theme.h
Author      : Brandon Van Pelt
Description : Library-owned color palette for EmbeddedGFX.

              A single global 'gfxTheme' holds every color the GUI draws with.
              Projects may overwrite its fields at startup, and the optional
              ThemeApp (src/apps/ThemeApp) swaps whole palettes at runtime.
===========================================================================
*/
#ifndef EMBEDDEDGFX_THEME_H
#define EMBEDDEDGFX_THEME_H

#include <stdint.h>

// RGB565 palette. Field names mirror the original ScanToolFD globals so
// existing app code ports with a simple find/replace to gfxTheme.<field>.
struct GfxTheme
{
    uint16_t background;    // themeBackground  — app body background
    uint16_t btnText;       // menuBtnText      — default button text
    uint16_t btnTextColor;  // menuBtnTextColor — menu/icon accent text
    uint16_t btnBorder;     // menuBtnBorder    — default button border
    uint16_t btnColor;      // menuBtnColor     — default button fill
    uint16_t menuBg;        // menuBackground   — top menu bar fill
    uint16_t menuBorder;    // menuBorder       — menu bar underline
    uint16_t frameBorder;   // frameBorder
    uint16_t orangeBtn;     // OrangeBtnColor   — accent / load bar
    uint16_t blackBtn;      // BlackBtnColor
    uint16_t menuText;      // menu-bar tab text (separate from btnTextColor so a
                            // dark/coloured bar can use light text over a light body)
};

// The single active palette. Defined in Theme.cpp, initialized to a dark-blue
// default that matches the original ScanToolFD look.
extern GfxTheme gfxTheme;

#endif // EMBEDDEDGFX_THEME_H
