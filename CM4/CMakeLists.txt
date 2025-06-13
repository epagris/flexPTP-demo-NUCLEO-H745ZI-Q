#
# User is free to modify the file as much as necessary
#

# Core project settings
set(CM4_TARGET CM4)
# project(${CM4_TARGET})

# Core MCU flags, CPU, instruction set and FPU setup
set(cpu_PARAMS ${cpu_PARAMS}
    -mthumb

    # Other parameters
    # -mcpu, -mfloat, -mfloat-abi, ...
    -mcpu=cortex-m4
	-mfpu=fpv4-sp-d16
	-mfloat-abi=hard
	
)

# Linker script
set(linker_script_SRC ${linker_script_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32h745zitx_CM4.ld
)

# Sources
set(sources_SRCS ${sources_SRCS}
    Common/Boot/system_stm32h7xx_dualcore_boot_cm4_cm7.c    
    Common/Drivers/CMSIS/CMSIS_RTOS_V2/cmsis_os2.c

	Src/main.c
	Src/syscall.c
	Src/sysmem.c
    Src/etherlib_options.c
	Startup/startup_stm32h745zitx_CM4.s

    Src/cmds.c
    Src/cmds.h
)

# Include directories
set(include_c_DIRS ${include_c_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Modules
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers
)

##########

# Create an executable object type
add_executable(${CM4_TARGET})

# Link directories setup
target_link_directories(${CM4_TARGET} PUBLIC ${link_DIRS})

# Add include paths
target_include_directories(${CM4_TARGET} PUBLIC
    ${include_c_DIRS}
)

# Select Ethernet stack
set(ETH_STACK "ETHERLIB") # select "LWIP" or "ETHERLIB"

# Add project symbols (macros)
set(comp_defs    
    CORE_CM4

    ETH_${ETH_STACK}
    ETH_STACK="${ETH_STACK}"
    PPS_SIMPLE

    # Configuration specific
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>: >
)

target_compile_definitions(${CM4_TARGET} PRIVATE ${comp_defs})

# Add linked libraries
target_link_libraries(${CM4_TARGET} freertos_kernel_CM4)

# Compiler options
target_compile_options(${CM4_TARGET} PRIVATE
    ${cpu_PARAMS}
    -Wall
    -Wextra
    #-Wpedantic
    -Wno-unused-parameter

    $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp -MMD -MP>
    $<$<CONFIG:Debug>:-O0 -g3 -ggdb>
    $<$<CONFIG:Release>:-Og -g0>
)

# Linker options
target_link_options(${CM4_TARGET} PRIVATE
    -T${linker_script_SRC}
    ${cpu_PARAMS}
    -Wl,-Map=${CM4_TARGET}.map
    -u _printf_float # STDIO float formatting support (remove if not used)
    --specs=nosys.specs
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -lsupc++
    -Wl,--end-group
    -Wl,-z,max-page-size=8 # Allow good software remapping across address space (with proper GCC section making)
    -Wl,--print-memory-usage
)

##########

# Add Middlewares (FreeRTOS etc.)
include("FreeRTOS.cmake")
target_compile_definitions(${CM4_TARGET} PUBLIC CMSIS_device_header="${device_header}") # exported from Middlewares

# Add HAL drivers
set(DRIVER_TARGET ${CM4_TARGET})
add_subdirectory(Common/Drivers/STM32H7_HAL)

# Add ICC
set(ICC_CORE "CM4")
set(ICC_TARGET ${CM4_TARGET})
add_subdirectory(Common/ICC)

# Add embfmt
set(EMBFMT_TARGET_TAG CM4)
set(EMBFMT_CPU_PARAMS ${cpu_PARAMS})
add_subdirectory(Common/Middlewares/embfmt)
target_link_libraries(${CM4_TARGET} embfmt_CM4)

# Add EtherLib
if (ETH_STACK STREQUAL "ETHERLIB")
    set(ETHERLIB_INCLUDES ${include_c_DIRS})
    set(ETHERLIB_CPU_PARAMS ${cpu_PARAMS})
    set(ETHERLIB_COMPILE_DEFS ${comp_defs})
    add_subdirectory(Modules/etherlib)
    target_link_libraries(${CM4_TARGET} etherlib)
elseif(ETH_STACK STREQUAL "LWIP")
    set(ETH_STACK_LIB lwipcore)
    set(LWIP_DIR ${CMAKE_CURRENT_LIST_DIR}/Modules/lwip)
    set(LWIP_PORT_DIR ${CMAKE_CURRENT_LIST_DIR}/Modules/lwip_port)
    #set(LWIP_CONTRIB_DIR ${LWIP_DIR}/contrib)
    set(LWIP_INCLUDE_DIRS
        ${LWIP_DIR}/src/include
        ${LWIP_DIR}/contrib
        ${LWIP_PORT_DIR}/arch
        ${LWIP_PORT_DIR}
        ${include_dirs}
    )
    message(${LWIP_PORT_DIR})
    set(LWIP_COMPILER_FLAGS ${cpu_PARAMS})
    include(${LWIP_DIR}/src/Filelists.cmake)
    #include(${LWIP_DIR}/contrib/Filelists.cmake)
    
    target_include_directories(${CM4_TARGET} PUBLIC ${LWIP_DIR}/src/include ${LWIP_PORT_DIR})
    target_include_directories(
        lwipcore
        PUBLIC 
        
        ${LWIP_DIR}/src/include
        ${LWIP_PORT_DIR} 
        ${CMAKE_CURRENT_LIST_DIR}/Inc 
        ${CMAKE_CURRENT_LIST_DIR}/Common/Drivers/CMSIS/CMSIS_RTOS_V2
        )
    target_sources(lwipcore PUBLIC ${LWIP_PORT_DIR}/OS/sys_arch.c)
    #target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${LWIP_PORT_DIR}/OS/sys_arch.c)

    target_link_libraries(${CM4_TARGET} lwipcore)
endif()

# Add non-shared drivers
add_subdirectory(Drivers)

# Add local modules
add_subdirectory(Src/standard_output)

set(BIO_TARGET ${CM4_TARGET})
add_subdirectory(Modules/blocking_io)

add_subdirectory(Src/cliutils)
add_subdirectory(Src/ethernet)

# Add flexPTP
if (ETH_STACK STREQUAL "ETHERLIB")
    set(FLEXPTP_HWPORT "H745_ETHERLIB")
elseif(ETH_STACK STREQUAL "LWIP")
    set(FLEXPTP_HWPORT "H745_LWIP")
endif()
set(FLEXPTP_NSD ${ETH_STACK})
set(FLEXPTP_SERVO "PID")
set(FLEXPTP_INCLUDES ${include_c_DIRS} ${HAL_INCLUDE_PATH} ${FREERTOS_CM4_INCLUDE_DIRS})
set(FLEXPTP_CPU_PARAMS ${cpu_PARAMS})
set(FLEXPTP_COMPILE_DEFS ${comp_defs})
add_subdirectory(Modules/flexptp)
target_link_libraries(${CM4_TARGET} flexptp)

# Add sources to executable
target_sources(${CM4_TARGET} PUBLIC ${sources_SRCS})

##########

# Execute post-build to print size, generate hex and bin
add_custom_command(TARGET ${CM4_TARGET} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CM4_TARGET}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CM4_TARGET}> ${CM4_TARGET}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CM4_TARGET}> ${CM4_TARGET}.bin
)
