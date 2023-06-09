set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

if(${CMAKE_VERSION} VERSION_LESS 3.6)
    if(NOT CMAKE_C_COMPILER)
        include(CMakeForceCompiler)
        cmake_force_c_compiler(${CROSS_COMPILE}gcc${CMAKE_EXECUTABLE_SUFFIX} GNU)
        set(CMAKE_C_COMPILER_AR ${CROSS_COMPILE}ar${CMAKE_EXECUTABLE_SUFFIX} CACHE FILEPATH "ar from toolchain")
        set(CMAKE_C_COMPILER_RANLIB ${CROSS_COMPILE}ranlib${CMAKE_EXECUTABLE_SUFFIX} CACHE FILEPATH "ranlib from toolchain")
        cmake_force_cxx_compiler(${CROSS_COMPILE}g++${CMAKE_EXECUTABLE_SUFFIX} GNU)
    endif(NOT CMAKE_C_COMPILER)
else(${CMAKE_VERSION} VERSION_LESS 3.6)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc${CMAKE_EXECUTABLE_SUFFIX} CACHE FILEPATH "asm compiler")
    set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc${CMAKE_EXECUTABLE_SUFFIX} CACHE FILEPATH "c compiler")
    set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++${CMAKE_EXECUTABLE_SUFFIX} CACHE FILEPATH "cxx compiler")
endif(${CMAKE_VERSION} VERSION_LESS 3.6)
