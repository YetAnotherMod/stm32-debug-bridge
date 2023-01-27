
#include <jtag.h>

#include <global_resources.h>

namespace jtag {
void tick(void) {
    const uint8_t i_am_alive_string[] = "I'm alive!\n";
    static bool waitRead = false;
    if (waitRead) {
        if (global::bbTx.empty()) {
            global::jtagRx.push( global::jtagIn.read() ? '1' : '0');
            waitRead = false;
        }
    }
    bool cont = !waitRead;
    while (cont) {
        auto [data, res] = global::jtagTx.popSafe();
        cont = res;
        if (res) {
            switch (data) {
            case 'B':
                global::led.writeLow();
                break;
            case 'b':
                global::led.writeHigh();
                break;
            case 'R':
                waitRead = true;
                cont = false;
                break;
            case '0':
                global::bbTx.push(
                    global::jtagOut.makeWriteWord(false, false, false));
                break;
            case '1':
                global::bbTx.push(
                    global::jtagOut.makeWriteWord(true, false, false));
                break;
            case '2':
                global::bbTx.push(
                    global::jtagOut.makeWriteWord(false, true, false));
                break;
            case '3':
                global::bbTx.push(
                    global::jtagOut.makeWriteWord(true, true, false));
                break;
            case '4':
                global::bbTx.push(
                    global::jtagOut.makeWriteWord(false, false, true));
                break;
            case '5':
                global::bbTx.push(
                    global::jtagOut.makeWriteWord(true, false, true));
                break;
            case '6':
                global::bbTx.push(
                    global::jtagOut.makeWriteWord(false, true, true));
                break;
            case '7':
                global::bbTx.push(
                    global::jtagOut.makeWriteWord(true, true, true));
                break;
            case 'r':
            case 't':
                global::bbTx.push(global::jtagOut.makeWriteWord<2>(true));
                break;

            case 's':
            case 'u':
                global::bbTx.push(global::jtagOut.makeWriteWord<2>(false));
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
