set(FIRMWARE_ORIGIN 0x08002000)
set(STARTUP src/startup-stm32f103.cpp)

if(NOT STM32CUBE)
set(STM32CUBE ${PROJECT_SOURCE_DIR}/STM32CubeF1)
endif(NOT STM32CUBE)

include_directories(include
    ${STM32CUBE}/Drivers/CMSIS/Core/Include
    ${STM32CUBE}/Drivers/CMSIS/Core_A/Include
    ${STM32CUBE}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${PLATFORM_DIR}
)

set (STM32_LDSCRIPT ${PROJECT_SOURCE_DIR}/src/STM32F103XB_FLASH.ld)

add_compile_definitions(
    PLATFORM_HEADER=<stm32f1xx.h>
    VENDOR_ID=0x1209
    PRODUCT_ID=0xfffe
    VENDOR_STRING=u"RC Module"_sd
    PRODUCT_STRING=u"DJM \(Debugger Jtag Module\)"_sd
)
set (MCPU cortex-m3)
