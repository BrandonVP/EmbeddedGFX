/*
===========================================================================
Name        : ITouch.h
Author      : Brandon Van Pelt
Description : Touch abstraction for EmbeddedGFX.

              The adapter is responsible for returning coordinates already in
              SCREEN space (same orientation as the display), so the framework
              needs no knowledge of rotation or panel calibration.
===========================================================================
*/
#ifndef EMBEDDEDGFX_ITOUCH_H
#define EMBEDDEDGFX_ITOUCH_H

#include <stdint.h>

class ITouch
{
public:
    virtual ~ITouch() {}

    // True while the panel is being touched.
    virtual bool touched() = 0;

    // Most recent touch point, in screen coordinates. Only valid immediately
    // after touched() returns true. Writes into the provided references.
    virtual void getPoint(int& x, int& y) = 0;
};

#endif // EMBEDDEDGFX_ITOUCH_H
