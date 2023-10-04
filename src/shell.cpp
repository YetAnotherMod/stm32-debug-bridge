#include <device-shell.h>
#include <shell.h>
#include <usb.h>
#include COMMANDS_HEADER

namespace deviceShell{
void tick (uint8_t c){
    const static char prompt[] = "> ";
    class backend{
        public:
        void push (char c){
            while(!global::shellRx.pushSafe(c))
                usb::regenerateRx();
        }
    };
    static shell::Shell<
        commands::CommandExecutor<backend>,
        prompt,
        60,
        8,
        shell::color::index::green,
        false,
        false,
        false,
        16
    > sh;
    sh.exec(c);
}
}
