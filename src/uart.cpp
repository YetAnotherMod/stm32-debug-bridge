#include <cdc_payload.h>
#include <cstring>

#include <stm32f1xx.h>

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

    uint32_t brr = (SystemCoreClock / lineCoding.dwDTERate);

    brr = brr / 2 + brr % 2;

    // если частота меньше минимальной для UART, ставим минимальную
    if (brr > (USART_BRR_DIV_Fraction_Msk | USART_BRR_DIV_Mantissa_Msk)) {
        brr = (USART_BRR_DIV_Fraction_Msk | USART_BRR_DIV_Mantissa_Msk);
    }

    // дожидаемся окончания работы UART. Предполагаем, если 3 чтения подряд
    // регистры пусты, то UART закончил работу
    int counter = 0;
    while (counter < 3) {
        if (((USART2->CR1 & USART_CR1_UE_Msk) != 0) &&
            ((USART2->SR & (USART_SR_TXE_Msk | USART_SR_RXNE_Msk)) !=
             USART_SR_TXE)) {
            counter = 0;
        } else {
            counter++;
        }
    }

    // инициализация UART
    uint32_t cr1 = USART_CR1_TE | USART_CR1_RE;
    USART2->CR1 = 0;

    USART2->BRR = brr;

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

    uint32_t cr3 = USART_CR3_CTSE | USART_CR3_RTSE;

    USART2->CR1 = cr1;
    USART2->CR2 = cr2;
    USART2->CR3 = cr3;

    // корректировка dwDTERate в соответствии с точным значением

    lineCoding.dwDTERate = SystemCoreClock * 2 / brr;
    lineCoding.dwDTERate = lineCoding.dwDTERate / 2 + lineCoding.dwDTERate % 2;

    // включение uart

    USART2->CR1 = cr1 | USART_CR1_UE;

    setControlLineState(controlLineState);

    // конец функции
    pendingApply = false;
}

} // namespace cdcPayload
} // namespace usb
