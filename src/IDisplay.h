/*
===========================================================================
Name        : IDisplay.h
Author      : Brandon Van Pelt
Description : Display abstraction for EmbeddedGFX.

              The GUI framework draws exclusively through this interface, so it
              has no dependency on any particular TFT library. Each project
              provides one adapter class that wraps its display driver
              (e.g. ILI9488_t3, UTFT, LovyanGFX) and implements these methods.

              Colors are 16-bit RGB565 to match the common Adafruit-GFX-style
              drivers used across the supported targets.
===========================================================================
*/
#ifndef EMBEDDEDGFX_IDISPLAY_H
#define EMBEDDEDGFX_IDISPLAY_H

#include <stdint.h>
#include "gfx_config.h"

class IDisplay
{
public:
    virtual ~IDisplay() {}

    // --- Rectangles --------------------------------------------------------
    virtual void fillRect(int x, int y, int w, int h, uint16_t color) = 0;
    virtual void drawRect(int x, int y, int w, int h, uint16_t color) = 0;
    virtual void fillRoundRect(int x, int y, int w, int h, int radius, uint16_t color) = 0;
    virtual void drawRoundRect(int x, int y, int w, int h, int radius, uint16_t color) = 0;

    // --- Text --------------------------------------------------------------
    // Select the font used for subsequent text. 'logicalSize' is a point-ish
    // size; the adapter maps it to whatever font asset it has. This is how the
    // framework stays font-library agnostic.
    virtual void setTextSize(uint8_t logicalSize) = 0;
    virtual void setTextColor(uint16_t color) = 0;

    // Draw 'len' characters of 'str' with the top-left at (x, y) in the
    // currently selected font/color.
    virtual void drawString(const char* str, int len, int x, int y) = 0;

    // Pixel width of 'str' in the currently selected font. Used for centering.
    virtual int strPixelLen(const char* str) = 0;

    // --- Frame buffer (optional; no-op on drivers without one) -------------
    virtual void useFrameBuffer(bool enable) = 0;
    virtual void updateScreen() = 0;

    // --- Geometry ----------------------------------------------------------
    virtual int width()  { return GFX_SCREEN_WIDTH;  }
    virtual int height() { return GFX_SCREEN_HEIGHT; }
};

#endif // EMBEDDEDGFX_IDISPLAY_H
