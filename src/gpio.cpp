#include <gpio.h>


namespace gpio{
    
__attribute__((section(".gpioa"))) GPIO_TypeDef gpioa[1];
__attribute__((section(".gpiob"))) GPIO_TypeDef gpiob[1];
__attribute__((section(".gpioc"))) GPIO_TypeDef gpioc[1];
__attribute__((section(".gpiod"))) GPIO_TypeDef gpiod[1];
__attribute__((section(".gpioe"))) GPIO_TypeDef gpioe[1];
__attribute__((section(".gpiof"))) GPIO_TypeDef gpiof[1];
__attribute__((section(".gpiog"))) GPIO_TypeDef gpiog[1];

}
