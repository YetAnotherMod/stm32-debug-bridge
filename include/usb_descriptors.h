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
    uartInterfaceName,
    shellInterfaceName,
    jtagInterfaceName,
    last
};
enum class EndpointIndex {
    control = 0x00,
    uartInterrupt,
    uartData,
    shellInterrupt,
    shellData,
    jtagInterrupt,
    jtagData,
    last
};
namespace epSize {
constexpr std::uint16_t control = sizeof(::usb::Setup);
constexpr std::uint16_t interrupt = 16;
constexpr std::uint16_t small = 32;
constexpr std::uint16_t large = 64;
} // namespace epSize

constexpr std::uint16_t idVendor = 0x1209;
constexpr std::uint16_t idProduct = 0xfffe;

enum class InterfaceIndex { uart, shell, jtag };

struct DeviceConfiguration : Configuration {

    struct SingleAcm {
        Iad iad;
        Interface comm;
        cdc::descriptor::Header header;
        cdc::descriptor::CallMgmt mgmt;
        cdc::descriptor::Acm acm;
        cdc::descriptor::Union union_;
        Endpoint commEp;
        Interface data;
        Endpoint dataRxEp;
        Endpoint dataTxEp;
        constexpr SingleAcm(InterfaceIndex iInd, cdc::serialInd sInd,
                            EndpointIndex ecInd, EndpointIndex edInd)
            : iad(static_cast<uint8_t>(iInd), 2,
                  static_cast<uint8_t>(Device::Class::comm), cdc::Subclass::acm,
                  cdc::Protocol::protocolDefault,
                  static_cast<uint8_t>(StringIndex::none)),
              comm(static_cast<uint8_t>(sInd) * 2, 0, 1, Device::Class::comm,
                   cdc::Subclass::acm, cdc::Protocol::protocolDefault,
                   static_cast<uint8_t>(StringIndex::uartInterfaceName)),
              header(), mgmt(0, static_cast<uint8_t>(sInd) * 2 + 1),
              acm(cdc::acmCapability::default_),
              union_(static_cast<uint8_t>(sInd) * 2,
                     static_cast<uint8_t>(sInd) * 2 + 1),
              commEp(Endpoint::directionIn | static_cast<uint8_t>(ecInd),
                     Endpoint::EpType::interrupt,
                     epSize::interrupt, cdc::linesPollingInterval),
              data(static_cast<uint8_t>(sInd) * 2 + 1, 0, 2,
                   Device::Class::cdc_data, cdc::Subclass::none,
                   cdc::Protocol::none,
                   static_cast<uint8_t>(StringIndex::none)),
              dataRxEp(Endpoint::directionOut | static_cast<uint8_t>(edInd),
                       Endpoint::EpType::bulk, epSize::large, 0),
              dataTxEp(Endpoint::directionIn | static_cast<uint8_t>(edInd),
                       Endpoint::EpType::bulk, epSize::small, 0) {}
    } __attribute__((packed));

    SingleAcm uart;
    SingleAcm shell;
    SingleAcm jtag;

    constexpr DeviceConfiguration()
        : Configuration(sizeof(*this), cdc::numPorts * 2, 1,
                        static_cast<uint8_t>(StringIndex::none),
                        Configuration::bmAttrReserved, 100_ma),
          uart(InterfaceIndex::uart, cdc::serialInd::uart,
               EndpointIndex::uartInterrupt, EndpointIndex::uartData),
          shell(InterfaceIndex::shell, cdc::serialInd::shell,
                EndpointIndex::shellInterrupt, EndpointIndex::shellData),
          jtag(InterfaceIndex::jtag, cdc::serialInd::jtag,
               EndpointIndex::jtagInterrupt, EndpointIndex::jtagData) {}

} __attribute__((packed));
extern const Device device;
extern const DeviceConfiguration deviceConfig;
extern const Base *strings[static_cast<std::size_t>(StringIndex::last)];
extern const io::Endpoint endpoints[static_cast<std::size_t>(EndpointIndex::last)];
} // namespace descriptor
} // namespace usb
