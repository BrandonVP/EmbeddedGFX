/*
===========================================================================
Name        : appManager.h
Author      : Brandon Van Pelt
Description : One registered app: which menu it lives on, its display name,
              its id, and the function pointers that draw/run it.

              Menu and app ids are generic integers (gfx_menu_id_t /
              gfx_app_id_t). The project defines its own enums for them, so the
              framework never needs to know the project's app names.
===========================================================================
*/
#ifndef EMBEDDEDGFX_APPMANAGER_H
#define EMBEDDEDGFX_APPMANAGER_H

#include <Arduino.h>
#include "gfx_config.h"

class appManager
{
public:
    appManager();

    appManager(gfx_menu_id_t menuLocation, const char* descriptiveTxt, gfx_app_id_t label, void (*appFunc)(int), uint8_t(*printFunc)(void), void (*animationFunc)(void), bool (*CBFunc)(int));
    appManager(gfx_menu_id_t menuLocation, const char* descriptiveTxt, gfx_app_id_t label, void (*appFunc)(int), uint8_t(*printFunc)(void), void (*animationFunc)(void));
    appManager(gfx_menu_id_t menuLocation, const char* descriptiveTxt, gfx_app_id_t label, void (*appFunc)(int), uint8_t(*printFunc)(void));

    uint8_t printButtons();
    void runApp(int userInput);
    void runAnimations();
    bool executeCB(int userInput);

    bool isPrintButtonsNULL() const;
    bool isAppNULL() const;
    bool isAnimationsNULL() const;
    bool isExecuteCBNULL() const;

    gfx_menu_id_t getAssignedMenu() const;
    const char* getName() const;
    gfx_app_id_t getAppLabel() const;

private:
    gfx_menu_id_t assignedMenu;
    const char* descriptiveName;
    gfx_app_id_t appLabel;

    void (*runAppPtr)(int) = nullptr;
    uint8_t(*printBtnPtr)(void) = nullptr;
    void (*runAnimationsPtr)(void) = nullptr;
    bool (*callBackPtr)(int) = nullptr;
};

#endif // EMBEDDEDGFX_APPMANAGER_H
