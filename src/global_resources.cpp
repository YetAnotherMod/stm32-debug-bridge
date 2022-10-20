#include <global_resources.h>

namespace global {

fifo::Fifo<std::uint8_t, cdcFifoLenRx> uartRx;
fifo::Fifo<std::uint8_t, cdcFifoLenTx> uartTx;

fifo::Fifo<std::uint8_t, cdcFifoLenRx> shellRx;
fifo::Fifo<std::uint8_t, cdcFifoLenTx> shellTx;

fifo::Fifo<std::uint8_t, cdcFifoLenRx> jtagRx;
fifo::Fifo<std::uint8_t, cdcFifoLenTx> jtagTx;

fifo::Fifo<std::uint32_t, 64> jtagCommands;
fifo::Fifo<bool, 64> jtagResponse;

gpio::Bulk<gpio::Port::a, 11, 12> usbPins;
gpio::Pin<gpio::Port::c, 13> led;
gpio::Bulk<gpio::Port::b, 12, 6, 9> jtagOut;
gpio::Bulk<gpio::Port::a, 5> jtagIn;

gpio::Bulk<gpio::Port::a, 0,1,2,3> uartPins;

LineCodingControl uartLineCoding = {
    115200, usb::cdcPayload::CharFormat::stopBit1,
    usb::cdcPayload::ParityType::none, usb::cdcPayload::DataBits::bits8, true};

} // namespace global
