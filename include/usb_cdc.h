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

namespace descriptor {

enum class Subtype : std::uint8_t {
    header = 0x00,
    call_management = 0x01,
    acm = 0x02,
    union_ = 0x06,
    country = 0x07,
};

using ::usb::descriptor::Base;

struct Header : Base {
    Subtype bDescriptorSubType;
    std::uint16_t bcdCDC;
} __attribute__((packed));

static_assert(sizeof(Header) == 5);
static_assert(alignof(Header) == 1);

struct Union : Base {
    Subtype bDescriptorSubType;
    std::uint8_t bMasterInterface0;
    std::uint8_t bSlaveInterface0;
} __attribute__((packed));

static_assert(sizeof(Union) == 5);
static_assert(alignof(Union) == 1);

struct CallMgmt : Base {
    Subtype bDescriptorSubType;
    std::uint8_t bmCapabilities;
    std::uint8_t bDataInterface;
} __attribute__((packed));

static_assert(sizeof(CallMgmt) == 5);
static_assert(alignof(CallMgmt) == 1);

struct CdcAcm : Base {
    Subtype bDescriptorSubType;
    std::uint8_t bmCapabilities;
} __attribute__((packed));

static_assert(sizeof(CdcAcm) == 4);
static_assert(alignof(CdcAcm) == 1);

} // namespace descriptor
} // namespace cdc
} // namespace usb