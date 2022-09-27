
#include <cstdint>

extern "C" void __attribute__((weak)) Default_Handler() {
    while (1)
        ;
}

using CallbackType = void (*)();

extern "C" void Reset_Handler();
extern "C" void NMI_Handler() __attribute__((weak, alias("Default_Handler")));
extern "C" void HardFault_Handler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void MemManage_Handler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void BusFault_Handler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void UsageFault_Handler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void SVC_Handler() __attribute__((weak, alias("Default_Handler")));
extern "C" void DebugMon_Handler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void PendSV_Handler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void SysTick_Handler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void WWDG_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void PVD_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void TAMPER_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void RTC_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void FLASH_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void RCC_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void EXTI0_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void EXTI1_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void EXTI2_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void EXTI3_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void EXTI4_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void DMA1_Channel1_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void DMA1_Channel2_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void DMA1_Channel3_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void DMA1_Channel4_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void DMA1_Channel5_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void DMA1_Channel6_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void DMA1_Channel7_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void ADC1_2_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void USB_HP_CAN1_TX_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void USB_LP_CAN1_RX0_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void CAN1_RX1_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void CAN1_SCE_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void EXTI9_5_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM1_BRK_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM1_UP_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM1_TRG_COM_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM1_CC_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM2_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM3_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM4_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void I2C1_EV_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void I2C1_ER_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void I2C2_EV_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void I2C2_ER_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void SPI1_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void SPI2_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void USART1_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void USART2_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void USART3_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void EXTI15_10_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void RTC_Alarm_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void USBWakeUp_IRQHandler()
    __attribute__((weak, alias("Default_Handler")));
extern "C" void BootRAM() __attribute__((weak, alias("Default_Handler")));
extern "C" void ret() __attribute__((weak, alias("Default_Handler")));

struct IsrVectors {
    std::uint32_t *estack_;

    CallbackType Reset_Handler_;
    CallbackType NMI_Handler_;
    CallbackType HardFault_Handler_;
    CallbackType MemManage_Handler_;
    CallbackType BusFault_Handler_;
    CallbackType UsageFault_Handler_;

    std::uint32_t reserved1[4];

    CallbackType SVC_Handler_;
    CallbackType DebugMon_Handler_;

    std::uint32_t reserved2;

    CallbackType PendSV_Handler_;
    CallbackType SysTick_Handler_;
    CallbackType WWDG_IRQHandler_;
    CallbackType PVD_IRQHandler_;
    CallbackType TAMPER_IRQHandler_;
    CallbackType RTC_IRQHandler_;
    CallbackType FLASH_IRQHandler_;
    CallbackType RCC_IRQHandler_;
    CallbackType EXTI0_IRQHandler_;
    CallbackType EXTI1_IRQHandler_;
    CallbackType EXTI2_IRQHandler_;
    CallbackType EXTI3_IRQHandler_;
    CallbackType EXTI4_IRQHandler_;
    CallbackType DMA1_Channel1_IRQHandler_;
    CallbackType DMA1_Channel2_IRQHandler_;
    CallbackType DMA1_Channel3_IRQHandler_;
    CallbackType DMA1_Channel4_IRQHandler_;
    CallbackType DMA1_Channel5_IRQHandler_;
    CallbackType DMA1_Channel6_IRQHandler_;
    CallbackType DMA1_Channel7_IRQHandler_;
    CallbackType ADC1_2_IRQHandler_;
    CallbackType USB_HP_CAN1_TX_IRQHandler_;
    CallbackType USB_LP_CAN1_RX0_IRQHandler_;
    CallbackType CAN1_RX1_IRQHandler_;
    CallbackType CAN1_SCE_IRQHandler_;
    CallbackType EXTI9_5_IRQHandler_;
    CallbackType TIM1_BRK_IRQHandler_;
    CallbackType TIM1_UP_IRQHandler_;
    CallbackType TIM1_TRG_COM_IRQHandler_;
    CallbackType TIM1_CC_IRQHandler_;
    CallbackType TIM2_IRQHandler_;
    CallbackType TIM3_IRQHandler_;
    CallbackType TIM4_IRQHandler_;
    CallbackType I2C1_EV_IRQHandler_;
    CallbackType I2C1_ER_IRQHandler_;
    CallbackType I2C2_EV_IRQHandler_;
    CallbackType I2C2_ER_IRQHandler_;
    CallbackType SPI1_IRQHandler_;
    CallbackType SPI2_IRQHandler_;
    CallbackType USART1_IRQHandler_;
    CallbackType USART2_IRQHandler_;
    CallbackType USART3_IRQHandler_;
    CallbackType EXTI15_10_IRQHandler_;
    CallbackType RTC_Alarm_IRQHandler_;
    CallbackType USBWakeUp_IRQHandler_;

    std::uint32_t reserved3[7];

    CallbackType BootRAM_;
};

extern std::uint32_t _estack[];

