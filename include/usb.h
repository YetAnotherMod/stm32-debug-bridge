#pragma once

#include <usb_callbacks.h>
#include <usb_cdc.h>
#include <usb_descriptors.h>
#include <usb_std.h>

namespace usb {
void reset(void);
void init(void);
} // namespace usb
