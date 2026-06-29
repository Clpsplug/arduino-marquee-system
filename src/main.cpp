#include <Arduino.h>
#include <app.h>
#include <MC24FC.h>

using namespace ECP::ArduinoMarquee;

static MC24FC mc24_fc(0x50);
static char s[20];

void setup()
{
    //auto& app = getApplication();
    //appSetup(app);
    Serial.begin(115200);
}

void loop()
{
    //auto& app = getApplication();
    //appLoop(app);
    mc24_fc.init();
    auto addr = 0;
    std::uint8_t buf;
    auto result = mc24_fc.readByte(0, &buf);
    if (!result)
    {
        Serial.println("Byte read fail");
        for (;;)
        {
            // infinite loop
        }
    }
    sprintf(s, "%X: %d", addr, buf);
    Serial.println(s);
    for (auto i = 1; i < 32767; i++)
    {
        auto out = mc24_fc.readNext();
        sprintf(s, "%X: %d", i, out);
        Serial.println(s);
    }
}
