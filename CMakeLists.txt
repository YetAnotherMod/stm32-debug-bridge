cmake_minimum_required(VERSION 3.13)

if(NOT CMAKE_TOOLCHAIN_FILE)
    if (NOT CROSS_COMPILE)
        set(CROSS_COMPILE arm-none-eabi-)
    endif(NOT CROSS_COMPILE)
    set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)
endif(NOT CMAKE_TOOLCHAIN_FILE)


project (debug-bridge ASM C CXX)

if(NOT STM32CUBE)
set(STM32CUBE ${PROJECT_SOURCE_DIR}/STM32CubeF1)
endif(NOT STM32CUBE)

include_directories(include
    ${STM32CUBE}/Drivers/CMSIS/Core/Include
    ${STM32CUBE}/Drivers/CMSIS/Core_A/Include
    ${STM32CUBE}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
)

set (STM32_LDSCRIPT ${STM32CUBE}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/linker/STM32F103XB_FLASH.ld)

add_compile_definitions(STM32F103xB HSE_VALUE=8000000U)
add_compile_options(-mthumb -mcpu=cortex-m3 -fno-pic)
add_compile_options(-Wall -Wextra)
add_compile_options(-O2 -gdwarf-2)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++2a)

add_link_options(-mthumb -mcpu=cortex-m3 -T${STM32_LDSCRIPT} --specs=nosys.specs --specs=nano.specs)
if(FIRMWARE_ORIGIN)
add_link_options(-Wl,-section-start=.isr_vector=${FIRMWARE_ORIGIN})
endif(FIRMWARE_ORIGIN)

add_executable(firmware.elf src/main.cpp
    src/usb_descriptors.cpp
    src/usb.cpp
    ${STM32CUBE}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c
    src/startup.cpp
)

add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Obinary firmware.elf firmware.bin
    COMMAND ${CMAKE_OBJCOPY} -Oihex firmware.elf firmware.hex
)

include(ExternalProject)

ExternalProject_Add(tests
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/tests
    CMAKE_ARGS -DREPORT_PATH=${CMAKE_BINARY_DIR}
    TEST_BEFORE_INSTALL true TEST_COMMAND ctest --output-on-failure
    INSTALL_COMMAND cmake --build . --target coverage
    BUILD_ALWAYS true
)
