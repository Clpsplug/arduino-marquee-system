#ifndef APP_H
#define APP_H
#include <HT16K33.h>
#include <MatrixScroller.h>

namespace ECP::ArduinoMarquee
{
// TODO: Make this truly opaque
struct App
{
    HT16K33 matrix = HT16K33();
    Sprite16 smile = Sprite16(6, 6, 18, 18, 0, 0, 33, 30);
    Sprite16 frown = Sprite16(6, 6, 18, 18, 0, 0, 30, 33);
    bool initialized = false;
    MatrixScroller scr;
};

using AppRef = App&;

AppRef getApplication();

void appLoop(App& app);

}

#endif
