/*
 * Terminal window implementation
 */

#include "GameWindow.h"

/*
 * Control Sequences
 */
const char CS_CCI[] PROGMEM = { 0x1B, 0x5B };
const char CS_GetWindowSize[] PROGMEM = { '1', '8', 't' };
const char CS_MoveCursor[] PROGMEM = { 'H' };
const char CS_HideCursor[] PROGMEM = { '?', '2', '5', 'l' };
const char CS_UseAlternateBuffer[] PROGMEM = { '?', '4', '7', 'h' };
const char CR_LF[] PROGMEM = { '\r', '\n' };

#define MAP_WIDTH 160
#define MAP_HEIGHT 48
#define TILE_COUNT (MAP_WIDTH * MAP_HEIGHT)

const MapTile_t mapTiles[] PROGMEM = {{ .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Water },
                                      { .Length = 0xFF, .TileType = Grass },
                                      { .Length = 0xFF, .TileType = Grass },
                                      { .Length = 0xFF, .TileType = Grass },
                                      { .Length = 0xFF, .TileType = Water },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Water },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Grass },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Grass },
                                      { .Length = 0xFF, .TileType = Grass },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Water },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Water },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0xFF, .TileType = Grass },
                                      { .Length = 0xFF, .TileType = Grass },
                                      { .Length = 0xFF, .TileType = Water },
                                      { .Length = 0xFF, .TileType = Stone },
                                      { .Length = 0x1E, .TileType = Stone }};
/*
 * Initializes a terminal window
 */
void GameWindowInit(GameWindow_t *window)
{
    window->WindowWidth = TERMINAL_DEF_WIDTH;
    window->WindowHeight = TERMINAL_DEF_HEIGHT;
    
    window->MapX = 0;
    window->MapY = 0;

    window->CursorX = (window->WindowWidth >> 1);
    window->CursorY = (window->WindowHeight >> 1);
}

/*
 * Gets the type of map tile given x and y
 */
MapTileType_t GameWindowGetTile(GameWindow_t *window, uint16_t x, uint16_t y)
{
    int32_t tileNum = x + (y * MAP_WIDTH);
    uint16_t i = 0;
    
    uint8_t length = pgm_read_byte(&(mapTiles[i].Length));

    while((tileNum - length) > 0)
    {
        tileNum -= length;
        length = pgm_read_byte(&(mapTiles[++i].Length));
    }

    return pgm_read_byte(&(mapTiles[i].TileType));
}

/*
 * Requests the size of the terminal window
 */
void GameWindowRequestSize()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_GetWindowSize, 3);
}

/*
 * Moves the terminal cursor to the home position (1, 1)
 */
void GameWindowCursorHome()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_MoveCursor, 1);
}

/*
 * Moves the terminal cursor to the position x, y
 */
void GameWindowCursorMove(uint16_t x, uint16_t y)
{
    char buf[5] = { 0 };

    UartPrintP(CS_CCI, 2);

    sprintf(buf, "%d", y);
    UartPrint(buf, strlen(buf));

    UartTransmitByte(';');

    sprintf(buf, "%d", x);
    UartPrint(buf, strlen(buf));
    
    UartTransmitByte('H');
}

/*
 * Hides the terminal cursor
 */
void GameWindowCursorHide()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_HideCursor, 4);
}

/*
 * Instructs the terminal emulator to use an alternate buffer with no scrollback
 */
void GameWindowUseAlternateBuffer()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_UseAlternateBuffer, 4);
}

/*
 * Changes the color of the terminal
 */
void GameWindowSetColor(TermColor_t bgColor, TermColor_t fgColor)
{
    static TermColor_t prevBgColor = -1;
    static TermColor_t prevFgColor = -1;
    
    if(bgColor != prevBgColor
        || fgColor != prevFgColor)
    {
        UartPrintP(CS_CCI, 2);

        UartTransmitByte('0');
        UartTransmitByte(';');

        char buf[5] = { 0 };
        
        sprintf(buf, "%d", 30 + fgColor);
        UartPrint(buf, strlen(buf));
        
        UartTransmitByte(';');
        
        sprintf(buf, "%d", 40 + bgColor);
        UartPrint(buf, strlen(buf));
        
        UartTransmitByte('m');

        prevBgColor = bgColor;
        prevFgColor = fgColor;
    }
}

