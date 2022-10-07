#pragma once

#include <usb_std.h>

namespace usb {
namespace cdc {

namespace Subclass {
constexpr std::uint8_t none = 0x00;
constexpr std::uint8_t acm = 0x02;
} // namespace Subclass

namespace Protocol {
constexpr std::uint8_t none = 0x0;
constexpr std::uint8_t v25ter = 0x01;
constexpr std::uint8_t protocolDefault = none;
} // namespace Protocol

namespace acmCapability {
constexpr std::uint8_t commFeature = 0x01;
constexpr std::uint8_t lineCoding = 0x02;
constexpr std::uint8_t sendBreak = 0x04;
constexpr std::uint8_t networkConnection = 0x08;
constexpr std::uint8_t default_ = lineCoding;
}; // namespace acmCapability


enum class Request : std::uint8_t {
    send_encapsulated_command = 0x00,
    get_encapsulated_response = 0x01,
    set_comm_feature = 0x02,
    get_comm_feature = 0x03,
    clear_comm_feature = 0x04,
    set_line_coding = 0x20,
    get_line_coding = 0x21,
    set_control_line_state = 0x22,
    send_break = 0x23,
};

struct Notification {
    std::uint8_t bmRequestType;
    std::uint8_t bNotificationType;
    std::uint16_t wValue;
    std::uint16_t wIndex;
    std::uint16_t wLength;
    std::uint16_t state;
} __attribute__((packed));

static_assert(sizeof(Notification) == 10);
static_assert(alignof(Notification) == 1);

enum class pin {
    rx,
    tx,
    rts,
    cts,
    dsr,
    dtr,
    dcd,
    ri,
    txa,
    unknow,
    last = unknow
};

constexpr std::uint8_t numPorts = 3;
constexpr std::size_t bufSize = 0x400;
constexpr std::size_t linesPollingInterval = 20;
enum class serialInd { uart = 0, shell = 1, jtag = 2 };
namespace descriptor {

enum class Subtype : std::uint8_t {
    header = 0x00,
    call_management = 0x01,
    acm = 0x02,
    union_ = 0x06,
    country = 0x07,
};

using ::usb::descriptor::Base;
using ::usb::descriptor::Type;

struct Header : Base {
    Subtype bDescriptorSubType;
    std::uint16_t bcdCDC;
    constexpr Header()
        : Base(sizeof(*this), Type::cs_interface),
          bDescriptorSubType(Subtype::header), bcdCDC(bcdVersion(1, 1, 0)) {}
} __attribute__((packed));

static_assert(sizeof(Header) == 5);
static_assert(alignof(Header) == 1);

struct Union : Base {
    Subtype bDescriptorSubType;
    std::uint8_t bMasterInterface0;
    std::uint8_t bSlaveInterface0;
    constexpr Union(std::uint8_t bMasterInterface0,
                    std::uint8_t bSlaveInterface0)
        : Base(sizeof(*this), Type::cs_interface),
          bDescriptorSubType(Subtype::union_),
          bMasterInterface0(bMasterInterface0),
          bSlaveInterface0(bSlaveInterface0) {}
} __attribute__((packed));

static_assert(sizeof(Union) == 5);
static_assert(alignof(Union) == 1);

struct CallMgmt : Base {
    Subtype bDescriptorSubType;
    std::uint8_t bmCapabilities;
    std::uint8_t bDataInterface;
    constexpr CallMgmt(std::uint8_t bmCapabilities, std::uint8_t bDataInterface)
        : Base(sizeof(*this), Type::cs_interface),
          bDescriptorSubType(Subtype::call_management),
          bmCapabilities(bmCapabilities), bDataInterface(bDataInterface) {}
} __attribute__((packed));

static_assert(sizeof(CallMgmt) == 5);
static_assert(alignof(CallMgmt) == 1);

struct Acm : Base {
    Subtype bDescriptorSubType;
    std::uint8_t bmCapabilities;
    constexpr Acm(std::uint8_t bmCapabilities)
        : Base(sizeof(*this), Type::cs_interface),
          bDescriptorSubType(Subtype::acm), bmCapabilities(bmCapabilities) {}
} __attribute__((packed));

static_assert(sizeof(Acm) == 4);
static_assert(alignof(Acm) == 1);

} // namespace descriptor
} // namespace cdc
} // namespace usb
