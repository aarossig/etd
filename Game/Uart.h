/*
 * Simplistic UART Driver for AVR Microcontrollers
 *
 * Uses a circular buffer for Tx and Rx. A counter is used to keep track of
 * the number of reads and writes in order to keep track of whether the buffer
 * is full or empty.
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "Interrupts.h"
#include "CircularBuffer.h"
#include "Bool.h"

#define BAUD 115200UL

void UartInit(void);

void UartTransmitByte(uint8_t b);

uint8_t UartReceiveByte(void);
uint8_t UartBytesToReceive();

void UartPrint(const char *str, uint8_t len);
void UartPrintP(const char *str, uint8_t len);


#endif
