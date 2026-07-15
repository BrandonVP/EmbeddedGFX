/*
===========================================================================
Name        : Theme.cpp
Author      : Brandon Van Pelt
Description : Default palette for EmbeddedGFX (dark blue).
===========================================================================
*/
#include "Theme.h"

// Dark Blue default — same values the original ScanToolFD booted with.
GfxTheme gfxTheme = {
    /* background   */ 0x424B,
    /* btnText      */ 0xFFFF,
    /* btnTextColor */ 0xBE18,
    /* btnBorder    */ 0x869B,
    /* btnColor     */ 0x0516,
    /* menuBg       */ 0x5B0E,
    /* menuBorder   */ 0x39E8,
    /* frameBorder  */ 0x8452,
    /* orangeBtn    */ 0xFC00,
    /* blackBtn     */ 0x0000
};
