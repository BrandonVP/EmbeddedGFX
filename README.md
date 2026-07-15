# EmbeddedGFX

A hardware-agnostic touch-GUI framework for embedded color displays, extracted from the
ScanToolFD project so it can be shared across multiple firmware projects and updated
independently.

The framework provides:

- A **menu / app / button** system (`App`, `appManager`, `GUI`, `UserInterfaceClass`).
- **App registration at runtime** — apps are registered from the sketch's `setup()`, so the
  library never depends on any project-specific module.
- A **themeable color palette** (`GfxTheme`) plus an optional drop-in **theme-picker app**.
- **Display / touch abstraction** via the `IDisplay` and `ITouch` interfaces. Each project
  supplies a tiny adapter that wraps its own display/touch library.

## Supported targets

Designed for (added in stages):

1. **Teensy 4.1** (ILI9488_t3 + Adafruit_FT6206) — first target, see the demo project.
2. ESP32-S3
3. Arduino Due (UTFT + UTouch)

Because all hardware access goes through `IDisplay` / `ITouch`, adding a target only means
writing two small adapter classes — the framework code does not change.

## How a project uses it

```cpp
#include <EmbeddedGFX.h>
#include "MyDisplayAdapter.h"   // implements IDisplay
#include "MyTouchAdapter.h"     // implements ITouch

MyDisplayAdapter gfxDisplay(...);
MyTouchAdapter   gfxTouch(...);
App app;

// Project owns the button arrays (so it controls memory placement, e.g. DMAMEM on Teensy)
UserInterfaceClass appButtons[GFX_APP_BUTTON_SIZE];
UserInterfaceClass menuButtons[GFX_MENU_BUTTON_SIZE];

void setup() {
    GUI_I.begin(gfxDisplay, gfxTouch, appButtons, menuButtons);
    GUI_I.setApp(&app);

    // Register apps — the project defines its own menu + app id enums
    app.add(MENU_main, "Home",  APP_HOME,  home_handler,  home_createBtns);
    app.add(MENU_tools,"Themes",APP_THEME, ThemeApp_handler, ThemeApp_createBtns);
    app.init();
}
```

## Theming

`GfxTheme gfxTheme` is a single global palette the GUI reads its colors from. Override the
defaults at startup, or register the optional `ThemeApp` (in `src/apps/`) as a normal app on
whatever menu you like — or leave it out entirely to disable theme switching. Persistence is
injected via callbacks so the library never depends on SD/EEPROM.

## Layout

```
EmbeddedGFX/
  library.properties
  keywords.txt
  EmbeddedGFX.vcxitems      <- Visual Studio / Visual Micro shared project
  src/
    EmbeddedGFX.h           <- umbrella header
    gfx_config.h            <- screen size, button counts, layout coords (override-able)
    IDisplay.h  ITouch.h    <- hardware abstraction interfaces
    Theme.h / .cpp          <- GfxTheme palette + defaults
    UserInterfaceClass.*    <- button object
    appManager.*            <- app record (menu id, name, function pointers)
    App.*                   <- app registry + render state machine
    Gui.*                   <- drawing, touch handling, page/menu rendering
    apps/ThemeApp.*         <- optional theme-picker app module
  examples/HelloGFX/
```

Consume it from a project by adding this repo as a git submodule under the project's
`Libraries/` folder, or drop it into your Arduino `libraries/` directory.
