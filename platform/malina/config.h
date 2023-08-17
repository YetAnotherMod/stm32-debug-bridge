#pragma once

#include <gpio.h>
#include <static_map.h>

namespace config{

struct PortPins{
    gpio::Pin<gpio::Port::b, 9> pwrOn;
    gpio::Pin<gpio::Port::b, 0> hostMode;
    gpio::Pin<gpio::Port::b, 1> edclLock;
    gpio::Pin<gpio::Port::a, 15> fanEn;
    gpio::Pin<gpio::Port::a, 6> fanPwm;
    gpio::Pin<gpio::Port::a, 8> nRst;
    gpio::Pin<gpio::Port::b, 10> jtagTrst;
    gpio::Pin<gpio::Port::b, 11> jtagHalt;
};

extern PortPins portPins;

static inline void ledOn(void){}

static inline void ledOff(void){}

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
    RCC->APB2ENR = RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR = RCC_APB1ENR_USBEN;
    RCC->AHBENR = RCC_AHBENR_FLITFEN | RCC_AHBENR_SRAMEN | RCC_AHBENR_DMA1EN;
    SystemCoreClockUpdate();
}

static inline void PortsInit(void) {
    using namespace gpio;
    Afio()->MAPR = Afio::MAPR::swjCfgSwdOnly;
    portPins.pwrOn.clockOn();
    portPins.pwrOn.write(false);
    portPins.pwrOn.configOutput(OutputType::gen_pp, OutputSpeed::_2mhz);
    portPins.hostMode.clockOn();
    portPins.hostMode.write(false);
    portPins.hostMode.configOutput(OutputType::gen_pp, OutputSpeed::_2mhz);
    portPins.edclLock.clockOn();
    portPins.edclLock.write(false);
    portPins.edclLock.configOutput(OutputType::gen_pp, OutputSpeed::_2mhz);
    portPins.fanPwm.clockOn();
    portPins.fanPwm.write(true);
    portPins.fanPwm.configOutput(OutputType::gen_pp, OutputSpeed::_2mhz);
    portPins.fanEn.clockOn();
    portPins.fanEn.write(true);
    portPins.fanEn.configOutput(OutputType::gen_pp, OutputSpeed::_2mhz);
    portPins.nRst.clockOn();
    portPins.nRst.write(false);
    portPins.nRst.configOutput(OutputType::gen_pp, OutputSpeed::_2mhz);
    portPins.jtagTrst.clockOn();
    portPins.jtagTrst.write(true);
    portPins.jtagTrst.configOutput(OutputType::gen_pp, OutputSpeed::_2mhz);
    portPins.jtagHalt.clockOn();
    portPins.jtagHalt.write(true);
    portPins.jtagHalt.configOutput(OutputType::gen_pp, OutputSpeed::_2mhz);
}

static inline void Panic(void){}

class CommandExecutor{
public:
    enum class CommandType{
        invalid,
        list,
        fan,
        power,
        host,
        elock,
        reset
    };
    class hasher{
        public:
        constexpr size_t operator () (std::string_view x) const{
           size_t result = 0;
           for ( uint8_t c:x){
               result = ((result << 13) | (result >> (sizeof(std::size_t)*8-13))) ^ c;
           }
           return result;
        }
    };
    void push (char c){
        global::shellRx.pushSafe(c);
    }
    template<size_t L>
    void execute(size_t argc, const std::array<std::string_view, L> &argv){
        using std::string_view;
        using namespace std::literals;
        static constexpr string_view switchNo = "incorrect parameter. must be 0 or 1"sv;
        static constexpr string_view error = "unknown command : "sv;
        static constexpr string_view errorParam = "invalid param: "sv;
        static constexpr string_view setTo = " set to: "sv;
        static constexpr string_view list = "LIST FAN POWER HOST ELOCK RESET"sv;
        static constexpr staticMap::StaticMap<string_view, CommandType, 6, 4, hasher> commands(
            {
                {"LIST"sv,CommandType::list},
                {"FAN"sv,CommandType::fan},
                {"POWER"sv,CommandType::power},
                {"HOST"sv,CommandType::host},
                {"ELOCK"sv,CommandType::elock},
                {"RESET"sv,CommandType::reset}
            }
        );
        CommandType c;
        {
            auto x = commands.find(argv[0]);
            if ( x == nullptr ){
                c = CommandType::invalid;
            }else{
                c = x->second;
            }
        }
        switch (c){
        case CommandType::invalid:
            for (uint8_t i:error)
                push(i);
            for (uint8_t i:argv[0])
                push(i);
            break;
        case CommandType::list:
            {
                for (uint8_t i:list)
                    push(i);
            }
            break;
        case CommandType::fan:
            {
                bool x;
                if (argc==2 && argv[1] == "0"){
                    x = false;
                }else if (argc==2 && argv[1] == "1"){
                    x = true;
                }else{
                    for (uint8_t i:errorParam)
                        push(i);
                    break;
                }
                for (uint8_t i:argv[0])
                    push(i);
                for (uint8_t i:setTo)
                    push(i);
                for (uint8_t i:argv[1])
                    push(i);
                portPins.fanPwm.write(x);
                portPins.fanEn.write(x);
            }
            break;
        case CommandType::power:
            {
                bool x;
                if (argc==2 && argv[1] == "0"){
                    x = false;
                }else if (argc==2 && argv[1] == "1"){
                    x = true;
                }else{
                    for (uint8_t i:switchNo)
                        push(i);
                    break;
                }
                for (uint8_t i:argv[0])
                    push(i);
                for (uint8_t i:setTo)
                    push(i);
                for (uint8_t i:argv[1])
                    push(i);
                portPins.pwrOn.write(x);
                if ( !x )
                    portPins.nRst.write(false);
            }
            break;
        case CommandType::host:
            {
                bool x;
                if (argc==2 && argv[1] == "0"){
                    x = false;
                }else if (argc==2 && argv[1] == "1"){
                    x = true;
                }else{
                    for (uint8_t i:switchNo)
                        push(i);
                    break;
                }
                for (uint8_t i:argv[0])
                    push(i);
                for (uint8_t i:setTo)
                    push(i);
                for (uint8_t i:argv[1])
                    push(i);
                portPins.hostMode.write(x);
            }
            break;
        case CommandType::elock:
            {
                bool x;
                if (argc==2 && argv[1] == "0"){
                    x = false;
                }else if (argc==2 && argv[1] == "1"){
                    x = true;
                }else{
                    for (uint8_t i:switchNo)
                        push(i);
                    break;
                }
                for (uint8_t i:argv[0])
                    push(i);
                for (uint8_t i:setTo)
                    push(i);
                for (uint8_t i:argv[1])
                    push(i);
                portPins.edclLock.write(x);
            }
            break;
        case CommandType::reset:
            {
                bool x;
                if (argc==2 && argv[1] == "0"){
                    x = false;
                }else if (argc==2 && argv[1] == "1"){
                    x = true;
                }else{
                    for (uint8_t i:switchNo)
                        push(i);
                    break;
                }
                for (uint8_t i:argv[0])
                    push(i);
                for (uint8_t i:setTo)
                    push(i);
                for (uint8_t i:argv[1])
                    push(i);
                portPins.nRst.write(x);
            }
            break;
        }
        push('\r');
        push('\n');
    }
private:
};

} // namespace global
