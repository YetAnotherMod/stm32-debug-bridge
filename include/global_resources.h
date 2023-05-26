#pragma once

#include <cdc_payload.h>
#include <fifo.h>
#include <gpio.h>

#ifdef GLOBAL_RESOURCES_DEFINITIONS
#define GLOBAL
#else
#define GLOBAL extern
#endif

#ifdef GLOBAL_RESOURCES_DEFINITIONS
#define GLOBAL_INIT(type,obj,params...)type obj(params)
#else
#define GLOBAL_INIT(type,obj,params...) extern type obj
#endif

#include <config.h>

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

struct LineCodingControl : usb::cdcPayload::LineCoding {
    bool isChanged;
};

GLOBAL LineCodingControl uartLineCoding;

} // namespace global
