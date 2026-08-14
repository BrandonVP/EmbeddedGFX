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

// Thickness of the coloured underline strip below the menu bar.
#ifndef GFX_MENU_STRIP_HEIGHT
#define GFX_MENU_STRIP_HEIGHT 5
#endif

// --- App body region (everything below the menu bar) -----------------------
// Derived from the screen + menu bar, so the library's generated pages (menu
// grids, theme swatches) fill whatever resolution the project configures — the
// framework never assumes a fixed 480x320.
#define GFX_BODY_TOP    (GFX_MENU_BAR_HEIGHT)
#define GFX_BODY_WIDTH  (GFX_SCREEN_WIDTH)
#define GFX_BODY_HEIGHT (GFX_SCREEN_HEIGHT - GFX_MENU_BAR_HEIGHT)

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

// --- Color helper ----------------------------------------------------------
// Lighten (pct > 0, toward white) or darken (pct < 0, toward black) an RGB565
// color by |pct| percent. Handy for deriving gradient endpoints and shadow
// tints from a single theme color.
static inline uint16_t gfxShade(uint16_t c, int pct)
{
    int r = (c >> 11) & 0x1F;
    int g = (c >> 5) & 0x3F;
    int b = c & 0x1F;

    if (pct >= 0)
    {
        r += ((31 - r) * pct) / 100;
        g += ((63 - g) * pct) / 100;
        b += ((31 - b) * pct) / 100;
    }
    else
    {
        r += (r * pct) / 100;   // pct negative -> reduces
        g += (g * pct) / 100;
        b += (b * pct) / 100;
    }

    if (r < 0) r = 0; else if (r > 31) r = 31;
    if (g < 0) g = 0; else if (g > 63) g = 63;
    if (b < 0) b = 0; else if (b > 31) b = 31;

    return (uint16_t)((r << 11) | (g << 5) | b);
}

// --- Grid layout helper ----------------------------------------------------
// Computes the {x1,y1,x2,y2} rect of cell `index` in a cols x rows grid that
// fills the app body region, with fixed outer margins and inter-cell gaps.
// Cells fill row-major (0 = top-left, then left to right). Used for the
// generated menu pages and the theme swatches, so both scale with the
// configured resolution instead of using fixed 480x320 tables.
static inline void gfxGridRect(uint8_t index, uint8_t cols, uint8_t rows,
                               uint16_t marginX, uint16_t marginY,
                               uint16_t gapX, uint16_t gapY,
                               uint16_t& x1, uint16_t& y1, uint16_t& x2, uint16_t& y2)
{
    if (cols == 0) cols = 1;
    if (rows == 0) rows = 1;

    uint16_t cellW = (uint16_t)((GFX_BODY_WIDTH  - 2 * marginX - (cols - 1) * gapX) / cols);
    uint16_t cellH = (uint16_t)((GFX_BODY_HEIGHT - 2 * marginY - (rows - 1) * gapY) / rows);

    uint8_t col = index % cols;
    uint8_t row = index / cols;

    x1 = (uint16_t)(marginX + col * (cellW + gapX));
    y1 = (uint16_t)(GFX_BODY_TOP + marginY + row * (cellH + gapY));
    x2 = (uint16_t)(x1 + cellW);
    y2 = (uint16_t)(y1 + cellH);
}

#endif // EMBEDDEDGFX_CONFIG_H
