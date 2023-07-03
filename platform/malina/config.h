#pragma once

#include <gpio.h>
#include <static_map.h>

namespace config{

GLOBAL gpio::Bulk<gpio::Port::a, 11, 12> usbPins;
GLOBAL gpio::Pin<gpio::Port::c, 13> led;
// tdi,tms,tck
GLOBAL gpio::Bulk<gpio::Port::b, 15, 12, 13> jtagOut;
GLOBAL gpio::Pin<gpio::Port::b, 14> jtagIn;
// RX, TX, CTS, RTS
using UartType = gpio::Bulk<gpio::Port::a, 10, 9, 11, 12>;
GLOBAL_INIT(UartType, uartPins,true,true,false,false);

constexpr bool uartCts = false;
constexpr bool uartRts = false;
constexpr uint32_t uartClkDiv = 1;

USART_TypeDef * const uart = USART1;
DMA_Channel_TypeDef * const uartDmaTx = DMA1_Channel4;
DMA_Channel_TypeDef * const uartDmaRx = DMA1_Channel5;

GLOBAL gpio::Pin<gpio::Port::b, 9> pwrOn;
GLOBAL gpio::Pin<gpio::Port::b, 0> hostMode;
GLOBAL gpio::Pin<gpio::Port::b, 1> edclLock;
GLOBAL gpio::Pin<gpio::Port::a, 6> fanPwm;

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
    RCC->APB2ENR = RCC_APB2ENR_USART1EN;
    RCC->APB1ENR = RCC_APB1ENR_USBEN;
    RCC->AHBENR = RCC_AHBENR_FLITFEN | RCC_AHBENR_SRAMEN | RCC_AHBENR_DMA1EN;
    SystemCoreClockUpdate();
}

static inline void PortsInit(void) {
    using namespace gpio;
    pwrOn.clockOn();
    pwrOn.write(false);
    pwrOn.configOutput(OutputType::gen_pp, OutputSpeed::_50mhz);
    hostMode.clockOn();
    hostMode.write(false);
    hostMode.configOutput(OutputType::gen_pp, OutputSpeed::_50mhz);
    edclLock.clockOn();
    edclLock.write(false);
    edclLock.configOutput(OutputType::gen_pp, OutputSpeed::_50mhz);
    fanPwm.clockOn();
    fanPwm.write(true);
    fanPwm.configOutput(OutputType::gen_pp, OutputSpeed::_50mhz);
}
static inline void Panic(void){
    config::led.writeLow();
}

class CommandExecutor{
public:
    enum class CommandType{
        invalid,
        list,
        fan,
        power,
        host,
        elock
    };
    template <typename fifoOut>
        requires requires(fifoOut &&o, uint8_t x){
            o.pushSafe(x);
        }
    void execute(std::string_view command, std::string_view param, fifoOut& output){
        using std::string_view;
        using namespace std::literals;
        static const string_view switchNo = "incorrect parameter. must be 0 or 1"sv;
        static const string_view error = "unknown command : "sv;
        static const string_view errorParam = "invalid param: "sv;
        static const string_view setTo = "set to: "sv;
        static const string_view list = "LIST FAN POWER HOST ELOCK"sv;
        static const staticMap::StaticMap<string_view, CommandType, 5> commands(
            {
                {"LIST"sv,CommandType::list},
                {"FAN"sv,CommandType::fan},
                {"POWER"sv,CommandType::power},
                {"HOST"sv,CommandType::host},
                {"ELOCK"sv,CommandType::elock}
            }
        );
        CommandType c;
        {
            auto x = commands.find(command);
            if ( x == nullptr ){
                c = CommandType::invalid;
            }else{
                c = x->second;
            }
        }
        switch (c){
        case CommandType::invalid:
            for (uint8_t i:error)
                output.pushSafe(i);
            for (uint8_t i:command)
                output.pushSafe(i);
            break;
        case CommandType::list:
            {
                for (uint8_t i:list)
                    output.pushSafe(i);
            }
            break;
        case CommandType::fan:
            {
                bool x;
                if (param == "0"){
                    x = false;
                }else if (param == "1"){
                    x = true;
                }else{
                    for (uint8_t i:errorParam)
                        output.pushSafe(i);
                    break;
                }
                for (uint8_t i:setTo)
                    output.pushSafe(i);
                for (uint8_t i:param)
                    output.pushSafe(i);
                fanPwm.write(x);
            }
            break;
        case CommandType::power:
            {
                bool x;
                if (param == "0"){
                    x = false;
                }else if (param == "1"){
                    x = true;
                }else{
                    for (uint8_t i:switchNo)
                        output.pushSafe(i);
                    break;
                }
                for (uint8_t i:setTo)
                    output.pushSafe(i);
                for (uint8_t i:param)
                    output.pushSafe(i);
                pwrOn.write(x);
            }
            break;
        case CommandType::host:
            {
                bool x;
                if (param == "0"){
                    x = false;
                }else if (param == "1"){
                    x = true;
                }else{
                    for (uint8_t i:switchNo)
                        output.pushSafe(i);
                    break;
                }
                for (uint8_t i:setTo)
                    output.pushSafe(i);
                for (uint8_t i:param)
                    output.pushSafe(i);
                hostMode.write(x);
            }
            break;
        case CommandType::elock:
            {
                bool x;
                if (param == "0"){
                    x = false;
                }else if (param == "1"){
                    x = true;
                }else{
                    for (uint8_t i:switchNo)
                        output.pushSafe(i);
                    break;
                }
                for (uint8_t i:setTo)
                    output.pushSafe(i);
                for (uint8_t i:param)
                    output.pushSafe(i);
                edclLock.write(x);
            }
            break;
        }
        output.pushSafe('\r');
        output.pushSafe('\n');
    }
private:
};

} // namespace global
