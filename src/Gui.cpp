/*
===========================================================================
Name        : Gui.cpp
Author      : Brandon Van Pelt
Description : GUI framework core for EmbeddedGFX (see Gui.h).

              Ported from ScanToolFD's gui.cpp: all display/touch access now
              goes through m_display / m_touch, and colors come from gfxTheme.
===========================================================================
*/
#include "Gui.h"
#include "App.h"

char GUI::printString[14][64];

GUI& GUI::instance()
{
    static GUI instance;
    return instance;
}

void GUI::begin(IDisplay& display, ITouch& touch,
                UserInterfaceClass* appButtons, UserInterfaceClass* menuButtons)
{
    m_display = &display;
    m_touch = &touch;
    m_appButtons = appButtons;
    m_menuButtons = menuButtons;
}

void GUI::setApp(App* app)
{
    appPtr = app;
}

void GUI::updateScreen()          { m_display->updateScreen(); }
void GUI::useFrameBuffer(bool en) { m_display->useFrameBuffer(en); }

// Monitor screen touch. The adapter returns coordinates already in screen space.
bool GUI::Touch_getXY()
{
    if (m_touch->touched())
    {
        m_touch->getPoint(x, y);
        return true;
    }
    return false;
}

void GUI::updateTouch()
{
    currentTouch = Touch_getXY();

    if (y <= GFX_MENU_BAR_HEIGHT)
        setTouchedMenu(true);
    else
        setTouchedBody(true);

    if (currentTouch && !lastTouch)
        touchState = TOUCH_PRESSED;
    else if (currentTouch && lastTouch)
        touchState = TOUCH_HELD;
    else if (!currentTouch && lastTouch)
        touchState = TOUCH_RELEASED;
    else
        touchState = TOUCH_IDLE;

    lastTouch = currentTouch;
}

void GUI::setTouchedMenu(bool isTouched) { isTouchedMenu = isTouched; }
void GUI::setTouchedBody(bool isTouched) { isTouchedBody = isTouched; }
bool GUI::getTouchedMenu() { return isTouchedMenu; }
bool GUI::getTouchedBody() { return isTouchedBody; }
uint8_t GUI::getGraphicLoaderState() { return graphicLoaderState; }
void GUI::setGraphicLoaderState(uint8_t newState) { graphicLoaderState = newState; }
int GUI::getY() { return y; }
void GUI::setY(int newValue) { y = newValue; }
int GUI::getX() { return x; }
void GUI::setX(int newValue) { x = newValue; }

void GUI::updateButton(int index)
{
    graphicLoaderState = 0;
    while (drawPage(&m_appButtons[index], graphicLoaderState, 1));
}

void GUI::updateButtonText(int index, const char* newText)
{
    m_appButtons[index].setText(newText);
    graphicLoaderState = 0;
    while (drawPage(&m_appButtons[index], graphicLoaderState, 1));
}

// Draw a square button with round edges
void GUI::drawRoundBtn(int x_start, int y_start, int x_stop, int y_stop, String buttonText, int btnBgColor, int btnBorderColor, int btnTxtColor, int alignText, int radius)
{
    const uint8_t yMagicOffset = 6;
    int stringLength, buttonWidth, offset;

    m_display->fillRoundRect(x_start, y_start, (x_stop - x_start), (y_stop - y_start), radius, btnBgColor);
    m_display->drawRoundRect(x_start, y_start, (x_stop - x_start), (y_stop - y_start), radius, btnBorderColor);
    m_display->drawRoundRect(x_start + 1, y_start + 1, (x_stop - x_start) - 2, (y_stop - y_start) - 2, radius, btnBorderColor);
    m_display->setTextColor(btnTxtColor);

    char c[64];
    strncpy(c, buttonText.c_str(), sizeof(c) - 1);
    c[sizeof(c) - 1] = '\0';
    stringLength = m_display->strPixelLen(c);

    size_t length = buttonText.length();
    int lengthAsInt = static_cast<int>(length);
    const char* cString = buttonText.c_str();

    switch (alignText)
    {
    case 1: // Left
        m_display->drawString(cString, lengthAsInt, x_start + 5, y_start + ((y_stop - y_start) / 2) - 8);
        break;
    case 2: // Center
        buttonWidth = (x_stop) - (x_start);
        offset = (buttonWidth - stringLength) / 2;
        m_display->drawString(cString, lengthAsInt, (x_start + offset), y_start + ((y_stop - y_start) / 2) - yMagicOffset);
        break;
    case 3: // Right
        m_display->drawString(cString, lengthAsInt, x_stop - (stringLength + 5), y_start + ((y_stop - y_start) / 2) - 8);
        break;
    default:
        break;
    }
}

