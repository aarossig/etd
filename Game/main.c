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

GameWindow_t gameWindow;

int main(void)
{
    UartInit();
    GameWindowInit(&gameWindow);
    GameWindowCursorHide();
    GameWindowUseAlternateBuffer();
    GameWindowRenderScreen(&gameWindow);
        
    while(1)
    {
        GameWindowParseInput(&gameWindow);
        GameWindowRenderScreen(&gameWindow);
    }
    
    return 0;
}

