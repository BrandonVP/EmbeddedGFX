/*
===========================================================================
Name        : App.cpp
Author      : Brandon Van Pelt
Description : App registry and render state machine (see App.h).
===========================================================================
*/
#include "App.h"
#include "Gui.h"
#include "Theme.h"

// --- Registration ----------------------------------------------------------
void App::add(gfx_menu_id_t menu, const char* name, gfx_app_id_t label, void (*appFunc)(int), uint8_t (*printFunc)(void))
{
    myApps.push_back(appManager(menu, name, label, appFunc, printFunc));
}

void App::add(gfx_menu_id_t menu, const char* name, gfx_app_id_t label, void (*appFunc)(int), uint8_t (*printFunc)(void), void (*animFunc)(void))
{
    myApps.push_back(appManager(menu, name, label, appFunc, printFunc, animFunc));
}

void App::add(gfx_menu_id_t menu, const char* name, gfx_app_id_t label, void (*appFunc)(int), uint8_t (*printFunc)(void), void (*animFunc)(void), bool (*cbFunc)(int))
{
    myApps.push_back(appManager(menu, name, label, appFunc, printFunc, animFunc, cbFunc));
}

void App::init()
{
    // Build id -> index map for O(1) lookup regardless of registration order.
    for (int i = 0; i < 256; i++) appIndexMap[i] = 0xFF;
    for (int i = 0; i < (int)myApps.size(); i++)appIndexMap[myApps[i].getAppLabel()] = (uint8_t)i;

    // Start on the first registered app.
    if (!myApps.empty())
    {
        activeApp = myApps[0].getAppLabel();
        nextApp = activeApp;
    }
    renderState = APP_STATE_INIT;
}

void App::run()
{
    switch (renderState)
    {
        case APP_STATE_INIT:
        {
            GUI_I.clearAppSpace();
            drawIndex = 0;

            if (!myApps[appIndexMap[activeApp]].isAnimationsNULL())
            {
                renderState = APP_STATE_ANIMATION;
            }
            else
            {
                buttonsOnPage = myApps[appIndexMap[activeApp]].printButtons();
                renderState = APP_STATE_DRAWING;
            }
            break;
        }
        case APP_STATE_ANIMATION:
        {
            myApps[appIndexMap[activeApp]].runAnimations();
            buttonsOnPage = myApps[appIndexMap[activeApp]].printButtons();
            renderState = APP_STATE_DRAWING;
            break;
        }
        case APP_STATE_DRAWING:
        {
            if (GUI_I.drawPage(GUI_I.appButtons(), drawIndex, buttonsOnPage))
                return; // keep drawing next frame

            renderState = APP_STATE_DONE;
            GUI_I.updateScreen();
            break;
        }
        case APP_STATE_DONE:
        {
            int userInput = GUI_I.subMenuButtonMonitor(GUI_I.appButtons(), buttonsOnPage);
            myApps[appIndexMap[activeApp]].runApp(userInput);

            if (nextApp != activeApp)
            {
                if (!myApps[appIndexMap[activeApp]].isExecuteCBNULL())
                {
                    if (myApps[appIndexMap[activeApp]].executeCB(userInput))
                        return; // callback vetoed the transition
                }

                appTransition();
                renderState = APP_STATE_INIT;
            }
            break;
        }
    }
}

// Lay out the buttons for a generated menu page as a grid that fills the app
// body region, so it scales with the configured resolution. 1 column for up to
// 4 apps, otherwise 2 columns; rows follow from the count.
uint8_t App::printMenu(gfx_menu_id_t menu, gfx_app_id_t label, uint8_t numOfButtons)
{
    UserInterfaceClass* buttons = GUI_I.appButtons();

    if (numOfButtons > GFX_MAX_BUTTONS_LAYOUT)
        numOfButtons = GFX_MAX_BUTTONS_LAYOUT;

    uint8_t cols = (numOfButtons <= 4) ? 1 : 2;
    uint8_t rows = (uint8_t)((numOfButtons + cols - 1) / cols);

    // 1-column pages use a narrow, centered button; 2-column pages fill wider.
    uint16_t marginX = (cols == 1) ? (uint16_t)(GFX_BODY_WIDTH * 3 / 10) : 45;
    uint16_t gapX    = (cols == 1) ? 0 : 20;

    uint8_t btnPos = 0;
    for (int i = 0; i < getAppSize(); i++)
    {
        if (btnPos >= numOfButtons) break;
        if ((getMenu(i) == menu) && (getLabel(i) != label))
        {
            uint16_t x1, y1, x2, y2;
            gfxGridRect(btnPos, cols, rows, marginX, 20, gapX, 10, x1, y1, x2, y2);
            buttons[btnPos].setButton(x1, y1, x2, y2, getLabel(i), true, 10, getName(i), Align_Text_Center, gfxTheme.btnColor, gfxTheme.btnBorder, gfxTheme.blackBtn, gfxTheme.btnText);
            btnPos++;
        }
    }

    return btnPos;
}

// Count sibling apps in the active app's menu, then lay them out.
uint8_t App::buildMenuButtons()
{
    if (appIndexMap[activeApp] == 0xFF) return 0;

    gfx_menu_id_t menu = (gfx_menu_id_t)getMenu(appIndexMap[activeApp]);
    gfx_app_id_t  label = activeApp;

    uint16_t appCount = 0;
    for (int i = 0; i < getAppSize(); i++)
        if ((getMenu(i) == menu) && (getLabel(i) != label))
            appCount++;

    return printMenu(menu, label, appCount);
}

void App::appTransition()
{
    GUI_I.graphicLoaderState = 0;
    activeApp = nextApp;
    GUI_I.stopLoadBarTimed();
    GUI_I.useFrameBuffer(true);
}

void App::newApp(gfx_app_id_t loadNewApp)
{
    if (loadNewApp != GFX_APP_ID_NONE && appIndexMap[loadNewApp] != 0xFF && loadNewApp != activeApp)nextApp = loadNewApp;
}

int App::getAppSize(void) { return (int)myApps.size(); }

int App::getMenu(int index)
{
    if (index < 0 || index >= (int)myApps.size()) return -1;
    return myApps[index].getAssignedMenu();
}

gfx_app_id_t App::getLabel(int index)
{
    if (index < 0 || index >= (int)myApps.size()) return myApps[0].getAppLabel();
    return myApps[index].getAppLabel();
}

const char* App::getName(int index)
{
    if (index < 0 || index >= (int)myApps.size()) return myApps[0].getName();
    return myApps[index].getName();
}

gfx_app_id_t App::getActiveApp() { return activeApp; }

gfx_menu_id_t App::getActiveMenu()
{
    if (appIndexMap[activeApp] == 0xFF) return 0;
    return (gfx_menu_id_t)getMenu(appIndexMap[activeApp]);
}

void App::backgroundProcess() { GUI_I.sideLoadBarTimed(); }

uint8_t App::getButtonsOnPage() { return buttonsOnPage; }
void App::setButtonsOnPage(uint8_t buttons) { buttonsOnPage = buttons; }

// --- Library-provided generic menu app -------------------------------------
uint8_t GFX_createMenu()
{
    App* app = GUI_I.getApp();
    if (!app) return 0;
    return app->buildMenuButtons();
}

void GFX_menuInput(int userInput)
{
    App* app = GUI_I.getApp();
    if (!app) return;
    if (userInput >= 0 && userInput != GFX_APP_ID_NONE)
        app->newApp((gfx_app_id_t)userInput);
}
