/*
 * Embedded Tower Defense!
 * 
 * This program aims to implement a tower defense game using a standard
 * terminal as the user interface.
 *
 * Enjoy!
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "Uart.h"

int main(void)
{
    UartInit();
    sei();
    
    while(1)
    {
        uint8_t b = UartReceiveByte();
        UartTransmitByte(b);
    }
    
	return 0;
}

