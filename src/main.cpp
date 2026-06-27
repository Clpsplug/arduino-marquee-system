#include <Arduino.h>
#include <app.h>

using namespace ECP::ArduinoMarquee;

void setup()
{
    auto& app = getApplication();
    // TODO: Move this to error handler within App
    if (!app.matrix.init(0x70))
    {
        app.initialized = false;
        app.scr.setText("  I2C Error Display");
        app.scr.begin();
    } else
    {
        app.initialized = true;
    }

    delay(1000);
}

void loop()
{
    auto& app = getApplication();
    appLoop(app);
    if (!app.initialized)
    {
        app.scr.update();
        return;
    }
}
