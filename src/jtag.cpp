
#include <jtag.h>

#include <global_resources.h>

namespace jtag {
void tick(void) {
    const uint8_t i_am_alive_string[] = "I'm alive!\r\n";
    bool cont = true;
    while (cont) {
        auto [data, res] = global::jtagTx.popSafe();
        cont = res;
        if (res) {
            switch (data) {
            case 'B':
                config::led.writeLow();
                break;
            case 'b':
                config::led.writeHigh();
                break;
            case 'R':
                config::jtagOut.writeRaw(0);
                global::jtagRx.push( config::jtagIn.read() ? '1' : '0');
                break;
            case '0':
                    config::jtagOut.write(false, false, false);
                break;
            case '1':
                    config::jtagOut.write(true, false, false);
                break;
            case '2':
                    config::jtagOut.write(false, true, false);
                break;
            case '3':
                    config::jtagOut.write(true, true, false);
                break;
            case '4':
                    config::jtagOut.write(false, false, true);
                break;
            case '5':
                    config::jtagOut.write(true, false, true);
                break;
            case '6':
                    config::jtagOut.write(false, true, true);
                break;
            case '7':
                    config::jtagOut.write(true, true, true);
                break;
            case 'r':
            case 't':
                config::jtagOut.write<2>(true);
                break;

            case 's':
            case 'u':
                config::jtagOut.write<2>(false);
                break;
            case 'h':
                global::jtagRx.write(i_am_alive_string,
                                     sizeof(i_am_alive_string) - 1);
                break;

            default:
                break;
            }
        }
    }
}
} // namespace jtag
