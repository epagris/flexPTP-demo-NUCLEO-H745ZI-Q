#
# User is free to modify the file as much as necessary
#

# Core project settings
set(CM7_TARGET CM7)
# project(${CM7_TARGET})

# Core MCU flags, CPU, instruction set and FPU setup
set(cpu_PARAMS ${cpu_PARAMS}
    -mthumb

    # Other parameters
    # -mcpu, -mfloat, -mfloat-abi, ...
    -mcpu=cortex-m7
	-mfpu=fpv5-d16
	-mfloat-abi=hard
	
)

# Linker script
set(linker_script_SRC ${linker_script_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32h745zitx_CM7.ld
)

# Sources
set(sources_SRCS ${sources_SRCS} 
    Common/Boot/system_stm32h7xx_dualcore_boot_cm4_cm7.c    
    Common/Drivers/CMSIS/CMSIS_RTOS_V2/cmsis_os2.c

	Startup/startup_stm32h745zitx_CM7.s
)

# Include directories
set(include_c_DIRS ${include_c_DIRS}
	${CMAKE_CURRENT_LIST_DIR}/Inc
    ${CMAKE_CURRENT_LIST_DIR}/Modules
)

##########

# Create an executable object type
add_executable(${CM7_TARGET})

# Link directories setup
target_link_directories(${CM7_TARGET} PUBLIC ${link_DIRS})

# Add include paths
target_include_directories(${CM7_TARGET} PUBLIC
    ${include_c_DIRS}
)

# Add project symbols (macros)
set(comp_defs    
    CORE_CM7

    # Configuration specific
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>: >
)

target_compile_definitions(${CM7_TARGET} PRIVATE
    ${comp_defs}
)

# Add linked libraries
target_link_libraries(${CM7_TARGET} freertos_kernel_CM7)

# Compiler options
target_compile_options(${CM7_TARGET} PRIVATE
    ${cpu_PARAMS}
    -Wall
    -Wextra
    #-Wpedantic
    -Wno-unused-parameter

    $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp -MMD -MP>
    $<$<CONFIG:Debug>:-O0 -ggdb>
    $<$<CONFIG:Release>:-Og -g0>
)

# Linker options
target_link_options(${CM7_TARGET} PRIVATE
    -T${linker_script_SRC}
    ${cpu_PARAMS}
    -Wl,-Map=${CM7_TARGET}.map
    #-u _printf_float # STDIO float formatting support (remove if not used)
    --specs=nosys.specs
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -lsupc++
    -Wl,--end-group
    #-Wl,-z,max-page-size=8 # Allow good software remapping across address space (with proper GCC section making)
    -Wl,--print-memory-usage
)

##########

# Add Middlewares (FreeRTOS etc.)
include("FreeRTOS.cmake")
target_compile_definitions(${CM7_TARGET} PUBLIC CMSIS_device_header="${device_header}") # exported from Middlewares

# Add HAL library
set(DRIVER_TARGET ${CM7_TARGET})
add_subdirectory(Common/Drivers/STM32H7_HAL) 
list(APPEND include_c_DIRS ${HAL_INCLUDE_PATH})

# Add ICC
set(ICC_CORE "CM7")
set(ICC_TARGET ${CM7_TARGET})
add_subdirectory(Common/ICC)

# Add embfmt
set(EMBFMT_TARGET_TAG CM7)
set(EMBFMT_CPU_PARAMS ${cpu_PARAMS})
add_subdirectory(Common/Middlewares/embfmt)
target_link_libraries(${CM7_TARGET} embfmt_CM7)

set(MODULES_TARGET ${CM7_TARGET})
add_subdirectory(Modules)

# Add sources to executable
target_sources(${CM7_TARGET} PUBLIC ${sources_SRCS})

add_subdirectory(Src) 

##########

# Execute post-build to print size, generate hex and bin
add_custom_command(TARGET ${CM7_TARGET} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CM7_TARGET}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CM7_TARGET}> ${CM7_TARGET}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CM7_TARGET}> ${CM7_TARGET}.bin
)