// Draw a square button
void GUI::drawSquareBtn(int x_start, int y_start, int x_stop, int y_stop, String buttonText, int btnBgColor, int btnBorderColor, int btnTxtColor, int alignText)
{
    const uint8_t yMagicOffset = 6;
    int stringLength, buttonWidth, offset;

    m_display->fillRect(x_start, y_start, (x_stop - x_start), (y_stop - y_start), btnBgColor);
    m_display->drawRect(x_start, y_start, (x_stop - x_start), (y_stop - y_start), btnBorderColor);
    m_display->setTextColor(btnTxtColor);

    char c[0xFF];
    strncpy(c, buttonText.c_str(), sizeof(c) - 1);
    c[sizeof(c) - 1] = '\0';
    stringLength = m_display->strPixelLen(c);

    size_t length = buttonText.length();
    int lengthAsInt = static_cast<int>(length);
    const char* cString = buttonText.c_str();

    switch (alignText)
    {
    case 1: // Left
        m_display->drawString(cString, lengthAsInt, x_start + 5, y_start + ((y_stop - y_start) / 2) - 8);
        break;
    case 2: // Center
        buttonWidth = (x_stop) - (x_start);
        offset = (buttonWidth - stringLength) / 2;
        m_display->drawString(cString, lengthAsInt, (x_start + offset), y_start + ((y_stop - y_start) / 2) - yMagicOffset);
        break;
    case 3: // Right
        m_display->drawString(cString, lengthAsInt, x_stop - (stringLength + 5), y_start + ((y_stop - y_start) / 2) - 8);
        break;
    default:
        break;
    }
}

// Vertical gradient via interpolated 1px-high strips (uses fillRect only, so it
// works on every backend). Cheap; on a framebuffered display it never flickers.
void GUI::fillGradientV(int x, int y, int w, int h, uint16_t colorTop, uint16_t colorBottom)
{
    if (h <= 0 || w <= 0) return;

    int r1 = (colorTop >> 11) & 0x1F, g1 = (colorTop >> 5) & 0x3F, b1 = colorTop & 0x1F;
    int r2 = (colorBottom >> 11) & 0x1F, g2 = (colorBottom >> 5) & 0x3F, b2 = colorBottom & 0x1F;
    int denom = (h > 1) ? (h - 1) : 1;

    for (int i = 0; i < h; i++)
    {
        int r = r1 + (r2 - r1) * i / denom;
        int g = g1 + (g2 - g1) * i / denom;
        int b = b1 + (b2 - b1) * i / denom;
        m_display->fillRect(x, y + i, w, 1, (uint16_t)((r << 11) | (g << 5) | b));
    }
}

// A raised card: an offset shadow rounded-rect behind the filled card.
void GUI::drawCard(int x, int y, int w, int h, int radius, uint16_t fill, uint16_t shadow, int shadowOffset)
{
    if (shadowOffset > 0)
        m_display->fillRoundRect(x + shadowOffset, y + shadowOffset, w, h, radius, shadow);
    m_display->fillRoundRect(x, y, w, h, radius, fill);
}

