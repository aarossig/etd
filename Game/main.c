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

int main(void)
{
    UartInit();
    RandInit();
    
    TerminalUseAlternateBuffer();

    TCCR1B = (1 << CS12) | (1 << CS10);
    
    while(1)
    {
        TCCR1B = (1 << CS12) | (1 << CS10);
        
        GameParseInput();
        GameRender();
        
        TCCR1B &= ~((1 << CS12) | (1 << CS10));

        uint16_t countValue = (TCNT1H << 8) | (TCNT1L);
        if(countValue > 800)
        {
            GameStep();
            TerminalRequestSize();
            
            TCNT1H = 0;
            TCNT1L = 0;
        }
    }
    
    return 0;
}

