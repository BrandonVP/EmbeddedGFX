/*
===========================================================================
Name        : ThemeApp.h
Author      : Brandon Van Pelt
Description : Optional drop-in theme-picker app for EmbeddedGFX.

              Register it like any other app, on whatever menu you want:

                  app.add(MENU_settings, "Themes", APP_THEME,
                          ThemeApp_handler, ThemeApp_createBtns);

              Leave it unregistered to disable theme switching entirely.

              Persistence and menu-bar redraw are injected via callbacks so the
              library depends on neither SD/EEPROM nor any project menu code.
===========================================================================
*/
#ifndef EMBEDDEDGFX_THEMEAPP_H
#define EMBEDDEDGFX_THEMEAPP_H

#include <Arduino.h>
#include "../gfx_config.h"

// Number of built-in palettes.
#define THEMEAPP_COUNT 6

// --- Optional hooks (all safe to leave unset) ------------------------------
// Persistence: supply functions to load/save the selected palette index.
void ThemeApp_setStorage(uint8_t (*loadIndex)(void), void (*saveIndex)(uint8_t));

// Called after a theme change so the project can repaint its own menu bar
// (icons, clock, custom buttons). The generic menu strip is recolored anyway.
void ThemeApp_setMenuRedraw(void (*redraw)(void));

// --- Lifecycle -------------------------------------------------------------
// Load the saved palette (if storage is set) and apply its colors WITHOUT
// drawing. Call once in setup() before the first screen is drawn.
void ThemeApp_begin(void);

// --- App interface (pass these to app.add) ---------------------------------
uint8_t ThemeApp_createBtns(void);
void    ThemeApp_handler(int userInput);

#endif // EMBEDDEDGFX_THEMEAPP_H