void GUI::updateButtonPressVisual(const UserInterfaceClass& btn)
{
    if (touchState == TOUCH_IDLE)
        return;

    if (touchState == TOUCH_PRESSED || touchState == TOUCH_HELD)
    {
        m_display->useFrameBuffer(false);

        if (btn.getIsRound())
        {
            if (!isTouchedMenu)
                m_display->drawRoundRect(btn.getXStart(), btn.getYStart(), btn.getXStop() - btn.getXStart(), btn.getYStop() - btn.getYStart(), btn.getRadius(), btn.getClickBorderColor());
        }
        else
        {
            m_display->drawRect(btn.getXStart(), btn.getYStart(), btn.getXStop() - btn.getXStart(), btn.getYStop() - btn.getYStart(), btn.getClickBorderColor());
        }

        m_display->useFrameBuffer(true);

        if (isTouchedMenu)
        {
            drawSquareBtn(0, GFX_MENU_BAR_HEIGHT - GFX_MENU_STRIP_HEIGHT, GFX_SCREEN_WIDTH, GFX_MENU_BAR_HEIGHT, "", gfxTheme.menuBorder, gfxTheme.menuBorder, gfxTheme.menuBorder, Align_Text_Center);
            drawSquareBtn(btn.getXStart(), GFX_MENU_BAR_HEIGHT - GFX_MENU_STRIP_HEIGHT, btn.getXStop(), GFX_MENU_BAR_HEIGHT, "", gfxTheme.btnColor, gfxTheme.btnColor, btn.getBorderColor(), Align_Text_Center);
        }
    }

    if (touchState == TOUCH_RELEASED)
    {
        if (btn.getIsRound())
        {
            if (!isTouchedMenu)
                m_display->drawRoundRect(btn.getXStart(), btn.getYStart(), btn.getXStop() - btn.getXStart(), btn.getYStop() - btn.getYStart(), btn.getRadius(), btn.getBorderColor());
        }
        else
        {
            m_display->drawRect(btn.getXStart(), btn.getYStart(), btn.getXStop() - btn.getXStart(), btn.getYStop() - btn.getYStart(), btn.getBorderColor());
        }

        m_display->useFrameBuffer(true);

        if (isTouchedMenu)
        {
            drawSquareBtn(btn.getXStart(), GFX_MENU_BAR_HEIGHT - GFX_MENU_STRIP_HEIGHT, btn.getXStop(), GFX_MENU_BAR_HEIGHT, "", gfxTheme.btnColor, gfxTheme.btnColor, btn.getBorderColor(), Align_Text_Center);
            drawSquareBtn(0, GFX_MENU_BAR_HEIGHT - GFX_MENU_STRIP_HEIGHT, GFX_SCREEN_WIDTH, GFX_MENU_BAR_HEIGHT, "", gfxTheme.menuBorder, gfxTheme.menuBorder, gfxTheme.menuBorder, Align_Text_Center);
            drawSquareBtn(btn.getXStart(), GFX_MENU_BAR_HEIGHT - GFX_MENU_STRIP_HEIGHT, btn.getXStop(), GFX_MENU_BAR_HEIGHT, "", gfxTheme.btnColor, gfxTheme.btnColor, btn.getBorderColor(), Align_Text_Center);
        }
        activeBodyButtonIndex = -1;
    }
}

void GUI::buttonMonitor(UserInterfaceClass* buttons, uint16_t size)
{
    if (!isTouchedMenu)
        return;

    for (uint8_t i = 0; i < size; i++)
    {
        if (touchState == TOUCH_PRESSED)
        {
            if ((x >= buttons[i].getXStart()) && (x <= buttons[i].getXStop()) &&
                (y >= buttons[i].getYStart()) && (y <= buttons[i].getYStop()))
            {
                if (buttons[i].getIsClickable())
                    activeMenuButtonIndex = i;
            }
        }
    }

    if (activeMenuButtonIndex != -1)
    {
        auto& btn = buttons[activeMenuButtonIndex];

        updateButtonPressVisual(btn);
        if (touchState == TOUCH_RELEASED)
        {
            if (appPtr) appPtr->newApp((gfx_app_id_t)btn.getClickReturn());
            activeMenuButtonIndex = -1;
            isTouchedMenu = false;
        }
    }
}

