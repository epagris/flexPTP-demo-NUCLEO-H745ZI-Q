#include "icc.h"

#include "stm32h7xx_hal.h"

ICC_SharedData sharedData __attribute__((section(".icc_section")));

__weak void icc_recv_cb() {
    return;
}

void icc_wait_for_M7_bootup() {
    // enable HSEM clock
    __HAL_RCC_HSEM_CLK_ENABLE();

    // activate notification for this core (CM4)
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(ICC_WAKEUP_SEMID));

    // stop the core and wait for wake up signal from the M7 core
    HAL_PWREx_ClearPendingEvent();
    HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);

    // -----
    // WAIT
    // -----

    __HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(ICC_WAKEUP_SEMID));

    // deactivate notification on wake up semaphore
    HAL_HSEM_DeactivateNotification(__HAL_HSEM_SEMID_TO_MASK(ICC_WAKEUP_SEMID));
}

void icc_open_pipe() {
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(ICC_FOURBOUND_SEMID));
    HAL_NVIC_SetPriority(HSEM2_IRQn, 0x7, 0);
    HAL_NVIC_EnableIRQ(HSEM2_IRQn);
}

void HAL_HSEM_FreeCallback(uint32_t SemMask) {
    icc_recv_cb();
    //usb_cdc_write("IRQ\r\n", 5);
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(ICC_FOURBOUND_SEMID));
}

void HSEM2_IRQHandler() {
    HAL_HSEM_IRQHandler();
}

void icc_notify() {
    if (HAL_HSEM_FastTake(ICC_SEVENBOUND_SEMID) == HAL_OK) {
        HAL_HSEM_Release(ICC_SEVENBOUND_SEMID, 0);
    }
}

// ---------------

ICCQueue * icc_get_outbound_pipe() {
    return &(sharedData.sevenBound);
}

ICCQueue * icc_get_inbound_pipe() {
    return &(sharedData.fourBound);
}