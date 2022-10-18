#pragma once

#include <fifo.h>
#include <gpio.h>
#include <cdc_payload.h>

namespace global{

constexpr std::size_t cdcFifoLenRx = 1024;
constexpr std::size_t cdcFifoLenTx = 1024;

extern fifo::Fifo<std::uint8_t, cdcFifoLenRx> uartRx;
extern fifo::Fifo<std::uint8_t, cdcFifoLenTx> uartTx;

extern fifo::Fifo<std::uint8_t, cdcFifoLenRx> shellRx;
extern fifo::Fifo<std::uint8_t, cdcFifoLenTx> shellTx;

extern fifo::Fifo<std::uint8_t, cdcFifoLenRx> jtagRx;
extern fifo::Fifo<std::uint8_t, cdcFifoLenTx> jtagTx;

extern fifo::Fifo<std::uint32_t, 64> jtagCommands;
extern fifo::Fifo<bool, 64> jtagResponse;

extern gpio::Bulk<gpio::Port::a, 11, 12> usbPins;
extern gpio::Pin<gpio::Port::c, 13> led;
extern gpio::Bulk<gpio::Port::b, 7,8,9> jtagOut;
extern gpio::Bulk<gpio::Port::a, 5> jtagIn;

struct LineCodingControl : usb::cdcPayload::LineCoding{
    bool isChanged;
};

extern LineCodingControl uartLineCoding;

}