
#include <usb_std.h>
#include <gpio.h>

int main() {
    gpio::Pin<gpio::Port::c, 13> led;
    led.clockOn();
    led.configOutput(gpio::OutputType::gen_pp, gpio::OutputSpeed::_2mhz);
    led.writeHigh();
    while (1)
        ;
    return 0;
}
