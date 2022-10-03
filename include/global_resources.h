#pragma once

#include <fifo.h>
#include <gpio.h>

extern gpio::Bulk<gpio::Port::a, 11, 12> usbPins;

extern gpio::Pin<gpio::Port::c, 13> led;
extern gpio::Bulk<gpio::Port::b, 7,8,9> jtagOut;
extern gpio::Bulk<gpio::Port::a, 5> jtagIn;
