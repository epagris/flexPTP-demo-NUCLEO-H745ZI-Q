/**
 *  CORTEX-M4 CORE
*/

#include <stdint.h>
#include <memory.h>

#include <stm32h7xx_hal.h>

#include <cmsis_os2.h>

#include "FreeRTOSConfig.h"

#include "ICC/icc.h"

#include "etherlib/dynmem.h"
#include "etherlib/etherlib.h"
#include "etherlib/prefab/conn_blocks/icmp_connblock.h"

#include "flexptp/task_ptp.h"
#include "flexptp/ptp_core.h"
#include "flexptp/profiles.h"
#include "flexptp/ptp_profile_presets.h"
#include "flexptp/settings_interface.h"
#include "flexptp/logging.h"

#include "cliutils/cli.h"

#include "cmds.h"

#include "ethernet/ethernet.h"

#define FLEXPTP_INITIAL_PROFILE ("gPTP")

void print_welcome_message() {
    MSG(ANSI_COLOR_BGREEN "Hi!" ANSI_COLOR_BYELLOW " This is a flexPTP demo for the STMicroelectronics NUCLEO-H745ZI-Q (STM32H745) board.\n\n"
                          "The application is built on FreeRTOS, flexPTP is currenty compiled against %s and uses the supplied example %s Network Stack Driver. "
                          "In this demo, the underlying Ethernet stack can be either lwip or EtherLib, the 'ETH_STACK' CMake variable (in the main CMakeLists.txt file) determines which one will be used. "
                          "The STM32H7xx PTP hardware module driver is also picked from the bundled ones. This flexPTP instance features a full CLI control interface, the help can be listed by typing '?' once the flexPTP has loaded. "
                          "The initial PTP preset that loads upon flexPTP initialization is the 'gPTP' (802.1AS) profile. It's a nowadays common profile, but we encourage "
                          "you to also try out the 'default' (plain IEEE 1588) profile and fiddle around with other options as well. The application will try to acquire an IP-address with DHCP. "
                          "Once the IP-address is secured, you might start the flexPTP module by typing 'flexptp'. 'Have a great time! :)'\n\n" ANSI_COLOR_RESET,
        ETH_STACK, ETH_STACK);

    MSG(ANSI_COLOR_BRED "By default, the MCU clock is sourced by the onboard (STLink) board controller on this devboard. According to our observations, this clock signal is loaded "
                        "with heavy noise rendering the clock synchronization unable to settle precisely. We highly recommend to solder a 8 or 25 MHz oscillator onto "
                        "the designated X3 pads to achieve the best results!\n\n" ANSI_COLOR_RESET);

    // MSG("Freq: %u\n", SystemCoreClock);
}

void task_startup(void *arg) {
    // open ICC pipe
    icc_open_pipe();

    osDelay(2000);

    MSG("Booting up the M4 core!\n");
    print_welcome_message();

    // initialize CLI
    cli_init();

    // initialize Ethernet stack
    init_ethernet();

    // initialize additional commands
    cmd_init();

    // -----------------

   __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef init;
    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Pin = GPIO_PIN_1;
    init.Pull = GPIO_NOPULL;
    init.Alternate = 0;
    init.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOE, &init);

    /* Loop forever */
	for(;;) {
        osDelay(1000);
        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1);
    }
}

// ---------------
// THIS CORE BOOTS ONLY AFTER M7 HAS FINISHED CLOCK INITIALIZATION
// ---------------
int main(void)
{
    // initialize FPU and several system blocks
    SystemInit();

    // initialize HAL library
    HAL_Init();

    icc_wait_for_M7_bootup();

    // update clock values
    SystemCoreClockUpdate();

    // -------------

    // initialize the FreeRTOS kernel
    osKernelInitialize();

    // create startup thread
    osThreadAttr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.stack_size = 2048;
    attr.name = "init";
    osThreadNew(task_startup, NULL, &attr);

    // start the FreeRTOS!
    osKernelStart();

	for(;;) {}
}

void flexptp_user_event_cb(PtpUserEventCode uev) {
    switch (uev) {
    case PTP_UEV_INIT_DONE:
        ptp_load_profile(ptp_profile_preset_get(FLEXPTP_INITIAL_PROFILE));
        ptp_print_profile();

        ptp_log_enable(PTP_LOG_DEF, true);
        ptp_log_enable(PTP_LOG_BMCA, true);
        break;
    default:
        break;
    }
}


// ------------------------

uint8_t ucHeap[configTOTAL_HEAP_SIZE] __attribute__((section(".FreeRTOSHeapSection")));

// ------------------------

void vApplicationTickHook(void) {
    HAL_IncTick();
}

void vApplicationIdleHook(void) {
    return;
}