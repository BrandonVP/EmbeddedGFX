/*
===========================================================================
Name        : appManager.cpp
Author      : Brandon Van Pelt
Description : One registered app (see appManager.h).
===========================================================================
*/
#include "appManager.h"

appManager::appManager()
    : assignedMenu(0), descriptiveName(""), appLabel(GFX_APP_ID_NONE)
{
}

appManager::appManager(gfx_menu_id_t menuLocation, const char* descriptiveTxt, gfx_app_id_t label, void (*appFunc)(int), uint8_t(*printFunc)(void), void (*animationFunc)(void), bool (*CBFunc)(int))
{
    assignedMenu = menuLocation;
    descriptiveName = descriptiveTxt;
    appLabel = label;
    runAppPtr = appFunc;
    printBtnPtr = printFunc;
    runAnimationsPtr = animationFunc;
    callBackPtr = CBFunc;
}

appManager::appManager(gfx_menu_id_t menuLocation, const char* descriptiveTxt, gfx_app_id_t label, void (*appFunc)(int), uint8_t(*printFunc)(void), void (*animationFunc)(void))
{
    assignedMenu = menuLocation;
    descriptiveName = descriptiveTxt;
    appLabel = label;
    runAppPtr = appFunc;
    printBtnPtr = printFunc;
    runAnimationsPtr = animationFunc;
    callBackPtr = nullptr;
}

appManager::appManager(gfx_menu_id_t menuLocation, const char* descriptiveTxt, gfx_app_id_t label, void (*appFunc)(int), uint8_t(*printFunc)(void))
{
    assignedMenu = menuLocation;
    descriptiveName = descriptiveTxt;
    appLabel = label;
    runAppPtr = appFunc;
    printBtnPtr = printFunc;
    runAnimationsPtr = nullptr;
    callBackPtr = nullptr;
}

uint8_t appManager::printButtons()
{
    if (printBtnPtr)
        return printBtnPtr();
    return 0;
}

void appManager::runApp(int userInput)
{
    if (runAppPtr)
        runAppPtr(userInput);
}

void appManager::runAnimations()
{
    if (runAnimationsPtr)
        runAnimationsPtr();
}

bool appManager::executeCB(int userInput)
{
    if (callBackPtr)
        return callBackPtr(userInput);
    return false;
}

bool appManager::isPrintButtonsNULL() const { return (printBtnPtr == nullptr); }
bool appManager::isAppNULL() const          { return (runAppPtr == nullptr); }
bool appManager::isAnimationsNULL() const    { return (runAnimationsPtr == nullptr); }
bool appManager::isExecuteCBNULL() const     { return (callBackPtr == nullptr); }

gfx_menu_id_t appManager::getAssignedMenu() const { return assignedMenu; }
const char* appManager::getName() const           { return descriptiveName; }
gfx_app_id_t appManager::getAppLabel() const       { return appLabel; }
