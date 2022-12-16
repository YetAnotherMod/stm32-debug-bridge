#include <global_resources.h>

namespace global {

fifo::Fifo<std::uint8_t, cdcFifoLenRx> uartRx;
fifo::Fifo<std::uint8_t, cdcFifoLenTx> uartTx;

fifo::Fifo<std::uint8_t, cdcFifoLenRx> shellRx;
fifo::Fifo<std::uint8_t, cdcFifoLenTx> shellTx;

fifo::Fifo<std::uint8_t, cdcFifoLenRx> jtagRx;
fifo::Fifo<std::uint8_t, cdcFifoLenTx> jtagTx;

gpio::Bulk<gpio::Port::a, 11, 12> usbPins;
gpio::Pin<gpio::Port::a, 6> led;
gpio::Bulk<gpio::Port::b, 6, 9, 12> jtagOut;
gpio::Bulk<gpio::Port::a, 5> jtagIn;


fifo::Fifo<std::uint32_t, bbFifoLenTx> bbTx;

gpio::Bulk<gpio::Port::a, 0, 1, 2, 3> uartPins;

LineCodingControl uartLineCoding = {
    115200, usb::cdcPayload::CharFormat::stopBit1,
    usb::cdcPayload::ParityType::none, usb::cdcPayload::DataBits::bits8, true};

} // namespace global
