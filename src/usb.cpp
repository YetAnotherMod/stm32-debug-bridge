
#include <usb.h>

#include <fifo.h>
#include <gpio.h>
#include <global_resources.h>

#include <algorithm>

namespace usb {


static volatile io::bTableEntity *const bTable =
    reinterpret_cast<io::bTableEntity *>(static_cast<uintptr_t>(USB_PMAADDR));

static int read(descriptor::EndpointIndex epNum, void *buf, size_t bufSize) {
    const ptrdiff_t epNum_ = static_cast<ptrdiff_t>(epNum);
    const io::pBufferData *epBuf = reinterpret_cast<const io::pBufferData *>(
        static_cast<uintptr_t>(USB_PMAADDR + bTable[epNum_].rxOffset * 2));
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
    io::pBufferData *epBuf = reinterpret_cast<io::pBufferData *>(
        static_cast<uintptr_t>(USB_PMAADDR + bTable[epNum_].txOffset * 2));
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

}

void controlRxHandler() { ; }
void controlTxHandler() { ; }
void controlSetupHandler() { ; }

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
    usbPins.clockOn();
    usbPins.write(false, false);
    usbPins.configOutput<0>(gpio::OutputType::gen_pp,
                            gpio::OutputSpeed::_10mhz);
    usbPins.configOutput<1>(gpio::OutputType::gen_pp,
                            gpio::OutputSpeed::_10mhz);

    for (int i = 0; i < 0xFFFF; i++) {
        __NOP();
    }

    usbPins.configOutput<0>(gpio::OutputType::alt_pp,
                            gpio::OutputSpeed::_10mhz);
    usbPins.configOutput<1>(gpio::OutputType::alt_pp,
                            gpio::OutputSpeed::_10mhz);
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
    uint16_t offset = sizeof(io::bTableEntity) * 8 / 2;
    using descriptor::endpoints;
    for (int epNum = 0;
         epNum < static_cast<int>(descriptor::EndpointIndex::last); epNum++) {

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
