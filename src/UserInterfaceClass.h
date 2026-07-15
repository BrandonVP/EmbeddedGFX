/*
===========================================================================
Name        : UserInterfaceClass.h
Author      : Brandon Van Pelt
Description : Button object for EmbeddedGFX.

              A lightweight value type describing one on-screen button:
              geometry, colors, text, and packed boolean settings. The GUI
              reads these to draw and hit-test; app code sets them.
===========================================================================
*/
#ifndef EMBEDDEDGFX_USERINTERFACE_H
#define EMBEDDEDGFX_USERINTERFACE_H

#include <Arduino.h>
#include "gfx_config.h"

class UserInterfaceClass
{
private:
    // Coordinates
    uint16_t xStart = 0;
    uint16_t xStop = 0;
    uint16_t yStart = 0;
    uint16_t yStop = 0;

    // Colors
    uint16_t backgroundColor = 0;
    uint16_t borderColor = 0;
    uint16_t clickBorderColor = 0;
    uint16_t textColor = 0;
    uint8_t textSize = 11;

    char textBuffer[32];
    const char* btnText;
    uint8_t roundBtnRadius = 20;
    uint8_t align = 1;
    uint16_t clickReturn = 0;
    uint8_t btnSettings = 0;

    enum assignedBtnSettings {
        Settings_isClickable = 0,
        Settings_isRound,
        Settings_isPrintable,
        Settings_unused2,
        Settings_unused3,
        Settings_unused4,
        Settings_unused5,
        Settings_unused6
    };

public:
    void setButton(uint16_t xStart, uint16_t yStart, uint16_t xStop, uint16_t yStop, uint16_t clickReturn, bool isRound, const char* btnText);
    void setButton(uint16_t xStart, uint16_t yStart, uint16_t xStop, uint16_t yStop, uint16_t clickReturn, bool isRound, const char* btnText, uint8_t alignText);
    void setButton(uint16_t xStart, uint16_t yStart, uint16_t xStop, uint16_t yStop, uint16_t clickReturn, bool isRound, uint8_t radius, const char* btnText, uint8_t alignText, uint16_t btnC, uint16_t borderC, uint16_t textC);
    void setButton(uint16_t xStart, uint16_t yStart, uint16_t xStop, uint16_t yStop, uint16_t clickReturn, bool isRound, uint8_t radius, const char* btnText, uint8_t alignText, uint16_t btnC, uint16_t borderC, uint16_t clickBorderC, uint16_t textC);
    void setText(const char* btnText);
    void setShape(uint8_t isRound);
    void setClickable(uint8_t isClickable);
    void setPrintable(uint8_t isPrintable);
    void setBgColor(uint16_t color);
    void setBorderColor(uint16_t color);
    void setClickBorderColor(uint16_t color);
    void setTextColor(uint16_t color);
    void setTextSize(uint8_t size);
    void setClickReturn(uint16_t size);
    void setTextInt(int value);
    void setTextHex(uint32_t value);
    void setTextFloat(float value, int decimals = 2);
    void setTextFormat(const char* fmt, ...);

    uint16_t getXStart() const;
    uint16_t getXStop() const;
    uint16_t getYStart() const;
    uint16_t getYStop() const;
    uint16_t getClickReturn() const;
    bool getIsRound() const;
    bool getIsClickable() const;
    bool getIsPrintable() const;
    uint8_t getRadius() const;
    const char* getBtnText() const;
    uint8_t getAlign() const;
    uint16_t getBtnColor() const;
    uint16_t getBorderColor() const;
    uint16_t getClickBorderColor() const;
    uint16_t getTextColor() const;
    uint8_t getTextSize() const;
};

#endif // EMBEDDEDGFX_USERINTERFACE_H
