#pragma once

#include <usb_callbacks.h>
#include <usb_cdc.h>
#include <usb_descriptors.h>
#include <usb_std.h>
#include <cdc_payload.h>
#include <global_resources.h>

namespace usb {
void reset(void);
void init(void);
bool sendFromFifo(descriptor::InterfaceIndex ind, fifo::Fifo<std::uint8_t, global::cdcFifoLenRx> &buf);
void regenerateTx(void);
void polling(void);
} // namespace usb
