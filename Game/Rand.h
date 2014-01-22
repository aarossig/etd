/*
 * ADC based random number generator for AVR microcontrollers
 */

#ifndef RAND_H
#define RAND_H

#include <avr/io.h>
#include <stdint.h>

#include "Bool.h"

void RandInit();
bool RandGetBit();
bool RandGetUnfilteredBit();
uint8_t RandGetByte();


#endif
