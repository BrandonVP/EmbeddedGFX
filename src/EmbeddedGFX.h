/*
===========================================================================
Name        : EmbeddedGFX.h
Author      : Brandon Van Pelt
Description : Umbrella header — include this one file to use the framework.

              #include <EmbeddedGFX.h>

              Optionally #define GFX_* config macros (see gfx_config.h) before
              including this, to override screen size / button counts / etc.
===========================================================================
*/
#ifndef EMBEDDEDGFX_H
#define EMBEDDEDGFX_H

#include "gfx_config.h"
#include "IDisplay.h"
#include "ITouch.h"
#include "Theme.h"
#include "UserInterfaceClass.h"
#include "appManager.h"
#include "App.h"
#include "Gui.h"

#endif // EMBEDDEDGFX_H
