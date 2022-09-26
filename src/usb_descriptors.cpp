#include <usb_callbacks.h>
#include <usb_descriptors.h>

namespace usb {
namespace descriptor {
const Device device(bcdVersion(2, 0, 0), Device::Class::misc,
                    Device::SubClass::iad, Device::Protocol::iad,
                    epSize::control, idVendor, idProduct, bcdVersion(0, 0, 0),
                    static_cast<std::uint8_t>(StringIndex::manufacturer),
                    static_cast<std::uint8_t>(StringIndex::product),
                    static_cast<std::uint8_t>(StringIndex::serial), 1);

const DeviceConfiguration deviceConfig;

static const String<static_cast<char16_t>(LanguageCode::en_US)> lang;
static const auto manufacturer = u"RC Module"_sd;
static const auto product = u"META (ModulE jTag Adapter)"_sd;
static const auto serial = u"NO SERIAL"_sd;
static const auto uartInterfaceName = u"uart"_sd;
static const auto shellInterfaceName = u"shell"_sd;
static const auto jtagInterfaceName = u"jtag"_sd;

const Base *strings[static_cast<std::size_t>(StringIndex::last)] = {
    &lang,
    &manufacturer,
    &product,
    &serial,
    &uartInterfaceName,
    &shellInterfaceName,
    &jtagInterfaceName};

const io::Endpoint endpoints[static_cast<std::size_t>(EndpointIndex::last)] = {
    {Endpoint::EpType::control, device.bMaxPacketSize, device.bMaxPacketSize,
     controlRxHandler, controlTxHandler, controlSetupHandler},
    {Endpoint::EpType::interrupt, 0,
     static_cast<std::uint8_t>(deviceConfig.uart.commEp.wMaxPacketSize),
     nullptr, nullptr, uartInterruptHandler},
    {Endpoint::EpType::bulk,
     static_cast<std::uint8_t>(deviceConfig.uart.dataRxEp.wMaxPacketSize),
     static_cast<std::uint8_t>(deviceConfig.uart.dataTxEp.wMaxPacketSize),
     uartRxHandler, uartTxHandler, nullptr},
    {Endpoint::EpType::interrupt, 0,
     static_cast<std::uint8_t>(deviceConfig.shell.commEp.wMaxPacketSize),
     nullptr, nullptr, shellInterruptHandler},
    {Endpoint::EpType::bulk,
     static_cast<std::uint8_t>(deviceConfig.shell.dataRxEp.wMaxPacketSize),
     static_cast<std::uint8_t>(deviceConfig.shell.dataTxEp.wMaxPacketSize),
     shellRxHandler, shellTxHandler, nullptr},
    {Endpoint::EpType::interrupt, 0,
     static_cast<std::uint8_t>(deviceConfig.jtag.commEp.wMaxPacketSize),
     nullptr, nullptr, jtagInterruptHandler},
    {Endpoint::EpType::bulk,
     static_cast<std::uint8_t>(deviceConfig.jtag.dataRxEp.wMaxPacketSize),
     static_cast<std::uint8_t>(deviceConfig.jtag.dataTxEp.wMaxPacketSize),
     jtagRxHandler, jtagTxHandler, nullptr}};

} // namespace descriptor
} // namespace usb
