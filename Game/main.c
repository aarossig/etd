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

void __attribute__((signal)) TIMER0_COMPA_vect(void)
{
    GameWindowRequestSize();
}

GameWindow_t gameWindow;

int main(void)
{
    UartInit();
    
    GameWindowInit(&gameWindow);
    GameWindowCursorHide();
    GameWindowUseAlternateBuffer();

    // Configure a ~60Hz interrupt
    TCCR0A = (1 << WGM01);
    OCR0A= 0xFF;
    TCCR0B = (1 << CS02) | (1 << CS00);
    TIMSK0 = (1 << OCIE0A);
    
    while(1)
    {
        TIMSK0 &= ~(1 << OCIE0A);
        GameWindowRenderScreen(&gameWindow);
        GameWindowParseInput(&gameWindow);
        TIMSK0 |= (1 << OCIE0A);
    }
    
    return 0;
}

