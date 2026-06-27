#include "app.h"
#include <Arduino.h>

using namespace ECP::ArduinoMarquee;

namespace
{

void pixel(HT16K33& matrix, const uint8_t row, const uint8_t col, const uint8_t val)
{
    matrix.setPixel(row, col, val);
    matrix.write();
    delay(10);
}

void spiral8(HT16K33& matrix, const uint8_t start, const uint8_t value)
{
    //start looping
    int row;
    int col;
    for (auto fiddle = 0; fiddle < 4; fiddle++)
    {
        for (row = (start + fiddle); row < (start + (7 - fiddle)); row++)
        {
            pixel(matrix, row, fiddle, value);
        }
        for (col = fiddle; col < (7 - fiddle); col++)
        {
            pixel(matrix, start + (7 - fiddle), col, value);
        }
        for (row = (start + (7 - fiddle)); row > (start + fiddle); row--)
        {
            pixel(matrix, row, (7 - fiddle), value);
        }
        for (col = (7 - fiddle); col > fiddle; col--)
        {
            pixel(matrix, start + fiddle, col, value);
        }
    }
}

void spiral16(HT16K33& matrix, const uint8_t value)
{
    //start looping
    int row;
    int col;
    for (auto fiddle = 0; fiddle < 4; fiddle++)
    {
        for (row = fiddle; row < (15 - fiddle); row++)
        {
            pixel(matrix, row, fiddle, value);
        }
        for (col = fiddle; col < (7 - fiddle); col++)
        {
            pixel(matrix, (15 - fiddle), col, value);
        }
        for (row = (15 - fiddle); row > fiddle; row--)
        {
            pixel(matrix, row, (7 - fiddle), value);
        }
        for (col = (7 - fiddle); col > fiddle; col--)
        {
            pixel(matrix, fiddle, col, value);
        }
    }
}

}

AppRef getApplication()
{
    static App app;
    return app;
}

void appLoop(App& app)
{

    app.matrix.drawSprite16(app.smile, 1, 1);
    app.matrix.drawSprite16(app.frown, 9, 1);
    app.matrix.write();

    for (auto brightness = 0; brightness < 9; brightness++)
    {
        app.matrix.setBrightness(brightness);
        delay(40);
    }
    delay(1000);
    for (auto brightness = 8; brightness >= 0; brightness--)
    {
        app.matrix.setBrightness(brightness);
        delay(40);
    }
    app.matrix.clear();
    app.matrix.setBrightness(8);
    // draw some spirals
    spiral8(app.matrix, 0, 1);
    spiral8(app.matrix, 8, 1);
    spiral16(app.matrix, 0);
    delay(200);

    // wipe
    uint8_t row;
    app.matrix.clear();
    for (row = 0; row < 8; row++)
    {
        app.matrix.setRow(row, 0xFFFF);
        app.matrix.write();
        delay(50);
    }
    for (row = 0; row < 8; row++)
    {
        app.matrix.setRow(row, 0);
        app.matrix.write();
        delay(50);
    }

    // another wipe
    uint8_t col;
    for (col = 0; col < 16; col++)
    {
        app.matrix.setColumn(col, 0xFF);
        app.matrix.write();
        delay(50);
    }
    for (col = 0; col < 16; col++)
    {
        app.matrix.setColumn(col, 0);
        app.matrix.write();
        delay(50);
    }
    delay(1000);
    app.matrix.clear();
}
