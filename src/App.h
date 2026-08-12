/*
===========================================================================
Name        : App.h
Author      : Brandon Van Pelt
Description : App registry and render state machine for EmbeddedGFX.

              Apps are registered from the sketch with add(), then init() is
              called once. The framework owns no project headers — every app is
              supplied as function pointers plus a project-defined id.
===========================================================================
*/
#ifndef EMBEDDEDGFX_APP_H
#define EMBEDDEDGFX_APP_H

#include "gfx_config.h"
#include "appManager.h"
#include <vector>

#define NO_INPUT (-1)

class App
{
public:
    enum AppRenderState
    {
        APP_STATE_INIT,
        APP_STATE_ANIMATION,
        APP_STATE_DRAWING,
        APP_STATE_DONE
    };

    gfx_app_id_t nextApp = 0;
    gfx_app_id_t activeApp = 0;
    std::vector<appManager> myApps;
    uint8_t appIndexMap[256];       // app id -> index in myApps (0xFF = none)
    uint8_t buttonsOnPage = 0;
    AppRenderState renderState = APP_STATE_INIT;
    uint8_t drawIndex = 0;

    App() {}

    // --- Registration (call from setup() before init()) --------------------
    void add(gfx_menu_id_t menu, const char* name, gfx_app_id_t label,
             void (*appFunc)(int), uint8_t (*printFunc)(void));
    void add(gfx_menu_id_t menu, const char* name, gfx_app_id_t label,
             void (*appFunc)(int), uint8_t (*printFunc)(void), void (*animFunc)(void));
    void add(gfx_menu_id_t menu, const char* name, gfx_app_id_t label,
             void (*appFunc)(int), uint8_t (*printFunc)(void), void (*animFunc)(void), bool (*cbFunc)(int));

    void init();
    void run();

    // Generic menu-page building (used by GFX_createMenu / GFX_menuInput).
    uint8_t printMenu(gfx_menu_id_t menu, gfx_app_id_t label, uint8_t numOfButtons);
    uint8_t buildMenuButtons();

    void newApp(gfx_app_id_t);
    int getAppSize(void);
    int getMenu(int index);
    gfx_app_id_t getLabel(int index);
    const char* getName(int index);
    gfx_app_id_t getActiveApp();
    // Menu id the active app belongs to — handy for drawing the active
    // menu-bar highlight (e.g. on the first page, before any tap).
    gfx_menu_id_t getActiveMenu();
    void backgroundProcess();
    uint8_t getButtonsOnPage();
    void setButtonsOnPage(uint8_t buttons);

private:
    void appTransition();
};

// --- Library-provided generic "menu app" behaviour -------------------------
// Register a main-menu entry with these as its (handler, createBtns):
//   app.add(MENU_main, "Home", APP_HOME, GFX_menuInput, GFX_createMenu);
// createMenu lists every app assigned to the active menu; menuInput switches
// to the tapped app.
uint8_t GFX_createMenu();
void    GFX_menuInput(int userInput);

#endif // EMBEDDEDGFX_APP_H
