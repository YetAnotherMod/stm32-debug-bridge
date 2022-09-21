#pragma once

#include <usb_cdc.h>
#include <usb_std.h>

namespace usb {
namespace descriptor {
enum class StringIndex {
    none = 0x00,
    lang = 0x00,
    manufacturer,
    product,
    serial,
    uart_1_interface_name,
    uart_2_interface_name,
    uart_3_interface_name,
    last
};
enum class EndpointIndex{
    control = 0x00,
    uartInterrupt,
    uartData,
    shellInterrupt,
    shellData,
    jtagInterrupt,
    jtagData,
    last
};
enum class InterfaceIndex{
    uart,
    shell,
    jtag
};
struct DeviceConfiguration {
    Configuration config;
    Iad uartIad;
    Interface uart;
};
extern const Base *strings[static_cast<std::size_t>(StringIndex::last)];
extern const Device device;
}
} // namespace usb
