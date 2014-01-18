/*
 * Circular Buffer Implementation
 */

#include "CircularBuffer.h"

/*
 * Determines whether the buffer is empty
 */
uint8_t CircularBufferIsEmpty(volatile CircularBuffer_t *buf)
{
    return (buf->Count == 0);
}

/*
 * Determines whether the buffer is full
 */
uint8_t CircularBufferIsFull(volatile CircularBuffer_t *buf)
{
    return (buf->Count == buf->Size);
}

/*
 * Inserts and element into the buffer, overwriting the oldest element if the
 * buffer is full.
 */
void CircularBufferWrite(volatile CircularBuffer_t *buf, uint8_t elem)
{
    if(!CircularBufferIsFull(buf))
    {
        buf->Count++;
    }
    
    buf->Buf[buf->Write] = elem;
    buf->Write = (buf->Write == (buf->Size - 1)) ? 0 : buf->Write + 1;
}

/*
 * Reads an element from the buffer.
 */
uint8_t CircularBufferRead(volatile CircularBuffer_t *buf)
{
    if(!CircularBufferIsEmpty(buf))
    {
        buf->Count--;
    }
    
    uint8_t b = buf->Buf[buf->Read];
    buf->Read = (buf->Read == (buf->Size - 1)) ? 0 : buf->Read + 1;
    return b;
}

