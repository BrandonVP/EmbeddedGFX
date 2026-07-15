/*
===========================================================================
Name        : UserInterfaceClass.cpp
Author      : Brandon Van Pelt
Description : Button object for EmbeddedGFX.
===========================================================================
*/
#include "UserInterfaceClass.h"
#include "Theme.h"

void UserInterfaceClass::setButton(uint16_t xStart, uint16_t yStart, uint16_t xStop, uint16_t yStop, uint16_t clickReturn, bool isRound, const char* btnText)
{
    this->btnSettings = 0;
    this->xStart = xStart;
    this->xStop = xStop;
    this->yStart = yStart;
    this->yStop = yStop;
    this->clickReturn = clickReturn;
    this->setClickable(true);
    this->setPrintable(true);
    this->setShape(isRound);
    this->roundBtnRadius = 20;
    this->align = Align_Text_Center;
    this->backgroundColor = gfxTheme.btnColor;
    this->borderColor = gfxTheme.btnBorder;
    this->clickBorderColor = gfxTheme.btnColor;
    this->textColor = gfxTheme.btnText;
    this->textSize = 11;
    if (btnText)
    {
        strncpy(textBuffer, btnText, sizeof(textBuffer));
        textBuffer[sizeof(textBuffer) - 1] = '\0';
        this->btnText = textBuffer;
    }
}

void UserInterfaceClass::setButton(uint16_t xStart, uint16_t yStart, uint16_t xStop, uint16_t yStop, uint16_t clickReturn, bool isRound, const char* btnText, uint8_t alignText)
{
    this->btnSettings = 0;
    this->xStart = xStart;
    this->xStop = xStop;
    this->yStart = yStart;
    this->yStop = yStop;
    this->clickReturn = clickReturn;
    this->setClickable(true);
    this->setPrintable(true);
    this->setShape(isRound);
    this->roundBtnRadius = 20;
    this->align = alignText;
    this->backgroundColor = gfxTheme.btnColor;
    this->borderColor = gfxTheme.btnBorder;
    this->clickBorderColor = gfxTheme.btnColor;
    this->textColor = gfxTheme.btnText;
    this->textSize = 11;
    if (btnText)
    {
        strncpy(textBuffer, btnText, sizeof(textBuffer));
        textBuffer[sizeof(textBuffer) - 1] = '\0';
        this->btnText = textBuffer;
    }
}

void UserInterfaceClass::setButton(uint16_t xStart, uint16_t yStart, uint16_t xStop, uint16_t yStop, uint16_t clickReturn, bool isRound, uint8_t radius, const char* btnText, uint8_t alignText, uint16_t btnC, uint16_t borderC, uint16_t textC)
{
    this->btnSettings = 0;
    this->xStart = xStart;
    this->xStop = xStop;
    this->yStart = yStart;
    this->yStop = yStop;
    this->clickReturn = clickReturn;
    this->setClickable(true);
    this->setPrintable(true);
    this->setShape(isRound);
    this->roundBtnRadius = radius;
    this->align = alignText;
    this->backgroundColor = btnC;
    this->borderColor = borderC;
    this->clickBorderColor = borderC;
    this->textColor = textC;
    this->textSize = 11;
    if (btnText)
    {
        strncpy(textBuffer, btnText, sizeof(textBuffer));
        textBuffer[sizeof(textBuffer) - 1] = '\0';
        this->btnText = textBuffer;
    }
}

void UserInterfaceClass::setButton(uint16_t xStart, uint16_t yStart, uint16_t xStop, uint16_t yStop, uint16_t clickReturn, bool isRound, uint8_t radius, const char* btnText, uint8_t alignText, uint16_t btnC, uint16_t borderC, uint16_t clickBorderC, uint16_t textC)
{
    this->btnSettings = 0;
    this->xStart = xStart;
    this->xStop = xStop;
    this->yStart = yStart;
    this->yStop = yStop;
    this->clickReturn = clickReturn;
    this->setClickable(true);
    this->setPrintable(true);
    this->setShape(isRound);
    this->roundBtnRadius = radius;
    this->align = alignText;
    this->backgroundColor = btnC;
    this->borderColor = borderC;
    this->clickBorderColor = clickBorderC;
    this->textColor = textC;
    this->textSize = 11;
    if (btnText)
    {
        strncpy(textBuffer, btnText, sizeof(textBuffer));
        textBuffer[sizeof(textBuffer) - 1] = '\0';
        this->btnText = textBuffer;
    }
}

