#pragma once

#include <gpio.h>
#include <static_map.h>

namespace config{

GLOBAL gpio::Bulk<gpio::Port::a, 11, 12> usbPins;
GLOBAL gpio::Pin<gpio::Port::a, 6> led;
// tdi,tms,tck
GLOBAL gpio::Bulk<gpio::Port::b, 6, 9, 12> jtagOut;
GLOBAL gpio::Pin<gpio::Port::a, 5> jtagIn;
// RX, TX, CTS, RTS
GLOBAL gpio::Bulk<gpio::Port::a, 3, 2, 0, 1> uartPins;
constexpr bool uartCts = true;
constexpr bool uartRts = true;
constexpr uint32_t uartClkDiv = 2;

USART_TypeDef * const uart = USART2;
DMA_Channel_TypeDef * const uartDmaTx = DMA1_Channel7;
DMA_Channel_TypeDef * const uartDmaRx = DMA1_Channel6;


static inline void ClockInit(void) {
    uint32_t reservedBitsCr =
        RCC->CR & ~(RCC_CR_PLLRDY_Msk | RCC_CR_PLLON_Msk | RCC_CR_CSSON_Msk |
                    RCC_CR_HSEBYP_Msk | RCC_CR_HSERDY_Msk | RCC_CR_HSEON_Msk |
                    RCC_CR_HSITRIM_Msk | RCC_CR_HSIRDY_Msk | RCC_CR_HSION_Msk);
    uint32_t reservedBitsCfgr =
        RCC->CFGR &
        ~(RCC_CFGR_MCO_Msk | RCC_CFGR_USBPRE_Msk | RCC_CFGR_PLLMULL_Msk |
          RCC_CFGR_PLLXTPRE_Msk | RCC_CFGR_PLLSRC_Msk | RCC_CFGR_ADCPRE_Msk |
          RCC_CFGR_PPRE2_Msk | RCC_CFGR_PPRE1_Msk | RCC_CFGR_HPRE_Msk |
          RCC_CFGR_SWS_Msk | RCC_CFGR_SW_Msk);

    RCC->CR = RCC->CR | RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY_Msk) == 0)
        ;

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_Msk) | RCC_CFGR_SW_HSI;
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_HSI)
        ;

    RCC->CR = reservedBitsCr | RCC_CR_HSION | RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_PLLRDY_Msk) != 0)
        ;

    RCC->CFGR = reservedBitsCfgr | RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLSRC |
                RCC_CFGR_ADCPRE_DIV6 | RCC_CFGR_PPRE2_DIV1 |
                RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_SW_HSI;

    FLASH->ACR =
        (FLASH->ACR & ~(FLASH_ACR_HLFCYA_Msk | FLASH_ACR_LATENCY_Msk)) |
        (FLASH_ACR_PRFTBE_Msk | FLASH_ACR_LATENCY_2);

    while ((RCC->CR & RCC_CR_HSERDY_Msk) == 0)
        ;

    RCC->CR = reservedBitsCr | RCC_CR_HSION | RCC_CR_HSEON | RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY_Msk) == 0)
        ;

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_Msk) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL)
        ;
    RCC->APB2ENR = 0;
    RCC->APB1ENR = RCC_APB1ENR_USBEN | RCC_APB1ENR_USART2EN;
    RCC->AHBENR = RCC_AHBENR_FLITFEN | RCC_AHBENR_SRAMEN | RCC_AHBENR_DMA1EN;
    SystemCoreClockUpdate();
}

static inline void PortsInit(void) {}
static inline void Panic(void){
    config::led.writeLow();
}

class CommandExecutor{
public:
    template <typename fifoOut>
        requires requires(fifoOut &&o, uint8_t x){
            o.pushSafe(x);
        }
    void execute(std::string_view command, std::string_view param, fifoOut& output){
        for (uint8_t i:command)
            output.pushSafe(i);
        output.pushSafe(' ');
        for (uint8_t i:param)
            output.pushSafe(i);
        output.pushSafe('\r');
        output.pushSafe('\n');
    }
private:
};

} // namespace global
