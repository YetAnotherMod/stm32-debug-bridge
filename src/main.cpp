
#include PLATFORM_HEADER

#define GLOBAL_RESOURCES_DEFINITIONS
#include <global_resources.h>

#include <gpio.h>
#include <jtag.h>
#include <usb.h>
#include <shell.h>

static inline void PortsInit(void) {

    using namespace gpio;
    config::led.clockOn();
    config::led.writeHigh();
    config::led.configOutput(OutputType::gen_pp,
                             OutputSpeed::_50mhz);

    config::usbPins.clockOn();
    config::usbPins.write(true, true);
    config::usbPins.configOutput<0>(OutputType::gen_pp,
                                    OutputSpeed::_50mhz);
    config::usbPins.configOutput<1>(OutputType::gen_pp,
                                    OutputSpeed::_50mhz);
    config::jtagOut.clockOn();
    config::jtagOut.write(false, false, false);
    config::jtagOut.configOutput<0>(OutputType::gen_od,
                                    OutputSpeed::_50mhz);
    config::jtagOut.configOutput<1>(OutputType::gen_od,
                                    OutputSpeed::_50mhz);
    config::jtagOut.configOutput<2>(OutputType::gen_od,
                                    OutputSpeed::_50mhz);

    config::jtagIn.clockOn();
    config::jtagIn.write(true);
    config::jtagIn.configInput(InputType::pull_up_down);

    config::uartPins.clockOn();
    config::uartPins.write(false, true, true, true);
    config::uartPins.configInput<0>(InputType::floating);  // RX
    config::uartPins.configOutput<1>(OutputType::alt_pp,
                                     OutputSpeed::_50mhz); // TX
    config::uartPins.configInput<2>(InputType::pull_up_down); // CTS
    config::uartPins.configOutput<3>(OutputType::alt_pp,
                                     OutputSpeed::_50mhz); // RTS

    config::PortsInit();

}

extern "C" void __terminate() {
    __disable_irq();
    config::Panic();
#ifdef NDEBUG
    SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
#endif
    while (1)
        ;
}

int main() {
    shell::Shell<1024, config::CommandExecutor> sh;
    config::ClockInit();
    SystemCoreClockUpdate();
    PortsInit();
    __enable_irq();

    uint32_t lastDmaRxLen = 0;
    uint32_t lastDmaTxLen = 0;
    usb::cdcPayload::applyLineCoding();
    {
        auto [addr, len] = global::uartRx.dmaPush();
        config::uartDmaRx->CNDTR = len;
        lastDmaRxLen = len;
        config::uartDmaRx->CMAR = reinterpret_cast<uintptr_t>(addr);
        config::uartDmaRx->CPAR = reinterpret_cast<uintptr_t>(&config::uart->DR);
        config::uartDmaRx->CCR = DMA_CCR_PSIZE_1 | DMA_CCR_MINC | DMA_CCR_PSIZE_1 |
                             DMA_CCR_CIRC | DMA_CCR_EN;
    }

    usb::init();
    while (1) {
        if (usb::cdcPayload::isPendingApply()) {
            usb::cdcPayload::applyLineCoding();
        }
        if (uint32_t dmaRxLen = config::uartDmaRx->CNDTR;
            lastDmaRxLen - dmaRxLen != 0) {
            global::uartRx.dmaPushApply((lastDmaRxLen - dmaRxLen) %
                                        global::uartRx.capacity());
            lastDmaRxLen = dmaRxLen;
        }
        if (uint32_t dmaTxLen = config::uartDmaTx->CNDTR;
            (lastDmaTxLen - dmaTxLen != 0) || (!global::uartTx.empty())) {
            if (dmaTxLen == 0) {
                config::uartDmaTx->CCR = 0;
                global::uartTx.dmaPopApply(lastDmaTxLen);
                auto [addr, len] = global::uartTx.dmaPop();
                config::uartDmaTx->CNDTR = len;
                lastDmaTxLen = len;
                config::uartDmaTx->CMAR = reinterpret_cast<uintptr_t>(addr);
                config::uartDmaTx->CPAR = reinterpret_cast<uintptr_t>(&config::uart->DR);
                config::uartDmaTx->CCR =
                    DMA_CCR_PSIZE_1 | DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_EN;
            } else {
                global::uartTx.dmaPopApply(lastDmaTxLen - dmaTxLen);
                lastDmaTxLen = dmaTxLen;
            }
        }

        usb::regenerateTx();
        sh.exec(global::shellTx,global::shellRx);
        jtag::tick();
        if (!global::uartRx.empty()) {
            usb::sendFromFifo(usb::descriptor::InterfaceIndex::uart,
                              global::uartRx);
        }
        if (!global::shellRx.empty()) {
            usb::sendFromFifo(usb::descriptor::InterfaceIndex::shell,
                              global::shellRx);
        }
        if (!global::jtagRx.empty()) {
            usb::sendFromFifo(usb::descriptor::InterfaceIndex::jtag,
                              global::jtagRx);
        }
    }

    return 0;
}
