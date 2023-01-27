#pragma once

#include <cdc_payload.h>
#include <fifo.h>
#include <gpio.h>

#ifdef GLOBAL_RESOURCES_DEFINITIONS
#define GLOBAL
#else
#define GLOBAL extern
#endif

namespace global {

constexpr std::size_t cdcFifoLenRx = 1024;
constexpr std::size_t cdcFifoLenTx = 1024;

constexpr std::size_t bbFifoLenRx = 1024;
constexpr std::size_t bbFifoLenTx = 1024;

GLOBAL fifo::Fifo<std::uint8_t, cdcFifoLenRx> uartRx;
GLOBAL fifo::Fifo<std::uint8_t, cdcFifoLenTx> uartTx;

GLOBAL fifo::Fifo<std::uint8_t, cdcFifoLenRx> shellRx;
GLOBAL fifo::Fifo<std::uint8_t, cdcFifoLenTx> shellTx;

GLOBAL fifo::Fifo<std::uint8_t, cdcFifoLenRx> jtagRx;
GLOBAL fifo::Fifo<std::uint8_t, cdcFifoLenTx> jtagTx;

GLOBAL fifo::Fifo<std::uint32_t, bbFifoLenTx> bbTx;

GLOBAL gpio::Bulk<gpio::Port::a, 11, 12> usbPins;
GLOBAL gpio::Pin<gpio::Port::a, 6> led;
// tdi,tms,tck
GLOBAL gpio::Bulk<gpio::Port::b, 6, 9, 12> jtagOut;
GLOBAL gpio::Pin<gpio::Port::a, 5> jtagIn;
// RX, TX, CTS, RTS
GLOBAL gpio::Bulk<gpio::Port::a, 3, 2, 0, 1> uartPins;

struct LineCodingControl : usb::cdcPayload::LineCoding {
    bool isChanged;
};

GLOBAL LineCodingControl uartLineCoding;

} // namespace global
