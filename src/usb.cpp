
#include <usb.h>

#include <fifo.h>
#include <global_resources.h>
#include <gpio.h>

#include <algorithm>

namespace usb {

struct DeviceState {
    enum class State { reset = 0x00, addressSet = 0x01, configured = 0x02 };
    State state;
    uint8_t address;
    uint8_t configuration;
};
static DeviceState deviceState;

struct ControlState {
    enum class State { idle, rx, tx, txZlp, txLast, statusIn, statusOut };
    State state;
    Setup setup;
    uint8_t *currentPayload;
    uint16_t payloadLeft;
};

static ControlState controlState;

static inline void endpointSetStall(descriptor::EndpointIndex epNum,
                                    descriptor::Endpoint::Direction dir,
                                    bool stall) {
    auto &epReg = io::epRegs(static_cast<ptrdiff_t>(epNum));
    auto epReg_ = epReg;
    if ((epReg_ & USB_EP_T_FIELD) != USB_EP_ISOCHRONOUS) {
        if (dir == descriptor::Endpoint::Direction::in) {
            if ((epReg_ & USB_EPTX_STAT_Msk) != USB_EP_TX_DIS) {
                if (stall) {
                    epReg = ((epReg_ ^ USB_EP_TX_STALL) &
                             (USB_EPREG_MASK | USB_EPTX_STAT_Msk)) |
                            (USB_EP_CTR_RX | USB_EP_CTR_TX);
                } else {
                    epReg = ((epReg_ ^ USB_EP_TX_NAK) &
                             (USB_EPREG_MASK | USB_EPTX_STAT_Msk |
                              USB_EP_DTOG_TX)) |
                            (USB_EP_CTR_RX | USB_EP_CTR_TX);
                }
            }
        } else {
            if ((epReg_ & USB_EPRX_STAT_Msk) != USB_EP_RX_DIS) {
                if (stall) {
                    epReg = ((epReg_ ^ USB_EP_RX_STALL) &
                             (USB_EPREG_MASK | USB_EPTX_STAT_Msk)) |
                            (USB_EP_CTR_RX | USB_EP_CTR_TX);
                } else {
                    // TODO: разобраться какого чёрта тут TX
                    epReg = ((epReg_ ^ USB_EP_TX_VALID) &
                             (USB_EPREG_MASK | USB_EPTX_STAT_Msk |
                              USB_EP_DTOG_RX)) |
                            (USB_EP_CTR_RX | USB_EP_CTR_TX);
                }
            }
        }
    }
}
static inline bool endpointGetStall(descriptor::EndpointIndex epNum,
                                    descriptor::Endpoint::Direction dir) {
    auto &epReg = io::epRegs(static_cast<ptrdiff_t>(epNum));
    if (dir == descriptor::Endpoint::Direction::in) {
        return (epReg & USB_EPTX_STAT_Msk) == USB_EP_TX_STALL;
    }
    return (epReg & USB_EPRX_STAT_Msk) == USB_EP_RX_STALL;
}

static volatile io::bTableEntity *const bTable =
    reinterpret_cast<io::bTableEntity *>(static_cast<uintptr_t>(USB_PMAADDR));

static inline const io::pBufferData *rxBuf(std::uint8_t epNum) {
    return reinterpret_cast<io::pBufferData *>(
        static_cast<uintptr_t>(USB_PMAADDR + bTable[epNum].rxOffset * 2));
}
static inline io::pBufferData *txBuf(std::uint8_t epNum) {
    return reinterpret_cast<io::pBufferData *>(
        static_cast<uintptr_t>(USB_PMAADDR + bTable[epNum].txOffset * 2));
}

static int read(descriptor::EndpointIndex epNum, void *buf, size_t bufSize) {
    const ptrdiff_t epNum_ = static_cast<ptrdiff_t>(epNum);
    const io::pBufferData *epBuf = rxBuf(epNum_);
    const uint16_t rxCount = bTable[epNum_].rxCount;
    const uint16_t epBytesCount = rxCount & USB_COUNT0_RX_COUNT0_RX;
    uint16_t *bufP = reinterpret_cast<uint16_t *>(buf);
    if (bufSize < epBytesCount) {
        return -1;
    }
    bTable[epNum_].rxCount =
        rxCount & static_cast<uint16_t>(~USB_COUNT0_RX_COUNT0_RX);
    for (unsigned i = epBytesCount / 2; i > 0; --i) {
        *bufP++ = (epBuf++)->data;
    }
    if (epBytesCount % 2) {
        *reinterpret_cast<uint8_t *>(bufP) = static_cast<uint8_t>(epBuf->data);
    }
    io::epRegs(epNum_) = ((io::epRegs(epNum_) ^ USB_EP_RX_VALID) &
                          (USB_EPREG_MASK | USB_EPRX_STAT)) |
                         (USB_EP_CTR_RX | USB_EP_CTR_TX);
    return epBytesCount;
}

static size_t write(descriptor::EndpointIndex epNum, const void *buf,
                    size_t count) {
    const ptrdiff_t epNum_ = static_cast<ptrdiff_t>(epNum);
    io::pBufferData *epBuf = txBuf(epNum_);
    const uint16_t *bufP = reinterpret_cast<const uint16_t *>(buf);
    const size_t txSize = descriptor::endpoints[epNum_].txSize;
    if (count > txSize) {
        count = txSize;
    }
    for (unsigned i = (count + 1) / 2; i > 0; --i) {
        (epBuf++)->data = *bufP++;
    }
    bTable[epNum_].txCount = count;
    io::epRegs(epNum_) = ((io::epRegs(epNum_) ^ USB_EP_TX_VALID) &
                          (USB_EPREG_MASK | USB_EPTX_STAT)) |
                         (USB_EP_CTR_RX | USB_EP_CTR_TX);
    return count;
}

size_t readToFifo(descriptor::EndpointIndex epNum,
                  fifo::Fifo<uint8_t, global::cdcFifoLenRx> data) {
    const ptrdiff_t epNum_ = static_cast<ptrdiff_t>(epNum);
    const io::pBufferData *epBuf = rxBuf(epNum_);
    const uint16_t rxCount = bTable[epNum_].rxCount;
    const uint16_t epBytesCount = rxCount & USB_COUNT0_RX_COUNT0_RX;

    bTable[epNum_].rxCount =
        rxCount & static_cast<uint16_t>(~USB_COUNT0_RX_COUNT0_RX);
    for (size_t wordsLeft = epBytesCount / 2; wordsLeft > 0; --wordsLeft) {
        data.write(reinterpret_cast<const uint8_t *>(epBuf), 2);
        epBuf++;
    }
    if (epBytesCount % 2) {
        data.push(epBuf->data);
    }
    return epBytesCount;
}

size_t writeFromFifo(descriptor::EndpointIndex epNum,
                     fifo::Fifo<uint8_t, global::cdcFifoLenTx> data) {
    const ptrdiff_t epNum_ = static_cast<ptrdiff_t>(epNum);
    size_t count =
        std::min<size_t>(descriptor::endpoints[epNum_].txSize, data.size());
    io::pBufferData *epBuf = txBuf(epNum_);
    for (size_t wordsLeft = count / 2; wordsLeft > 0; --wordsLeft) {
        data.read(reinterpret_cast<uint8_t *>(epBuf), 2);
        epBuf++;
    }
    if (count % 2) {
        epBuf->data = data.pop();
    }
    bTable[epNum_].txCount = count;
    io::epRegs(epNum_) = ((io::epRegs(epNum_) ^ USB_EP_TX_VALID) &
                          (USB_EPREG_MASK | USB_EPTX_STAT)) |
                         (USB_EP_CTR_RX | USB_EP_CTR_TX);
    return count;
}

namespace control {

enum class status { ack = 0x00, nak = 0x01, fail = 0x02 };

status processRequest() {
    if ((controlState.setup.type == Setup::type_class) &&
        (controlState.setup.recipient == Setup::recipient_interface)) {
        if ((controlState.setup.wIndex ==
             static_cast<uint16_t>(descriptor::InterfaceIndex::jtag)) ||
            (controlState.setup.wIndex ==
             static_cast<uint16_t>(descriptor::InterfaceIndex::shell))) {
            return status::ack;
        } else if (controlState.setup.wIndex ==
                   static_cast<uint16_t>(descriptor::InterfaceIndex::uart)) {
            switch (static_cast<cdc::Request>(controlState.setup.bRequest)) {
            case cdc::Request::set_control_line_state:
                cdcPayload::setControlLineState(controlState.setup.wValue);
                return status::ack;
            case cdc::Request::set_line_coding: {
                const cdcPayload::LineCoding *lineCoding =
                    static_cast<cdcPayload::LineCoding *>(
                        static_cast<void *>(controlState.setup.payload));
                if (controlState.setup.wLength ==
                    sizeof(cdcPayload::LineCoding)) {
                    bool dryRun = false;
                    if (!global::uartTx.isEmpty()) {
                        dryRun = true;
                    }
                    cdcPayload::setLineCoding(lineCoding, dryRun);
                }
            } break;
            default:
                break;
            }
        }
    }
}

static inline void epStall() {
    endpointSetStall(descriptor::EndpointIndex::control,
                     descriptor::Endpoint::Direction::out, true);
    endpointSetStall(descriptor::EndpointIndex::control,
                     descriptor::Endpoint::Direction::in, true);
    controlState.state = ControlState::State::idle;
}

void rxDispatch() {
    size_t setupSize;
    size_t payloadBytesReceived = 0;
    switch (controlState.state) {
    case ControlState::State::idle:
        setupSize =
            read(descriptor::EndpointIndex::control,
                 static_cast<void *>(&controlState.setup), sizeof(Setup));
        if (setupSize != sizeof(Setup)) {
            epStall();
            return;
        } else {
            controlState.currentPayload = controlState.setup.payload;
            controlState.payloadLeft = controlState.setup.wLength;
            if ((controlState.setup.direction ==
                 Setup::direction_host_to_device) &&
                (controlState.setup.wLength != 0)) {
                if (controlState.setup.wLength > sizeof(Setup::payload)) {
                    epStall();
                } else {
                    controlState.state = ControlState::State::rx;
                }
                return;
            }
        }
        break;
    case ControlState::State::rx:
        payloadBytesReceived =
            read(descriptor::EndpointIndex::control,
                 controlState.currentPayload, controlState.setup.wLength);
        if (controlState.setup.wLength != payloadBytesReceived) {
            controlState.currentPayload += payloadBytesReceived;
            controlState.payloadLeft -= payloadBytesReceived;
            return;
        }
        break;
    case ControlState::State::statusOut:
        read(descriptor::EndpointIndex::control, nullptr, 0);
        controlState.state = ControlState::State::idle;
        break;
    default:
        epStall();
        return;
    }
    controlState.currentPayload = controlState.setup.payload;
    controlState.payloadLeft = sizeof(Setup::payload);
    switch (0) {}
}

} // namespace control
void controlRxHandler() { control::rxDispatch(); }
void controlTxHandler() { ; }
void controlSetupHandler() {
    controlState.state = ControlState::State::idle;
    control::rxDispatch();
}

void uartRxHandler() { ; }
void uartTxHandler() { ; }
void uartInterruptHandler() { ; }

void shellRxHandler() { ; }
void shellTxHandler() { ; }
void shellInterruptHandler() { ; }

void jtagRxHandler() { ; }
void jtagTxHandler() { ; }
void jtagInterruptHandler() { ; }

constexpr uint16_t convertEpType(descriptor::Endpoint::EpType t) {
    uint16_t epType = 0;
    switch (t) {
    case descriptor::Endpoint::EpType::control:
        epType = USB_EP_CONTROL;
        break;
    case descriptor::Endpoint::EpType::isochronous:
        epType = USB_EP_ISOCHRONOUS;
        break;
    case descriptor::Endpoint::EpType::bulk:
        epType = USB_EP_BULK;
        break;
    case descriptor::Endpoint::EpType::interrupt:
        epType = USB_EP_INTERRUPT;
        break;
    }
    return epType;
}

void init(void) {
    /* Force USB re-enumeration */
    global::usbPins.configOutput<0>(gpio::OutputType::gen_pp,
                                    gpio::OutputSpeed::_10mhz);
    global::usbPins.configOutput<1>(gpio::OutputType::gen_pp,
                                    gpio::OutputSpeed::_10mhz);

    for (int i = 0; i < 0xFFFF; i++) {
        __NOP();
    }

    global::usbPins.configOutput<0>(gpio::OutputType::alt_pp,
                                    gpio::OutputSpeed::_50mhz);
    global::usbPins.configOutput<1>(gpio::OutputType::alt_pp,
                                    gpio::OutputSpeed::_50mhz);
    NVIC_EnableIRQ(USB_LP_IRQn);
    NVIC_DisableIRQ(USB_LP_IRQn);

    RCC->APB1ENR = RCC->APB1ENR | RCC_APB1ENR_USBEN;
    USB->CNTR = USB_CNTR_FRES;
    USB->BTABLE = 0;
    USB->DADDR = 0;
    USB->ISTR = 0;
    USB->CNTR = USB_CNTR_RESETM;
}

void reset(void) {
    deviceState.state = DeviceState::State::reset;
    deviceState.address = 0;
    deviceState.configuration = 0;
    uint16_t offset = sizeof(io::bTableEntity) * 8 / 2;
    using descriptor::endpoints;
    for (std::ptrdiff_t epNum = 0;
         epNum < static_cast<std::ptrdiff_t>(descriptor::EndpointIndex::last);
         epNum++) {

        bTable[epNum].txOffset = offset;
        bTable[epNum].txCount = 0;
        offset += endpoints[epNum].txSize;

        bTable[epNum].rxOffset = offset;
        if (endpoints[epNum].rxSize > io::smallBlockSizeLimit) {
            bTable[epNum].rxCount =
                ((endpoints[epNum].rxSize / io::largeBlockSize - 1)
                 << USB_COUNT0_RX_NUM_BLOCK_Pos) |
                USB_COUNT0_RX_BLSIZE;
        } else {
            bTable[epNum].rxCount =
                (endpoints[epNum].rxSize / io::smallBlockSize)
                << USB_COUNT0_RX_NUM_BLOCK_Pos;
        }
        offset += endpoints[epNum].rxSize;
        io::epRegs(epNum) = USB_EP_RX_VALID | USB_EP_TX_NAK |
                            convertEpType(endpoints[epNum].type) | epNum;
    }
    USB->CNTR = USB_CNTR_CTRM | USB_CNTR_RESETM | USB_CNTR_SUSPM |
                USB_CNTR_WKUPM | USB_CNTR_SOFM;
    USB->DADDR = USB_DADDR_EF;
}
} // namespace usb
extern "C" void USB_LP_IRQHandler() {
    using usb::descriptor::endpoints;
    uint16_t istr = USB->ISTR;
    if (istr & USB_ISTR_CTR) {
        uint16_t epNum = (istr & USB_ISTR_EP_ID_Msk) >> USB_ISTR_EP_ID_Pos;
        auto epReg = usb::io::epRegs(epNum);
        usb::io::epRegs(epNum) = epReg & (USB_EP_T_FIELD_Msk | USB_EP_KIND_Msk |
                                          USB_EPADDR_FIELD_Msk);
        epReg &= (USB_EP_T_FIELD_Msk | USB_EP_KIND_Msk | USB_EPADDR_FIELD_Msk |
                  USB_EP_CTR_TX | USB_EP_CTR_RX);
        if (epReg & USB_EP_CTR_TX) {
            if (endpoints[epNum].txHandler) {
                endpoints[epNum].txHandler();
            }
        } else if (epReg & USB_EP_SETUP) {
            if (endpoints[epNum].setupHandler) {
                endpoints[epNum].setupHandler();
            }
        } else {
            if (endpoints[epNum].rxHandler) {
                endpoints[epNum].rxHandler();
            }
        }
    } else if (istr & USB_ISTR_RESET) {
        USB->ISTR = (uint16_t)(~USB_ISTR_RESET);
        usb::reset();
    } else if (istr & USB_ISTR_SUSP) {
        USB->ISTR = (uint16_t)(~USB_ISTR_SUSP);
        USB->CNTR = USB->CNTR | USB_CNTR_FSUSP;
        USB->DADDR = USB_DADDR_EF;
    } else if (istr & USB_ISTR_WKUP) {
        USB->ISTR = (uint16_t)(~USB_ISTR_WKUP);
        USB->CNTR = USB->CNTR & ~USB_CNTR_FSUSP;
    }
}
