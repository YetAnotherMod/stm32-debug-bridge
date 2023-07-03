#include <cdc_payload.h>
#include <cstring>
#include <global_resources.h>

#include PLATFORM_HEADER

namespace usb {
namespace cdcPayload {

static LineCoding lineCoding{115200, CharFormat::stopBit1, ParityType::none,
                             DataBits::bits8};

static uint16_t controlLineState = 0;

static bool pendingApply = true;

bool isPendingApply() { return pendingApply; }

void setControlLineState(uint16_t v) {
    // TODO: заглушка
    controlLineState = v;
}

bool setLineCoding(const LineCoding *v) {
    // TODO: ревью

    // считаем максимальный делитель частоты. считаем, что если делитель должен
    // быть на 0,5% больше, чем размер регистра, то частота корректная
    constexpr uint32_t maxBrr2 =
        (USART_BRR_DIV_Fraction_Msk | USART_BRR_DIV_Mantissa_Msk) * 995 / 500;
    constexpr uint32_t maxBrr = maxBrr2 / 2 + maxBrr2 % 2;

    if ((v->bDataBits != DataBits::bits8)) {
        return false;
    }
    if (v->bCharFormat >= CharFormat::last) {
        return false;
    }
    if (v->bParityType >= ParityType::last) {
        return false;
    }

    if (v->dwDTERate < SystemCoreClock / maxBrr) {
        return false;
    }

    memcpy(&lineCoding, v, sizeof(LineCoding));
    pendingApply = true;
    return true;
}

void applyLineCoding() {
    // TODO: заглушка

    // считаем делитель с округлением

    uint32_t brr = (SystemCoreClock / lineCoding.dwDTERate) * (2/config::uartClkDiv);

    brr = brr / 2 + brr % 2;

    // если частота меньше минимальной для UART, ставим минимальную
    if (brr > (USART_BRR_DIV_Fraction_Msk | USART_BRR_DIV_Mantissa_Msk)) {
        brr = (USART_BRR_DIV_Fraction_Msk | USART_BRR_DIV_Mantissa_Msk);
    }

    // дожидаемся окончания работы UART. Предполагаем, если 3 чтения подряд
    // регистры пусты, то UART закончил работу
    while(config::uartDmaTx->CNDTR != 0);

    // инициализация UART
    uint32_t cr1 = USART_CR1_TE | USART_CR1_RE;
    config::uart->CR1 = 0;

    config::uart->BRR = brr;

    if (lineCoding.bParityType == ParityType::odd ||
        lineCoding.bParityType == ParityType::even) {
        cr1 |= USART_CR1_PCE;
        if (lineCoding.bParityType == ParityType::odd) {
            cr1 |= USART_CR1_PS;
        }
    }

    if (lineCoding.bParityType == ParityType::space) {
        cr1 |= USART_CR1_M;
    }

    uint32_t cr2 = [](CharFormat x) {
        switch (x) {
        case CharFormat::stopBit1:
            return static_cast<uint32_t>(0);
        case CharFormat::stopBit1p5:
            return static_cast<uint32_t>(USART_CR2_STOP_0 | USART_CR2_STOP_1);
        case CharFormat::stopBit2:
        default:
            return static_cast<uint32_t>(USART_CR2_STOP_1);
        }
    }(lineCoding.bCharFormat);

    uint32_t cr3 = (config::uartCts?USART_CR3_CTSE:0) | (config::uartRts?USART_CR3_RTSE:0) | USART_CR3_DMAT | USART_CR3_DMAR;

    config::uart->CR1 = cr1;
    config::uart->CR2 = cr2;
    config::uart->CR3 = cr3;

    // корректировка dwDTERate в соответствии с точным значением

    lineCoding.dwDTERate = SystemCoreClock * config::uartClkDiv / brr;
    lineCoding.dwDTERate = lineCoding.dwDTERate / config::uartClkDiv + lineCoding.dwDTERate % 2;

    // включение uart

    config::uart->CR1 = cr1 | USART_CR1_UE;

    setControlLineState(controlLineState);

    // конец функции
    pendingApply = false;
}

} // namespace cdcPayload
} // namespace usb
