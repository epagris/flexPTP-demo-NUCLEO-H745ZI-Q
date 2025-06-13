set(FREERTOS_TAG CM4)

add_library(freertos_config_CM4 INTERFACE)

target_include_directories(
    freertos_config_CM4
    SYSTEM INTERFACE
    Inc
)

target_compile_definitions(freertos_config_CM4
    INTERFACE
    projCOVERAGE_TEST=0
)

target_compile_options(
  freertos_config_CM4
  INTERFACE
  ${cpu_PARAMS}
)

set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)
# Select the native compile PORT
set( FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)
# Select the cross-compile PORT
if (CMAKE_CROSSCOMPILING)
  set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)
endif()

set(freertos_kernel_SOURCE_DIR_CM4 ${CMAKE_CURRENT_SOURCE_DIR}/Common/Middlewares/FreeRTOS)
add_subdirectory(${freertos_kernel_SOURCE_DIR_CM4})

set(FREERTOS_CM4_INCLUDE_DIRS
  ${freertos_kernel_SOURCE_DIR_CM4}/include  
  ${freertos_kernel_SOURCE_DIR_CM4}/portable/GCC/ARM_CM4F
  PARENT_DIRECTORY
)

target_include_directories(
  ${CM4_TARGET} 
  PUBLIC
  FREERTOS_CM4_INCLUDE_DIRS
)

set(device_header "${freertos_kernel_SOURCE_DIR_CM4}/portable/GCC/ARM_CM4F/portmacro.h")