void UserInterfaceClass::setText(const char* btnText)
{
    this->btnText = btnText;
}

void UserInterfaceClass::setShape(uint8_t isRound)
{
    if (isRound)
        btnSettings |= (1 << Settings_isRound);
    else
        btnSettings &= ~(1 << Settings_isRound);
}

void UserInterfaceClass::setClickable(uint8_t isClickable)
{
    if (isClickable)
        btnSettings |= (1 << Settings_isClickable);
    else
        btnSettings &= ~(1 << Settings_isClickable);
}

void UserInterfaceClass::setPrintable(uint8_t isPrintable)
{
    if (isPrintable)
        btnSettings |= (1 << Settings_isPrintable);
    else
        btnSettings &= ~(1 << Settings_isPrintable);
}

void UserInterfaceClass::setBgColor(uint16_t color)          { this->backgroundColor = color; }
void UserInterfaceClass::setBorderColor(uint16_t color)      { this->borderColor = color; }
void UserInterfaceClass::setClickBorderColor(uint16_t color) { this->clickBorderColor = color; }
void UserInterfaceClass::setTextColor(uint16_t color)        { this->textColor = color; }
void UserInterfaceClass::setTextSize(uint8_t size)           { this->textSize = size; }
void UserInterfaceClass::setClickReturn(uint16_t size)       { this->clickReturn = size; }

uint16_t UserInterfaceClass::getXStart() const      { return this->xStart; }
uint16_t UserInterfaceClass::getXStop() const       { return this->xStop; }
uint16_t UserInterfaceClass::getYStart() const      { return this->yStart; }
uint16_t UserInterfaceClass::getYStop() const       { return this->yStop; }
uint16_t UserInterfaceClass::getClickReturn() const { return this->clickReturn; }
bool UserInterfaceClass::getIsRound() const         { return CHECK_BIT(this->btnSettings, Settings_isRound); }
bool UserInterfaceClass::getIsClickable() const     { return CHECK_BIT(this->btnSettings, Settings_isClickable); }
bool UserInterfaceClass::getIsPrintable() const     { return CHECK_BIT(this->btnSettings, Settings_isPrintable); }
uint8_t UserInterfaceClass::getRadius() const       { return this->roundBtnRadius; }
const char* UserInterfaceClass::getBtnText() const  { return this->btnText; }
uint8_t UserInterfaceClass::getAlign() const        { return this->align; }
uint16_t UserInterfaceClass::getBtnColor() const    { return this->backgroundColor; }
uint16_t UserInterfaceClass::getClickBorderColor() const { return this->clickBorderColor; }
uint16_t UserInterfaceClass::getBorderColor() const { return this->borderColor; }
uint16_t UserInterfaceClass::getTextColor() const   { return this->textColor; }
uint8_t UserInterfaceClass::getTextSize() const     { return this->textSize; }

void UserInterfaceClass::setTextInt(int value)
{
    snprintf(textBuffer, sizeof(textBuffer), "%d", value);
    btnText = textBuffer;
}

void UserInterfaceClass::setTextHex(uint32_t value)
{
    snprintf(textBuffer, sizeof(textBuffer), "%lX", (unsigned long)value);
    btnText = textBuffer;
}

void UserInterfaceClass::setTextFloat(float value, int decimals)
{
    snprintf(textBuffer, sizeof(textBuffer), "%.*f", decimals, value);
    btnText = textBuffer;
}

void UserInterfaceClass::setTextFormat(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(textBuffer, sizeof(textBuffer), fmt, args);
    va_end(args);
    btnText = textBuffer;
}
