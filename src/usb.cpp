
#include <usb.h>

namespace usb {

static int read(uint8_t epNum, void *buf, size_t buf_size)
{
    
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

static volatile io::bTableEntity *const bTable =
    reinterpret_cast<io::bTableEntity *>(static_cast<uintptr_t>(USB_PMAADDR));

constexpr uint16_t convertEpType (descriptor::Endpoint::EpType t){
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

void init(void){
    /* Force USB re-enumeration */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH &= ~GPIO_CRH_CNF12;
    GPIOA->CRH |= GPIO_CRH_MODE12_1;
    for (int i=0; i<0xFFFF; i++) {
        __NOP();
    }
    GPIOA->CRH &= ~GPIO_CRH_MODE12;
    GPIOA->CRH |= GPIO_CRH_CNF12_0;
    /* Initialize USB */
    NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    if (SystemCoreClock != RCC_MAX_FREQUENCY) {
        RCC->CFGR |= RCC_CFGR_USBPRE;
    }
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;
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
        io::epRegs(epNum) = USB_EP_RX_VALID | USB_EP_TX_NAK | convertEpType(endpoints[epNum].type) | epNum;
    }
    USB->CNTR = USB_CNTR_CTRM | USB_CNTR_RESETM | USB_CNTR_SUSPM | USB_CNTR_WKUPM | USB_CNTR_SOFM;
    USB->DADDR = USB_DADDR_EF;
}
} // namespace usb
extern "C" void USB_LP_IRQHandler() {
    using usb::descriptor::endpoints;
    uint16_t istr = USB->ISTR;
    if (istr & USB_ISTR_CTR) {
        uint16_t epNum = (istr & USB_ISTR_EP_ID_Msk) >> USB_ISTR_EP_ID_Pos;
        auto epReg = usb::io::epRegs(epNum) &
                     (USB_EP_T_FIELD_Msk | USB_EP_KIND_Msk |
                      USB_EPADDR_FIELD_Msk | USB_EP_CTR_TX | USB_EP_CTR_RX);
        usb::io::epRegs(epNum) &=
            (USB_EP_T_FIELD_Msk | USB_EP_KIND_Msk | USB_EPADDR_FIELD_Msk);
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
        USB->CNTR |= USB_CNTR_FSUSP;
        USB->DADDR = USB_DADDR_EF;
    } else if (istr & USB_ISTR_WKUP) {
        USB->ISTR = (uint16_t)(~USB_ISTR_WKUP);
        USB->CNTR &= ~USB_CNTR_FSUSP;
    }
}