/*
 * Parses user input
 */
void GameWindowParseInput(GameWindow_t *window)
{
    static uint8_t csCount = 0;
    uint8_t bytes = UartBytesToReceive();

    while(!UartBytesToReceive());

    while(bytes--)
    {
        uint8_t b = UartReceiveByte();
        
        // CCI Begin (ESC)
        if(b == 0x1B && csCount == 0)
        {
            csCount++;
        }
        // CCI Begin ([)
        else if(b == 0x5B && csCount == 1)
        {
            csCount++;
        }
        // Up Arrow
        else if(b == 0x41 && csCount == 2)
        {
            window->MapY =
                (window->MapY == (MAP_HEIGHT - window->WindowHeight - 1))
                    ? window->MapY : window->MapY + 1;
            csCount = 0;
        }
        // Down Arrow
        else if(b == 0x42 && csCount == 2)
        {
            // Move map up
            window->MapY = (window->MapY == 0) ? 0 : window->MapY - 1;
            csCount = 0;
        }
        // Right Arrow
        else if(b == 0x43 && csCount == 2)
        {
            window->MapX = (window->MapX == 0) ? 0 : window->MapX - 1;
            csCount = 0;
        }
        // Left Arrow
        else if(b == 0x44 && csCount == 2)
        {
            window->MapX =
                (window->MapX == (MAP_WIDTH - window->WindowWidth - 1))
                    ? window->MapX : window->MapX + 1;
            csCount = 0;
        }
        // Cursor Up (w)
        else if(b == 'w' && csCount == 0)
        {
            window->CursorY = (window->CursorY == 0) ? 0 : window->CursorY - 1;
        }
        // Cursor Down (s)
        else if(b == 's' && csCount == 0)
        {
            window->CursorY = (window->CursorY == (MAP_HEIGHT - 1))
                ? window->CursorY : window->CursorY + 1;
        }
        // Cursor Right (d)
        else if(b == 'd' && csCount == 0)
        {
            window->CursorX = (window->CursorX == (MAP_WIDTH - 1))
                ? window->CursorX : window->CursorX + 1;
        }
        // Cursor Left (a)
        else if(b == 'a' && csCount == 0)
        {
            window->CursorX = (window->CursorX == 0) ? 0 : window->CursorX - 1;
        }
        else
        {
            csCount = 0;
        }
    }
}

void GameWindowRenderScreen(GameWindow_t *window)
{
    GameWindowRequestSize();
    
    for(uint16_t y = 1;
            y < (window->WindowHeight - 1) && (window->MapY + y) < MAP_HEIGHT;
            y++)
    {
        GameWindowCursorMove(2, y + 1);

        uint16_t i = 0;
        uint8_t j = 0;
        
        int32_t tileNum = window->MapX + ((window->MapY + y) * MAP_WIDTH);
        uint8_t length = pgm_read_byte(&(mapTiles[i].Length));

        while((tileNum - length) > 0)
        {
            tileNum -= length;
            length = pgm_read_byte(&(mapTiles[++i].Length));
        }
        
        j = (tileNum - length);
        
        for(uint16_t x = 1;
            x < (window->WindowWidth - 1) && (window->MapX + x) < MAP_WIDTH;
            x++)
        {
            if(j++ >= pgm_read_byte(&(mapTiles[i].Length)))
            {
                j = 0;
                i++;
            }
            
            switch(pgm_read_byte(&(mapTiles[i].TileType)))
            {
                case Empty:
                    UartTransmitByte(' ');
                    break;
                case Stone:
                    GameWindowSetColor(TermColor_Black, TermColor_White);
                    UartTransmitByte('#');
                    break;
                case Water:
                    GameWindowSetColor(TermColor_Blue, TermColor_White);
                    UartTransmitByte('~');
                    break;
                case Grass:
                    GameWindowSetColor(TermColor_Green, TermColor_White);
                    UartTransmitByte('.');
                    break;
                default:
                    UartTransmitByte('X');
            }
        }
        
        if((y < (window->WindowHeight - 1))
            && ((window->MapY + y) < (MAP_HEIGHT - 1)))
        {
           UartPrintP(CR_LF, 2);
        }
    }
}

