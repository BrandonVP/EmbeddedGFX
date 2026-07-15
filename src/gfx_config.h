/*
===========================================================================
Name        : gfx_config.h
Author      : Brandon Van Pelt
Description : Compile-time configuration for EmbeddedGFX.

              Every value here has a sensible default and can be overridden by
              the project by #define-ing it BEFORE including <EmbeddedGFX.h>
              (e.g. from a build flag or a project config header).
===========================================================================
*/
#ifndef EMBEDDEDGFX_CONFIG_H
#define EMBEDDEDGFX_CONFIG_H

#include <stdint.h>

// --- Screen geometry -------------------------------------------------------
#ifndef GFX_SCREEN_WIDTH
#define GFX_SCREEN_WIDTH  480
#endif
#ifndef GFX_SCREEN_HEIGHT
#define GFX_SCREEN_HEIGHT 320
#endif

// --- Menu bar geometry (top strip that holds the main-menu buttons) --------
#ifndef GFX_MENU_BAR_HEIGHT
#define GFX_MENU_BAR_HEIGHT 50   // y < this == menu bar, y >= this == app body
#endif

// --- Button array sizing ---------------------------------------------------
// The project allocates the button arrays (so it controls memory placement,
// e.g. DMAMEM on Teensy) and passes them to GUI::begin(). These are the
// element counts the project should allocate.
#ifndef GFX_APP_BUTTON_SIZE
#define GFX_APP_BUTTON_SIZE 30
#endif
#ifndef GFX_MENU_BUTTON_SIZE
#define GFX_MENU_BUTTON_SIZE 3
#endif

// Largest number of auto-laid-out buttons on a generated menu page.
#ifndef GFX_MAX_BUTTONS_LAYOUT
#define GFX_MAX_BUTTONS_LAYOUT 8
#endif

// --- Load bar --------------------------------------------------------------
#ifndef GFX_LOADBAR_TURN_OFF
#define GFX_LOADBAR_TURN_OFF 471
#endif
#ifndef GFX_LOADBAR_DEFAULT_RUN_TIME
#define GFX_LOADBAR_DEFAULT_RUN_TIME 8000
#endif

// --- Generic id types ------------------------------------------------------
// The framework does not know the project's app/menu names. The project
// defines its own enums; these typedefs are what the framework stores.
typedef uint8_t gfx_app_id_t;
typedef uint8_t gfx_menu_id_t;

// Sentinel meaning "no app registered at this slot".
#define GFX_APP_ID_NONE (0xFF)

// --- Text alignment --------------------------------------------------------
enum textAlignment
{
    Align_Text_Left   = 1,
    Align_Text_Center = 2,
    Align_Text_Right  = 3
};

// Aliases kept for source compatibility with existing app code.
#ifndef ALIGN_LEFT
#define ALIGN_LEFT   1
#define ALIGN_CENTER 2
#define ALIGN_RIGHT  3
#endif

// Test a single bit of a value.
#ifndef CHECK_BIT
#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#endif

// --- Menu button auto-layout coordinate tables -----------------------------
// {xStart, yStart, xStop, yStop} for 4 / 6 / 8 button generated menu pages.
static const uint16_t MENU_4Center[4][4] = {
    {145,  75, 335, 125}, {145, 135, 335, 185},
    {145, 195, 335, 245}, {145, 255, 335, 305}
};

static const uint16_t MENU_6Grid[6][4] = {
    { 45,  95, 230, 145}, {250,  95, 435, 145},
    { 45, 155, 230, 205}, {250, 155, 435, 205},
    { 45, 215, 230, 265}, {250, 215, 435, 265}
};

static const uint16_t MENU_8Grid[8][4] = {
    { 45,  75, 230, 125}, {250,  75, 435, 125},
    { 45, 135, 230, 185}, {250, 135, 435, 185},
    { 45, 195, 230, 245}, {250, 195, 435, 245},
    { 45, 255, 230, 305}, {250, 255, 435, 305}
};

#endif // EMBEDDEDGFX_CONFIG_H
