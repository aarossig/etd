/*
 * A circular buffer data structure. A read/write counter is used to track the
 * capacity of the buffer remaining.
 */

#ifndef CircularBuffer_h
#define CircularBuffer_h

#include <stdint.h>

/* Ring Buffer Data Structure *************************************************/

typedef struct CircularBuffer_t {
    uint8_t Read;
    uint8_t Write;
    uint8_t Count;
    uint8_t Size;
    uint8_t *Buf;
} CircularBuffer_t;

/* Functions ******************************************************************/

uint8_t CircularBufferIsEmpty(volatile CircularBuffer_t *buf);
uint8_t CircularBufferIsFull(volatile CircularBuffer_t *buf);

void CircularBufferWrite(volatile CircularBuffer_t *buf, uint8_t elem);
uint8_t CircularBufferRead(volatile CircularBuffer_t *buf);

#endif
