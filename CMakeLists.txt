cmake_minimum_required(VERSION 3.22)

list(APPEND CMAKE_MODULE_PATH "{{sr:cmake_path}}")
message("Build CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})
include("cmake/gcc-arm-none-eabi.cmake")

project(flexptp-multicore-demo)
enable_language(C CXX ASM) 
message("Build type: " ${CMAKE_BUILD_TYPE})

# set(ALLOW_DUPLICATE_CUSTOM_TARGETS TRUE)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON) 
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS ON)  

set(MCU_TYPE STM32H745xx)

#########

# Add global compile definitions
add_compile_definitions(
    ${MCU_TYPE}
    USE_HAL_DRIVER
    CMSIS_OS2
)

#########

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/Common)
set(MIDDLEWARES_DIR ${COMMON_DIR}/Middlewares)

# Add common driver includes
set(CMSIS_DIR Common/Drivers/CMSIS)
include_directories(
    ${CMSIS_DIR}/Device/ST/Include
    ${CMSIS_DIR}/Include
    ${CMSIS_DIR}/CMSIS_RTOS_V2

    ${MIDDLEWARES_DIR}
    ${COMMON_DIR}
)

add_link_options(-L${CMAKE_SOURCE_DIR}/Common)

# Add core targets
add_subdirectory(CM7)
add_subdirectory(CM4)
