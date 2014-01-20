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
#include "GameWindow.h"

void __attribute__((signal)) TIMER0_OVF_vect(void)
{
    TerminalRequestSize();
    //GameStep();
    TCNT0 = 0;
}

int main(void)
{
    UartInit();
    
    TerminalUseAlternateBuffer();

    // Configure a ~60Hz interrupt
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS02) | (1 << CS00);
    TIMSK0 = (1 << TOIE0);
    
    while(1)
    {
        TIMSK0 &= ~(1 << TOIE0);
        GameRender();
        GameParseInput();
        TIMSK0 |= (1 << TOIE0);
    }
    
    return 0;
}

