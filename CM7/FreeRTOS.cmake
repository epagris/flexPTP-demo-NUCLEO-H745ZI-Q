set(FREERTOS_TAG CM7)

add_library(freertos_config_${FREERTOS_TAG} INTERFACE)

target_include_directories(
    freertos_config_${FREERTOS_TAG}
    SYSTEM INTERFACE
    Inc
)

target_compile_definitions(freertos_config_${FREERTOS_TAG}
    INTERFACE
    projCOVERAGE_TEST=0
)

target_compile_options(
  freertos_config_${FREERTOS_TAG}
  INTERFACE
  ${cpu_PARAMS}
)

set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)
# Select the native compile PORT
set( FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)
# Select the cross-compile PORT
if (CMAKE_CROSSCOMPILING)
  set(FREERTOS_PORT "GCC_ARM_CM7" CACHE STRING "" FORCE)
endif()

set(freertos_kernel_SOURCE_DIR_CM7 ${CMAKE_CURRENT_SOURCE_DIR}/Common/Middlewares/FreeRTOS)
add_subdirectory(${freertos_kernel_SOURCE_DIR_CM7})

target_include_directories(
  ${CM7_TARGET} 
  PUBLIC
  ${freertos_kernel_SOURCE_DIR_CM7}/include  
  ${freertos_kernel_SOURCE_DIR_CM7}/portable/GCC/ARM_CM7/r0p1
)

set(device_header "${freertos_kernel_SOURCE_DIR_CM7}/portable/GCC/ARM_CM7/r0p1/portmacro.h")