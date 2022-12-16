#pragma once

#include <stm32f1xx.h>

#include <tuple>

#ifndef NDEBUG
#include <stdexcept>
#endif

namespace gpio {

enum class Port {
#ifdef GPIOA
    a,
#endif
#ifdef GPIOB
    b,
#endif
#ifdef GPIOC
    c,
#endif
#ifdef GPIOD
    d,
#endif
#ifdef GPIOE
    e,
#endif
#ifdef GPIOF
    f,
#endif
#ifdef GPIOG
    g,
#endif
    end
};

enum class OutputSpeed { _10mhz = 0x1, _2mhz = 0x2, _50mhz = 0x3 };

enum class InputType { analog = 0x0, floating = 0x1, pull_up_down = 0x2 };

enum class OutputType {
    gen_pp = 0x0,
    gen_od = 0x1,
    alt_pp = 0x2,
    alt_od = 0x3
};

template <Port port, uint8_t... pins> class Bulk;

template <Port port, uint8_t pin> class Pin {
    static_assert(port < Port::end, "Port is not exist");
    static_assert(pin < 16, "pin must be less 16");

  private:
    static uint32_t getClockMsk() {
        uint32_t result;

        switch (port) {
#ifdef GPIOA
        case Port::a:
            result = RCC_APB2ENR_IOPAEN_Msk;
            break;
#endif
#ifdef GPIOB
        case Port::b:
            result = RCC_APB2ENR_IOPBEN_Msk;
            break;
#endif
#ifdef GPIOC
        case Port::c:
            result = RCC_APB2ENR_IOPCEN_Msk;
            break;
#endif
#ifdef GPIOD
        case Port::d:
            result = RCC_APB2ENR_IOPDEN_Msk;
            break;
#endif
#ifdef GPIOE
        case Port::e:
            result = RCC_APB2ENR_IOPEEN_Msk;
            break;
#endif
#ifdef GPIOF
        case Port::f:
            result = RCC_APB2ENR_IOPFEN_Msk;
            break;
#endif
#ifdef GPIOG
        case Port::g:
            result = RCC_APB2ENR_IOPGEN_Msk;
            break;
#endif
        }
        return result;
    }
#ifndef NDEBUG
    static bool bisy;
#endif
    uint32_t makeWriteWordHigh() const { return 1u << (pin + 0); }
    uint32_t makeWriteWordLow() const { return 1u << (pin + 16); }
    void writeRaw(uint32_t v) {
        getGpioPointer()->BSRR = v;
        __DMB();
    }

  public:
    GPIO_TypeDef *getGpioPointer() const {
        GPIO_TypeDef *p;
        switch (port) {
#ifdef GPIOA
        case Port::a:
            p = GPIOA;
            break;
#endif
#ifdef GPIOB
        case Port::b:
            p = GPIOB;
            break;
#endif
#ifdef GPIOC
        case Port::c:
            p = GPIOC;
            break;
#endif
#ifdef GPIOD
        case Port::d:
            p = GPIOD;
            break;
#endif
#ifdef GPIOE
        case Port::e:
            p = GPIOE;
            break;
#endif
#ifdef GPIOF
        case Port::f:
            p = GPIOF;
            break;
#endif
#ifdef GPIOG
        case Port::g:
            p = GPIOG;
            break;
#endif
        }
        return p;
    }
    uint32_t makeWriteWord(bool v) const {
        if (v) {
            return makeWriteWordHigh();
        }
        return makeWriteWordLow();
    }
    void configInput(InputType type) {
        GPIO_TypeDef *p = getGpioPointer();
        if (pin < 8) {
            auto t = p->CRL;
            t &= ~(0xfu << ((pin - 0) * 4));
            t |= static_cast<uint32_t>(type) << ((pin - 0) * 4 + 2);
            p->CRL = t;
        } else {
            auto t = p->CRH;
            t &= ~(0xfu << ((pin - 8) * 4));
            t |= static_cast<uint32_t>(type) << ((pin - 8) * 4 + 2);
            p->CRH = t;
        }
        __DMB();
    }
    void configOutput(OutputType type, OutputSpeed speed) {
        GPIO_TypeDef *p = getGpioPointer();
        if (pin < 8) {
            auto t = p->CRL;
            t &= ~(0xfu << ((pin - 0) * 4));
            t |= (static_cast<uint32_t>(speed) |
                  (static_cast<uint32_t>(type) << 2))
                 << ((pin - 0) * 4);
            p->CRL = t;
        } else {
            auto t = p->CRH;
            t &= ~(0xfu << ((pin - 8) * 4));
            t |= (static_cast<uint32_t>(speed) |
                  (static_cast<uint32_t>(type) << 2))
                 << ((pin - 8) * 4);
            p->CRH = t;
        }
        __DMB();
    }
    bool read() const {
        return static_cast<bool>(getGpioPointer()->IDR >> pin & 1);
    }
    void writeHigh() { writeRaw(makeWriteWordHigh()); }
    void writeLow() { writeRaw(makeWriteWordLow()); }
    void write(bool v) {
        if (v) {
            writeHigh();
        } else {
            writeLow();
        }
    }
    void operator=(bool v) { write(v); }
    static void clockOn() { RCC->APB2ENR = RCC->APB2ENR | getClockMsk(); }
    static void clockOff() { RCC->APB2ENR = RCC->APB2ENR & ~getClockMsk(); }
    void toggle() { write(!read()); }
    Pin() {
#ifndef NDEBUG
        auto primask = __get_PRIMASK();
        __disable_irq();
        if (bisy) {
            throw std::bad_exception();
        }
        bisy = true;
        __set_PRIMASK(primask);
#endif
    }
    Pin(const Pin &) = delete;
    Pin(Pin &&) {}
    ~Pin() {
        configInput(InputType::floating);
#ifndef NDEBUG
        auto primask = __get_PRIMASK();
        __disable_irq();
        bisy = false;
        __set_PRIMASK(primask);
#endif
    }
    template <Port p, uint8_t... pins> friend class Bulk;
};

#ifndef NDEBUG
template <Port port, uint8_t pin> bool Pin<port, pin>::bisy = false;
#endif

template <Port port, uint8_t... pins> class Bulk {
  private:
    static constexpr uint8_t ind[sizeof...(pins)] = {pins...};
    std::tuple<Pin<port, pins>...> pins_;
    template <size_t I = 0>
    uint32_t makeWriteWord(__unused const bool v[sizeof...(pins)]) const
        requires(I == sizeof...(pins)) {
        return 0;
    }
    template <size_t I = 0>
    uint32_t makeWriteWord(const bool v[sizeof...(pins)]) const {
        return makeWriteWord<I + 1>(v) | std::get<I>(pins_).makeWriteWord(v[I]);
    }

  public:
    GPIO_TypeDef *getGpioPointer() {
        return std::get<0>(pins_).getGpioPointer();
    }
    template <typename... in>
    uint32_t makeWriteWord(in... args) requires(sizeof...(in) ==
                                                sizeof...(pins)) {
        bool v[] = {args...};
        return makeWriteWord(v);
    }
    void clockOn() { std::get<0>(pins_).clockOn(); }
    template <uint8_t I> void configInput(InputType type) {
        std::get<I>(pins_).configInput(type);
    }
    template <uint8_t I> void configOutput(OutputType type, OutputSpeed speed) {
        std::get<I>(pins_).configOutput(type, speed);
    }

    template <typename... in> void write(in... args) {
        static_assert(sizeof...(in) == sizeof...(pins), "incorrect bulk width");
        bool v[] = {args...};
        std::get<0>(pins_).writeRaw(makeWriteWord(v));
    }
    template <uint8_t I> void write(bool v) { std::get<I>(pins_).write(v); }
    template <uint8_t I> uint32_t makeWriteWord(bool v) {
        return std::get<I>(pins_).makeWriteWord(v);
    }
    template <uint8_t I> bool read() { return std::get<I>(pins_).read(); }
};

} // namespace gpio