int GUI::subMenuButtonMonitor(UserInterfaceClass* buttons, uint8_t size)
{
    if (!isTouchedBody)
        return -1;

    for (uint8_t i = 0; i < size; i++)
    {
        if (touchState == TOUCH_PRESSED)
        {
            if ((x >= buttons[i].getXStart()) && (x <= buttons[i].getXStop()) && (y >= buttons[i].getYStart()) && (y <= buttons[i].getYStop()))
            {
                if (buttons[i].getIsClickable())
                    activeBodyButtonIndex = i;
            }
        }
    }

    if (activeBodyButtonIndex != -1 && buttons[activeBodyButtonIndex].getIsClickable())
    {
        auto& btn = buttons[activeBodyButtonIndex];

        updateButtonPressVisual(btn);

        if (touchState == TOUCH_RELEASED)
        {
            int ret = btn.getClickReturn();
            activeBodyButtonIndex = -1;
            isTouchedBody = false;
            return ret;
        }
    }
    return -1;
}

// Select font by logical text size. The adapter maps this to a real font.
void GUI::setTextSize(int textSize)
{
    m_display->setTextSize((uint8_t)textSize);
}

// Draw an App page, one button per call (cooperative, non-blocking).
bool GUI::drawPage(UserInterfaceClass* buttons, uint8_t& pos, uint8_t buttonsToPrint)
{
    if (pos == 0)
    {
        pos++;
        return true;
    }

    uint8_t btn = pos - 1;
    if (!buttons[btn].getIsPrintable())
    {
        pos++;
    }
    else if (buttonsToPrint != 0)
    {
        setTextSize(buttons[btn].getTextSize());

        if (buttons[btn].getIsRound())
            drawRoundBtn(buttons[btn].getXStart(), buttons[btn].getYStart(), buttons[btn].getXStop(), buttons[btn].getYStop(), buttons[btn].getBtnText(), buttons[btn].getBtnColor(), buttons[btn].getBorderColor(), buttons[btn].getTextColor(), buttons[btn].getAlign(), buttons[btn].getRadius());
        else
            drawSquareBtn(buttons[btn].getXStart(), buttons[btn].getYStart(), buttons[btn].getXStop(), buttons[btn].getYStop(), buttons[btn].getBtnText(), buttons[btn].getBtnColor(), buttons[btn].getBorderColor(), buttons[btn].getTextColor(), buttons[btn].getAlign());
        pos++;
    }

    return (pos > buttonsToPrint) ? false : true;
}

// Clear the app body (everything below the menu bar).
void GUI::clearAppSpace()
{
    m_display->fillRect(0, GFX_MENU_BAR_HEIGHT, GFX_SCREEN_WIDTH, GFX_SCREEN_HEIGHT - GFX_MENU_BAR_HEIGHT, gfxTheme.background);
}

void GUI::isButtonsEnabled(UserInterfaceClass* buttons, uint8_t start, uint8_t end, bool isDisabled)
{
    for (uint8_t i = start; i <= end; i++)
        buttons[i].setClickable(isDisabled);
}

void GUI::setLoadBarRuntimeMS(uint16_t time) { loarBarRunTimeMS = time; }

void GUI::startLoadBar(void)
{
    loadBarIndex = 250;
    loadBarTimer = millis();
    m_display->useFrameBuffer(false);
}

void GUI::stopLoadBarTimed(void) { loadBarIndex = GFX_LOADBAR_TURN_OFF; }

void GUI::sideLoadBarTimed(void)
{
    if (loadBarIndex > GFX_LOADBAR_TURN_OFF - 1)
        return;

    uint32_t temp = (int)ceil(loarBarRunTimeMS / 270);

    if (millis() - loadBarTimer > temp)
    {
        m_display->fillRect(loadBarIndex++, 181, 1, 38, gfxTheme.orangeBtn);
        loadBarTimer = millis();
    }
}

void GUI::loadBarPrintFrame(void)
{
    drawSquareBtn(244, 176, 476, 225, "", gfxTheme.menuBg, gfxTheme.blackBtn, gfxTheme.btnColor, Align_Text_Center);
    drawSquareBtn(248, 180, 472, 221, "", gfxTheme.background, gfxTheme.blackBtn, gfxTheme.background, Align_Text_Center);
}

