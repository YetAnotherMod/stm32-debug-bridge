#pragma once

#include <cdc_payload.h>
#include <fifo.h>
#include <gpio.h>

namespace global {

constexpr std::size_t cdcFifoLenRx = 1024;
constexpr std::size_t cdcFifoLenTx = 1024;

constexpr std::size_t bbFifoLenRx = 1024;
constexpr std::size_t bbFifoLenTx = 1024;

extern fifo::Fifo<std::uint8_t, cdcFifoLenRx> uartRx;
extern fifo::Fifo<std::uint8_t, cdcFifoLenTx> uartTx;

extern fifo::Fifo<std::uint8_t, cdcFifoLenRx> shellRx;
extern fifo::Fifo<std::uint8_t, cdcFifoLenTx> shellTx;

extern fifo::Fifo<std::uint8_t, cdcFifoLenRx> jtagRx;
extern fifo::Fifo<std::uint8_t, cdcFifoLenTx> jtagTx;

extern fifo::Fifo<std::uint32_t, bbFifoLenTx> bbTx;

extern gpio::Bulk<gpio::Port::a, 11, 12> usbPins;
extern gpio::Pin<gpio::Port::a, 6> led;
// tdi,tms,tck
extern gpio::Bulk<gpio::Port::b, 6, 9, 12> jtagOut;
extern gpio::Bulk<gpio::Port::a, 5> jtagIn;
extern gpio::Bulk<gpio::Port::a, 0, 1, 2, 3> uartPins;

struct LineCodingControl : usb::cdcPayload::LineCoding {
    bool isChanged;
};

extern LineCodingControl uartLineCoding;

} // namespace global