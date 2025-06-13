#ifndef ICC_ICC_QUEUE
#define ICC_ICC_QUEUE

#include <stdbool.h>
#include <stdint.h>

/**
 * Generic Queue.
 */
typedef struct _ICCQueue {
    uint32_t writeIdx; ///< Next block to write
    uint32_t readIdx; ///< Next block to read
    uint32_t length; ///< Size of circular buffer
    uint32_t elemSize; ///< Element size
    volatile uint8_t * elements; ///< Array of packets
} ICCQueue;

/**
 * Create Queue.
 * @param q pointer to uninitialized ICCQueue instance
 * @param p pointer to buffer area that is going to be assigned to the queue
 * @param length length of circular buffer
 * @param elemSize element size
 * @return pointer to Queue instance OR NULL on failure
 */
void  iccq_create(ICCQueue * q, volatile uint8_t * p, uint32_t length, uint32_t elemSize);

/**
 * Create Queue based on storage type.
 */
#define ICCQ_CREATE_T(length,T) iccq_create((length), sizeof(T))

/**
 * Clear circular buffer.
 * @param q pointer to Queue
 */
void iccq_clear(ICCQueue * q);

/**
 * Get number of available elements.
 * @param q pointer to Queue
 * @return number of available elements
 */
uint32_t iccq_avail(const ICCQueue * q);

/**
 * Push element to the Queue.
 * @param q pointer to Queue
 * @param raw pointer to raw packet
 * @return true on success, false on failure (e.g.: queue full)
 */
bool iccq_push(ICCQueue * q, const void * src);

/**
 * Get top element.
 * @param q pointer to Queue
 * @return top element (COPY, NOT POINTER!)
 */
void iccq_top(ICCQueue * q, void * dest);

/**
 * Pop top element.
 * @param q pointer to Queue
 */
void iccq_pop(ICCQueue * q);

#endif /* ICC_ICC_QUEUE */
