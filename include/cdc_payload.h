#pragma once

#include <cstdint>

namespace usb {
namespace cdcPayload {

namespace lineState {
constexpr std::uint16_t dtr = 0x01;
constexpr std::uint16_t rts = 0x02;
} // namespace lineState

namespace serialState {
constexpr std::uint16_t dcd = 0x01;
constexpr std::uint16_t dsr = 0x02;
constexpr std::uint16_t ri = 0x08;
constexpr std::uint16_t parityError = 0x20;
constexpr std::uint16_t overrun = 0x40;
} // namespace serialState

enum class CharFormat : std::uint8_t { stopBit1, stopBit1p5, stopBit2, last };

enum class ParityType : std::uint8_t { none, odd, even, mark, space, last };

enum class DataBits : std::uint8_t {
    bits8 = 8,
    bits9 = 9,
};

struct LineCoding {
    std::uint32_t dwDTERate;
    CharFormat bCharFormat;
    ParityType bParityType;
    DataBits bDataBits;
} __attribute__((packed));

static_assert(sizeof(LineCoding) == 7);
static_assert(alignof(LineCoding) == 1);

void setControlLineState(uint16_t v);

bool setLineCoding(const LineCoding *v);

void applyLineCoding();

bool isPendingApply();

} // namespace cdcPayload
} // namespace usb
