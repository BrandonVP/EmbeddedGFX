/*
===========================================================================
Name        : Gui.h
Author      : Brandon Van Pelt
Description : GUI framework core for EmbeddedGFX.

              Singleton that owns touch state and all drawing. It talks to the
              hardware only through IDisplay / ITouch, and reads colors from
              gfxTheme, so it is fully project- and hardware-agnostic.

              Access it via the GUI_I macro.
===========================================================================
*/
#ifndef EMBEDDEDGFX_GUI_H
#define EMBEDDEDGFX_GUI_H

#include <Arduino.h>
#include "gfx_config.h"
#include "IDisplay.h"
#include "ITouch.h"
#include "Theme.h"
#include "UserInterfaceClass.h"

#define GUI_I GUI::instance()

class App; // forward declaration

class GUI
{
public:
    enum TouchState
    {
        TOUCH_IDLE,
        TOUCH_PRESSED,
        TOUCH_HELD,
        TOUCH_RELEASED
    };

    static GUI& instance();

    // Wire up hardware adapters and the project-owned button arrays. Call once
    // in setup() before any drawing.
    void begin(IDisplay& display, ITouch& touch,
               UserInterfaceClass* appButtons, UserInterfaceClass* menuButtons);

    void setApp(App* app);
    App* getApp() { return appPtr; }

    UserInterfaceClass* appButtons()  { return m_appButtons; }
    UserInterfaceClass* menuButtons() { return m_menuButtons; }

    // Thin passthroughs to the display adapter (used by App and app modules).
    void updateScreen();
    void useFrameBuffer(bool enable);

    bool Touch_getXY();
    void updateTouch();
    void setTouchedMenu(bool isTouched);
    void setTouchedBody(bool isTouched);
    bool getTouchedMenu();
    bool getTouchedBody();
    uint8_t getGraphicLoaderState();
    void setGraphicLoaderState(uint8_t newState);
    int getY();
    void setY(int newValue);
    int getX();
    void setX(int newValue);

    void updateButton(int index);
    void updateButtonText(int index, const char* newText);
    void drawRoundBtn(int x_start, int y_start, int x_stop, int y_stop, String buttonText, int btnBgColor, int btnBorderColor, int btnTxtColor, int align, int radius);
    void drawSquareBtn(int x_start, int y_start, int x_stop, int y_stop, String buttonText, int btnBgColor, int btnBorderColor, int btnTxtColor, int align);

    // --- Graphics helpers (built on the IDisplay primitives) ---------------
    // Vertical gradient fill (colorTop at y, blending to colorBottom at y+h-1).
    void fillGradientV(int x, int y, int w, int h, uint16_t colorTop, uint16_t colorBottom);
    // A "floating" rounded card: an offset shadow behind a filled rounded rect.
    void drawCard(int x, int y, int w, int h, int radius, uint16_t fill, uint16_t shadow, int shadowOffset = 3);
    void updateButtonPressVisual(const UserInterfaceClass& btn);
    void buttonMonitor(UserInterfaceClass* buttons, uint16_t size);
    int subMenuButtonMonitor(UserInterfaceClass* buttons, uint8_t size);
    void setTextSize(int textSize);
    bool drawPage(UserInterfaceClass* buttons, uint8_t& pos, uint8_t buttonsToPrint);
    void clearAppSpace();
    void setLoadBarRuntimeMS(uint16_t time);
    void startLoadBar(void);
    void stopLoadBarTimed(void);
    void sideLoadBarTimed(void);
    void loadBarPrintFrame(void);
    void LoadBarPrint(String buttonText, uint8_t fontSize, uint16_t textColor);
    void LoadBarPrint(String buttonText);
    void loadBarPrintFailed(void);
    void loadBarPrintPass(void);
    void drawSettingsPopUp(void);
    void clearSettingsPopUp(void);
    void LCD_Terminal(String buttonText);
    void clear_LCD_Terminal();
    void isButtonsEnabled(UserInterfaceClass* buttons, uint8_t start, uint8_t end, bool isDisabled);

    uint8_t graphicLoaderState = 0;
    uint16_t loadBarIndex = 471; // ends at 470
    uint32_t loadBarTimer = 0;
    uint16_t loarBarRunTimeMS = GFX_LOADBAR_DEFAULT_RUN_TIME;

    bool touchStartedInMenu = false;

    int activeMenuButtonIndex = -1;
    int activeBodyButtonIndex = -1;

    static char printString[14][64];

private:
    GUI() = default;

    IDisplay* m_display = nullptr;
    ITouch*   m_touch = nullptr;
    UserInterfaceClass* m_appButtons = nullptr;
    UserInterfaceClass* m_menuButtons = nullptr;
    App* appPtr = nullptr;

    int x = 0;
    int y = 0;
    bool isTouchedMenu = false;
    bool isTouchedBody = false;

    bool currentTouch = false;
    bool lastTouch = false;
    TouchState touchState = TOUCH_IDLE;
};

#endif // EMBEDDEDGFX_GUI_H
