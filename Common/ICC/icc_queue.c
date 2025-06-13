#include "icc_queue.h"
#include <string.h>

void iccq_create(ICCQueue * q, volatile uint8_t * p, uint32_t length, uint32_t elemSize) {
    q->elements = p;
    q->length = length;
    q->elemSize = elemSize;
    q->readIdx = 0;
    q->writeIdx = 0;
    memset(q->elements, 0, length * elemSize);

    return q;
}

void iccq_clear(ICCQueue * q) {
    q->readIdx = 0;
    q->writeIdx = 0;
    memset(q->elements, 0, q->length * q->elemSize);
}

uint32_t iccq_avail(const ICCQueue * q) {
    return ((q->writeIdx < q->readIdx) ? (q->writeIdx + q->length) : (q->writeIdx)) - q->readIdx;
}

#define MQ_NEXT(size,current) (((current)+1)%(size))

bool iccq_push(ICCQueue * q, const void * src) {
    if (MQ_NEXT(q->length, q->writeIdx) == q->readIdx) { // cannot push, queue is full
        return false;
    }

    // can push
    memcpy(q->elements + q->writeIdx * q->elemSize, src, q->elemSize);

    // advance write pointer
    q->writeIdx = MQ_NEXT(q->length, q->writeIdx);

    return true;
}

void iccq_top(ICCQueue * q, void * dest) {
    memcpy(dest, q->elements + q->readIdx, q->elemSize);
}

void iccq_pop(ICCQueue * q) {
    if (iccq_avail(q) > 0) { // if there's something to pop
        q->readIdx = MQ_NEXT(q->length, q->readIdx);
    }
}

