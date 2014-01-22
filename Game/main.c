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
#include "Rand.h"
#include "GameWindow.h"

void __attribute__((signal)) TIMER1_COMPA_vect(void)
{
    sei();
    GameStep();
    TerminalRequestSize();
}

int main(void)
{
    UartInit();
    RandInit();
    
    TerminalUseAlternateBuffer();

    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1AH = (800 >> 8);
    OCR1AL = (800 & 0xFF);
    TIMSK1 = (1 << OCIE1A);
    
    while(1)
    {
        TIMSK1 &= ~(1 << OCIE1A);
        GameParseInput();
        GameRender();
        TIMSK1 |= (1 << OCIE1A);
    }
    
    return 0;
}

