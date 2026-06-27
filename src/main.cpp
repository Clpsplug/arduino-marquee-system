#include <Arduino.h>
#include <../lib/ht16k33-arduino/HT16K33.h>
#include <MatrixScroller.h>

struct App {
    HT16K33 matrix = HT16K33();
    Sprite16 smile = Sprite16( 6, 6, 18, 18, 0, 0, 33, 30);
    Sprite16 frown = Sprite16( 6, 6, 18, 18, 0, 0, 30, 33);
    bool initialized=false;
    MatrixScroller scr;
} s_app;

void spiral8(uint8_t, uint8_t);
void spiral16(uint8_t);
void pixel(uint8_t, uint8_t, uint8_t);
void setup() {
    if (!s_app.matrix.init(0x70)) {
        s_app.initialized=false;
        s_app.scr.setText("  I2C Error Display");
        s_app.scr.begin();
    }else {
        s_app.initialized=true;
    }

    delay(1000);
}

void loop() {
    if (!s_app.initialized) {
        s_app.scr.update();
        return;
    }
    s_app.matrix.drawSprite16(s_app.smile, 1, 1);
    s_app.matrix.drawSprite16(s_app.frown, 9, 1);
    s_app.matrix.write();

    for (int brightness = 0; brightness < 9; brightness++) {
        s_app.matrix.setBrightness(brightness);
        delay(40);
    }
    delay(1000);
    for (int brightness = 8; brightness >= 0; brightness--) {
        s_app.matrix.setBrightness(brightness);
        delay(40);
    }
    s_app.matrix.clear();
    s_app.matrix.setBrightness(8);
    // draw some spirals
    spiral8(0, 1);
    spiral8(8, 1);
    spiral16(0);
    delay(200);

    // wipe
    uint8_t row;
    s_app.matrix.clear();
    for (row = 0; row < 8; row++)
    {
        s_app.matrix.setRow(row, 0xFFFF);
        s_app.matrix.write();
        delay(50);
    }
    for (row = 0; row < 8; row++)
    {
        s_app.matrix.setRow(row, 0);
        s_app.matrix.write();
        delay(50);
    }

    // another wipe
    uint8_t col;
    for (col = 0; col < 16; col++)
    {
        s_app.matrix.setColumn(col, 0xFF);
        s_app.matrix.write();
        delay(50);
    }
    for (col = 0; col < 16; col++)
    {
        s_app.matrix.setColumn(col, 0);
        s_app.matrix.write();
        delay(50);
    }
    delay(1000);
    s_app.matrix.clear();
}

void spiral8(uint8_t start, uint8_t value)
{
    //start looping
    int row;
    int col;
    for (int fiddle = 0; fiddle < 4; fiddle++)
    {
        for (row = (start + fiddle); row < (start + (7 - fiddle)); row++) { pixel(row, fiddle, value); }
        for (col = fiddle; col < (7 - fiddle); col++) { pixel(start + (7 - fiddle), col, value); }
        for (row = (start + (7 - fiddle)); row > (start + fiddle); row--) { pixel(row, (7-fiddle), value); }
        for (col = (7 - fiddle); col > fiddle; col--) { pixel(start + fiddle, col, value); }
    }
}

void spiral16(uint8_t value)
{
    //start looping
    int row;
    int col;
    for (int fiddle = 0; fiddle < 4; fiddle++)
    {
        for (row = fiddle; row < (15 - fiddle); row++) { pixel(row, fiddle, value); }
        for (col = fiddle; col < (7 - fiddle); col++)  { pixel((15 - fiddle), col, value); }
        for (row = (15 - fiddle); row > fiddle; row--) { pixel(row, (7 - fiddle), value); }
        for (col = (7 - fiddle); col > fiddle; col--)  { pixel(fiddle, col, value); }
    }
}

void pixel( uint8_t row, uint8_t col, uint8_t val )
{
    s_app.matrix.setPixel(row, col, val);
    s_app.matrix.write();
    delay(10);
}
