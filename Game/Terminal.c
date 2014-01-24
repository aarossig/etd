/*
 * Terminal manipulation implementation
 */

#include "Terminal.h"

/*
 * Control Sequences
 */
const char CS_CCI[] PROGMEM = { 0x1B, 0x5B };
const char CS_GetWindowSize[] PROGMEM = { '1', '8', 't' };
const char CS_MoveCursor[] PROGMEM = { 'H' };
const char CS_HideCursor[] PROGMEM = { '?', '2', '5', 'l' };
const char CS_ShowCursor[] PROGMEM = { '?', '2', '5', 'h' };
const char CS_ClearScreen[] PROGMEM = { '2', 'J' };
const char CS_UseAlternateBuffer[] PROGMEM = { '?', '4', '7', 'h' };
const char CS_SetXTermBgColor[] PROGMEM = { '4', '8', ';', '5', ';' };
const char CS_SetXTermFgColor[] PROGMEM = { '3', '8', ';', '5', ';' };
const char CR_LF[] PROGMEM = { '\r', '\n' };

/*
 * Requests the size of the terminal window
 */
void TerminalRequestSize()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_GetWindowSize, 3);
}

/*
 * Moves the terminal cursor to the home position (1, 1)
 */
void TerminalCursorHome()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_MoveCursor, 1);
}

/*
 * Moves the terminal cursor to a point
 */
void TerminalCursorMove(const Point_t *p)
{
    UartPrintP(CS_CCI, 2);
    
    // Buffer size based on 3 characters per parameter plus the delimeters
    char buf[9];
    sprintf(buf, "%u;%uH", p->Y + 1, p->X + 1);
    UartPrint(buf, strlen(buf));
}

/*
 * Moves the terminal to x, y
 */
void TerminalCursorMoveXY(const uint8_t x, const uint8_t y)
{
    UartPrintP(CS_CCI, 2);
    
    // Buffer size based on 3 characters per parameter plus the delimeters
    char buf[9];
    sprintf(buf, "%u;%uH", y + 1, x + 1);
    UartPrint(buf, strlen(buf));
}

/*
 * Hides the terminal cursor
 */
void TerminalCursorHide()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_HideCursor, 4);
}

/*
 * Shows the terminal cursor
 */
void TerminalCursorShow()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_ShowCursor, 4);
}

/*
 * Switches the terminal emulator to the alternate buffer
 */
void TerminalUseAlternateBuffer()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_UseAlternateBuffer, 4);
}

/*
 * Clears the terminal window
 */
void TerminalClearScreen()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_ClearScreen, 2);
}

/*
 * Changes the foreground color of the terminal
 */
void TerminalSetFgColor(TermColor_t color)
{
    static TermColor_t prevColor = 0;
    
    if(color != prevColor)
    {
        UartPrintP(CS_CCI, 2);
        UartPrintP(CS_SetXTermFgColor, 5);
        
        char buf[5];
        sprintf(buf, "%um", color);
        UartPrint(buf, strlen(buf));
        
        prevColor = color;
    }
}

/*
 * Changes the background color of the terminal
 */
void TerminalSetBgColor(TermColor_t color)
{
    static TermColor_t prevColor = 0;

    if(color != prevColor)
    {
        UartPrintP(CS_CCI, 2);
        UartPrintP(CS_SetXTermBgColor, 5);
        
        char buf[5];
        sprintf(buf, "%um", color);
        UartPrint(buf, strlen(buf));
        
        prevColor = color;
    }
}

void TerminalInsertSpaces(uint8_t count)
{
    UartPrintP(CS_CCI, 2);
    
    char buf[5];
    sprintf(buf, "%u@", count);
    UartPrint(buf, strlen(buf));
}
