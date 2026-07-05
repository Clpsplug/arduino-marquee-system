#ifndef APP_H
#define APP_H
#include <HT16K33.h>
#include <MatrixScroller.h>

namespace ECP::ArduinoMarquee
{
struct App;

using AppRef = App&;

AppRef getApplication();

void appSetup(App& app);

void appLoop(App& app);

}

#endif
