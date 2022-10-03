#include <global_resources.h>


gpio::Bulk<gpio::Port::a, 11, 12> usbPins;
gpio::Pin<gpio::Port::c, 13> led;
gpio::Bulk<gpio::Port::b, 7,8,9> jtagOut;
gpio::Bulk<gpio::Port::a, 5> jtagIn;