void GUI::LoadBarPrint(String buttonText, uint8_t fontSize, uint16_t textColor)
{
    if (buttonText == "")
    {
        drawSquareBtn(244, 109, 476, 175, buttonText, gfxTheme.background, gfxTheme.background, gfxTheme.background, Align_Text_Center);
    }
    else
    {
        setTextSize(fontSize);
        drawSquareBtn(244, 110, 476, 225, "", gfxTheme.menuBg, gfxTheme.blackBtn, textColor, Align_Text_Center);
        drawSquareBtn(250, 112, 470, 160, buttonText, gfxTheme.menuBg, gfxTheme.menuBg, textColor, Align_Text_Center);
        m_display->setTextSize(11);
    }
}

void GUI::LoadBarPrint(String buttonText)
{
    m_display->setTextSize(10);
    drawSquareBtn(244, 125, 476, 145, buttonText, gfxTheme.background, gfxTheme.background, gfxTheme.btnTextColor, Align_Text_Center);
    m_display->setTextSize(11);
}

void GUI::loadBarPrintFailed(void)
{
    m_display->setTextSize(10);
    drawSquareBtn(244, 130, 476, 150, "Test Failed", gfxTheme.background, gfxTheme.background, gfxTheme.btnTextColor, Align_Text_Center);
    m_display->setTextSize(11);
}

void GUI::loadBarPrintPass(void)
{
    m_display->setTextSize(10);
    drawSquareBtn(244, 125, 476, 145, "Passed All Tests", gfxTheme.background, gfxTheme.background, gfxTheme.btnTextColor, Align_Text_Center);
    m_display->setTextSize(11);
}

void GUI::drawSettingsPopUp(void)
{
    drawSquareBtn(244, 315, 476, 316, "", gfxTheme.blackBtn, gfxTheme.blackBtn, gfxTheme.blackBtn, Align_Text_Center); // Bottom border
    for (int i = 0; i < 220; i++)
    {
        drawSquareBtn(245, 314 - i, 475, 315 - i, "", gfxTheme.menuBg, gfxTheme.menuBg, gfxTheme.menuBg, Align_Text_Center);       // Body
        drawSquareBtn(244, 314 - i, 245, 315 - i, "", gfxTheme.blackBtn, gfxTheme.blackBtn, gfxTheme.blackBtn, Align_Text_Center); // Left border
        drawSquareBtn(475, 314 - i, 476, 315 - i, "", gfxTheme.blackBtn, gfxTheme.blackBtn, gfxTheme.blackBtn, Align_Text_Center); // Right border
    }
    drawSquareBtn(244, 94, 476, 95, "", gfxTheme.blackBtn, gfxTheme.blackBtn, gfxTheme.blackBtn, Align_Text_Center); // Top border
}

void GUI::clearSettingsPopUp(void)
{
    for (int i = 0; i < 230; i++)
        drawSquareBtn(244, 90 + i, 476, 91 + i, "", gfxTheme.background, gfxTheme.background, gfxTheme.background, Align_Text_Center);
}

// Scrolling text terminal in the left column.
void GUI::LCD_Terminal(String buttonText)
{
    static uint8_t index = 0;

    memcpy(printString[index], buttonText.c_str(), buttonText.length());
    printString[index++][buttonText.length()] = '\0';

    m_display->setTextColor(gfxTheme.btnText);
    m_display->setTextSize(8);
    uint8_t temp = 0;
    if (index == 0)
        temp = 13;
    else
        temp = index - 1;
    if (index > 13)
        index = 0;

    for (uint16_t i = 0, j = 295, k = temp; i < 13; i++, j = j - 19)
    {
        if (appPtr) appPtr->backgroundProcess();

        int length = strlen(printString[k]);
        if (length > 24) length = 24;

        if (j == 295)
        {
            m_display->fillRect(4, j + 4, 226, 14, gfxTheme.menuBg);
            m_display->drawString(printString[k], length, 5, j + 6);
        }
        else
        {
            m_display->fillRect(5, j, 225, 11, gfxTheme.blackBtn);
            m_display->drawString(printString[k], length, 5, j);
        }

        if (k == 0 || k > 13)
            k = 13;
        else
            k--;
    }
    m_display->setTextSize(11);
}

void GUI::clear_LCD_Terminal()
{
    memset(printString, 0, sizeof(printString));
}
