/*
===========================================================================
Name        : KeyboardApp.h
Author      : Brandon Van Pelt
Description : Optional on-screen keyboard app for EmbeddedGFX.

              A full text-entry page: a 10x4 key grid with lowercase, uppercase
              and symbol layers, plus shift / layer / space / backspace and
              accept / cancel. Long enough for a WiFi password (64 chars), and
              able to mask what it shows.

              Like ThemeApp, it is a plain app the project registers:

                  app.add(MENU_hidden, "Keyboard", APP_KEYBOARD,
                          KeyboardApp_handler, KeyboardApp_createBtns);

              Register it on a menu with no tab, so it never shows up in a
              generated menu list. To use it:

                  KeyboardApp_open("Password for MyAP", "", 63, true,
                                   APP_WIFI, onPasswordEntered);
                  app->newApp(APP_KEYBOARD);

              The callback runs on accept or cancel, and the keyboard then
              returns to the app id it was given.

              Layout is computed from the display's real size at build time, so
              it fits any panel the framework runs on.

              NOTE: this page needs KEYBOARDAPP_BUTTONS app-button slots — set
              GFX_APP_BUTTON_SIZE to at least that in the project config.
===========================================================================
*/
#ifndef EMBEDDEDGFX_KEYBOARDAPP_H
#define EMBEDDEDGFX_KEYBOARDAPP_H

#include <stdint.h>
#include "../gfx_config.h"

// Longest string the keyboard will collect (a WPA passphrase is 63).
#define KEYBOARDAPP_MAX_TEXT 64

// App-button slots this page occupies: 40 keys + 4 function + 2 action +
// title + field + the reveal toggle.
#define KEYBOARDAPP_BUTTONS 49

// Called once when the user accepts or cancels. `text` is the entry on accept,
// and the unchanged original on cancel.
typedef void (*KeyboardDoneFn)(const char* text, bool accepted);

// Arm the keyboard. Call immediately before switching to its app id.
//   title       shown above the field, e.g. "Password for MyAP" (may be null)
//   initialText pre-filled entry (may be null)
//   maxLen      characters allowed, capped at KEYBOARDAPP_MAX_TEXT
//   maskInput   start masked (asterisks). The field then carries a SHOW/HIDE
//               toggle, so the user can check what they typed
//   returnApp   app id to switch to once done
//   onDone      result callback (may be null)
void KeyboardApp_open(const char* title, const char* initialText, uint8_t maxLen,
                      bool maskInput, gfx_app_id_t returnApp, KeyboardDoneFn onDone);

uint8_t KeyboardApp_createBtns(void);
void    KeyboardApp_handler(int userInput);

// Current entry — valid inside the done callback, and after it.
const char* KeyboardApp_text(void);

#endif // EMBEDDEDGFX_KEYBOARDAPP_H