__attribute__((section(".isr_vector"))) IsrVectors g_pfnVectors = {
    .estack_ = _estack,
    .Reset_Handler_ = Reset_Handler,
    .NMI_Handler_ = NMI_Handler,
    .HardFault_Handler_ = HardFault_Handler,
    .MemManage_Handler_ = MemManage_Handler,
    .BusFault_Handler_ = BusFault_Handler,
    .UsageFault_Handler_ = UsageFault_Handler,
    .reserved1 = {0, 0, 0, 0},
    .SVC_Handler_ = SVC_Handler,
    .DebugMon_Handler_ = DebugMon_Handler,
    .reserved2 = 0,
    .PendSV_Handler_ = PendSV_Handler,
    .SysTick_Handler_ = SysTick_Handler,
    .WWDG_IRQHandler_ = WWDG_IRQHandler,
    .PVD_IRQHandler_ = PVD_IRQHandler,
    .TAMPER_IRQHandler_ = TAMPER_IRQHandler,
    .RTC_IRQHandler_ = RTC_IRQHandler,
    .FLASH_IRQHandler_ = FLASH_IRQHandler,
    .RCC_IRQHandler_ = RCC_IRQHandler,
    .EXTI0_IRQHandler_ = EXTI0_IRQHandler,
    .EXTI1_IRQHandler_ = EXTI1_IRQHandler,
    .EXTI2_IRQHandler_ = EXTI2_IRQHandler,
    .EXTI3_IRQHandler_ = EXTI3_IRQHandler,
    .EXTI4_IRQHandler_ = EXTI4_IRQHandler,
    .DMA1_Channel1_IRQHandler_ = DMA1_Channel1_IRQHandler,
    .DMA1_Channel2_IRQHandler_ = DMA1_Channel2_IRQHandler,
    .DMA1_Channel3_IRQHandler_ = DMA1_Channel3_IRQHandler,
    .DMA1_Channel4_IRQHandler_ = DMA1_Channel4_IRQHandler,
    .DMA1_Channel5_IRQHandler_ = DMA1_Channel5_IRQHandler,
    .DMA1_Channel6_IRQHandler_ = DMA1_Channel6_IRQHandler,
    .DMA1_Channel7_IRQHandler_ = DMA1_Channel7_IRQHandler,
    .ADC1_2_IRQHandler_ = ADC1_2_IRQHandler,
    .USB_HP_CAN1_TX_IRQHandler_ = USB_HP_CAN1_TX_IRQHandler,
    .USB_LP_CAN1_RX0_IRQHandler_ = USB_LP_CAN1_RX0_IRQHandler,
    .CAN1_RX1_IRQHandler_ = CAN1_RX1_IRQHandler,
    .CAN1_SCE_IRQHandler_ = CAN1_SCE_IRQHandler,
    .EXTI9_5_IRQHandler_ = EXTI9_5_IRQHandler,
    .TIM1_BRK_IRQHandler_ = TIM1_BRK_IRQHandler,
    .TIM1_UP_IRQHandler_ = TIM1_UP_IRQHandler,
    .TIM1_TRG_COM_IRQHandler_ = TIM1_TRG_COM_IRQHandler,
    .TIM1_CC_IRQHandler_ = TIM1_CC_IRQHandler,
    .TIM2_IRQHandler_ = TIM2_IRQHandler,
    .TIM3_IRQHandler_ = TIM3_IRQHandler,
    .TIM4_IRQHandler_ = TIM4_IRQHandler,
    .I2C1_EV_IRQHandler_ = I2C1_EV_IRQHandler,
    .I2C1_ER_IRQHandler_ = I2C1_ER_IRQHandler,
    .I2C2_EV_IRQHandler_ = I2C2_EV_IRQHandler,
    .I2C2_ER_IRQHandler_ = I2C2_ER_IRQHandler,
    .SPI1_IRQHandler_ = SPI1_IRQHandler,
    .SPI2_IRQHandler_ = SPI2_IRQHandler,
    .USART1_IRQHandler_ = USART1_IRQHandler,
    .USART2_IRQHandler_ = USART2_IRQHandler,
    .USART3_IRQHandler_ = USART3_IRQHandler,
    .EXTI15_10_IRQHandler_ = EXTI15_10_IRQHandler,
    .RTC_Alarm_IRQHandler_ = RTC_Alarm_IRQHandler,
    .USBWakeUp_IRQHandler_ = USBWakeUp_IRQHandler,
    .reserved3 = {0, 0, 0, 0, 0, 0, 0},
    .BootRAM_ = BootRAM};

extern "C" int main();
extern "C" int SystemInit();
extern "C" void __libc_init_array();
extern "C" void __libc_fini_array();

extern "C" void Reset_Handler() {

    {
        extern std::uint32_t _sdata[];
        extern std::uint32_t _edata[];
        const extern std::uint32_t _sidata[];
        uint32_t *dst = _sdata;
        const uint32_t *src = _sidata;
        while (dst < _edata) {
            *dst++ = *src++;
        }
    }
    {
        extern std::uint32_t _sbss[];
        extern std::uint32_t _ebss[];
        uint32_t *p = _sbss;
        while (p < _ebss) {
            *p++ = 0;
        }
    }
    SystemInit();
    __libc_init_array();
    main();
    __libc_fini_array();
    ret();
}
