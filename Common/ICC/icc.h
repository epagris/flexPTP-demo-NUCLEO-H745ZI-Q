#ifndef ICC_ICC
#define ICC_ICC

#include <stdbool.h>
#include <stdint.h>

#include "icc_queue.h"

#define ICC_QUEUE_LENGTH (4096)

// typedef struct {
//     uint8_t p[ICC_COPYBUF_LEN]; ///< Buffer area
//     uint16_t n; ///< Size of stored content
// } ICC_CopyBuf;

typedef struct {
    ICCQueue sevenBound, fourBound; ///< Copy buffers for each direction
    uint8_t pSevenBound[ICC_QUEUE_LENGTH]; ///< Data area for the queues
    uint8_t pFourBound[ICC_QUEUE_LENGTH]; 
} ICC_SharedData;

#define ICC_WAKEUP_SEMID (0) // semaphore ID for wakeing up the CM4 core from the CM7 core

#define ICC_SEVENBOUND_SEMID (1) // semaphore IDs for communication between the cores
#define ICC_FOURBOUND_SEMID (2)

// ------ CM7 ONLY --------

/**
 * Initialize FIFOs.
*/
void icc_init();

/**
 * Wake up the M4 core.
*/
void icc_wake_up_M4();

// ------ CM4 ONLY --------

/**
 * Wait for the M7 core's signal.
*/
void icc_wait_for_M7_bootup();

// ------ IMPLEMENTED ON BOTH CORES --------

/**
 * Open up the pipe between the two cores.
*/
void icc_open_pipe();

/**
 * Notify the other core.
*/
void icc_notify();

/**
 * Get the outbound pipe.
 * @return pointer to outbound pipe
*/
ICCQueue * icc_get_outbound_pipe();

/**
 * Get the inbound pipe.
 * @return pointer to inbound pipe
*/
ICCQueue * icc_get_inbound_pipe();

#endif /* ICC_ICC */
