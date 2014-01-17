/*
 * Simplistic UART Driver for AVR Microcontrollers
 *
 * Uses a circular buffer for Tx and Rx. A counter is used to keep track of
 * the number of reads and writes in order to keep track of whether the buffer
 * is full or empty.
 */

#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "CircularBuffer.h"

#define BAUD 9600UL

void UartInit(void);
void UartTransmitByte(uint8_t b);
uint8_t UartReceiveByte(void);

#endif
