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
const char CS_ShowCursor[] PROGMEM = { '?', '2', '5', 'h' };
const char CS_ClearScreen[] PROGMEM = { '2', 'J' };
const char CS_UseAlternateBuffer[] PROGMEM = { '?', '4', '7', 'h' };
const char CS_SetXTermBgColor[] PROGMEM = { '4', '8', ';', '5', ';' };
const char CS_SetXTermFgColor[] PROGMEM = { '3', '8', ';', '5', ';' };
const char CR_LF[] PROGMEM = { '\r', '\n' };

#define PARAM_BUF_LEN 5
#define PARAM_COUNT 2

#define BORDER_WIDTH 1

#define MAP_WIDTH 121
#define MAP_HEIGHT 121

#include "Map.h"

#define ENTRY_POINT_COUNT 3

Point_t basePosition = { .X = 51, .Y = 31 };

Point_t entryPoints[] = {{ .X = 23, .Y = 0 },
                         { .X = 107, .Y = 0},
                         { .X = 0, .Y = 15}};

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

    window->Gold = DEFAULT_MONEY;
}

/*
 * Gets the type of map tile given x and y
 */
MapTileType_t GameWindowGetTile(GameWindow_t *window, uint8_t x, uint8_t y)
{
    uint16_t tileNum = (y * MAP_WIDTH) + x;
    uint8_t tile = pgm_read_byte(&(mapTiles[tileNum >> 2]));
    return (tile >> ((tileNum & 0b11) << 1)) & 0b11;
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
void GameWindowCursorMove(uint8_t x, uint8_t y)
{
    char buf[12] = { 0 };

    UartPrintP(CS_CCI, 2);

    sprintf(buf, "%d;%dH", y, x);
    UartPrint(buf, strlen(buf));
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
 * Shows the terminal cursor
 */
void GameWindowCursorShow()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_ShowCursor, 4);
}

/*
 * Instructs the terminal emulator to use an alternate buffer with no scrollback
 */
void GameWindowUseAlternateBuffer()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_UseAlternateBuffer, 4);
}

void GameWindowClearScreen()
{
    UartPrintP(CS_CCI, 2);
    UartPrintP(CS_ClearScreen, 2);
}

/*
 * Changes the foreground color of the terminal
 */
void GameWindowSetFgColor(TermColor_t color)
{
    static TermColor_t prevColor = 0;
    
    if(color != prevColor)
    {
        UartPrintP(CS_CCI, 2);
        UartPrintP(CS_SetXTermFgColor, 5);
        
        char buf[6];
        
        sprintf(buf, "%dm", color);
        UartPrint(buf, strlen(buf));
        
        prevColor = color;
    }
}

/*
 * Changes the background color of the terminal
 */
void GameWindowSetBgColor(TermColor_t color)
{
    static TermColor_t prevColor = 0;

    if(color != prevColor)
    {
        UartPrintP(CS_CCI, 2);
        UartPrintP(CS_SetXTermBgColor, 5);
        
        char buf[6];
        
        sprintf(buf, "%dm", color);
        UartPrint(buf, strlen(buf));
        
        prevColor = color;
    }
}

/*
 * Parses user input
 */
void GameWindowParseInput(GameWindow_t *window)
{
    static uint8_t csCount = 0;
    static char params[PARAM_COUNT][PARAM_BUF_LEN] = {{ 0 }};

    while(UartBytesToReceive())
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
        // Down Arrow
        else if(b == 0x42 && csCount == 2)
        {
            if(window->MapY <
                        (MAP_HEIGHT - window->WindowHeight + (BORDER_WIDTH * 2))
                && window->WindowHeight < MAP_HEIGHT)
            {
                window->MapY++;
            }

            csCount = 0;
        }
        // Up Arrow
        else if(b == 0x41 && csCount == 2)
        {
            // Move map up
            window->MapY = (window->MapY == 0) ? 0 : window->MapY - 1;
            csCount = 0;
        }
        // Left Arrow
        else if(b == 0x44 && csCount == 2)
        {
            window->MapX = (window->MapX == 0) ? 0 : window->MapX - 1;
            csCount = 0;
        }
        // Right Arrow
        else if(b == 0x43 && csCount == 2)
        {
            if(window->MapX <
                          (MAP_WIDTH - window->WindowWidth + (BORDER_WIDTH * 2))
                && window->WindowWidth < MAP_WIDTH)
            {
                window->MapX++;
            }

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
        // Begin processing the size of the terminal window
        else if(b == '8' && csCount == 2)
        {
            csCount++;
        }
        // First argument terminate
        else if(b == ';' && csCount == 3)
        {
            csCount++;
        }
        // Read a numeric parameter response
        else if(b >= 0x30 && b < 0x3A && csCount >= 4 && csCount < 9)
        {
            params[0][csCount - 4] = b;
            csCount++;
        }
        // Second argument terminate
        else if(b == ';' && csCount >= 5 && csCount < 10)
        {
            csCount = 10;
        }
        // Read a numeric parameter response
        else if(b >= 0x30 && b < 0x3A && csCount >= 10 && csCount < 15)
        {
            params[1][csCount - 10] = b;
            csCount++;
        }
        // Terminal window size changed terminate
        else if(b == 't' && csCount >= 11 && csCount < 16)
        {
            uint16_t width = TERMINAL_DEF_WIDTH;
            uint16_t height = TERMINAL_DEF_HEIGHT;
            
            sscanf(params[0], "%d", &height);
            sscanf(params[1], "%d", &width);

            height = (height > 255) ? 255 : height;
            width = (width > 255) ? 255 : width;
            
            if(window->WindowWidth != width
                || window->WindowHeight != height)
            {
                window->MapX = 0;
                window->MapY = 0;
            }

            window->WindowWidth = width;
            window->WindowHeight = height;

            csCount = 0;
            
            // Clear the parameter buffers
            memset(params, 0, PARAM_COUNT * PARAM_BUF_LEN);
        }
        else
        {
            csCount = 0;
        }
    }
}

void GameWindowRenderScreen(GameWindow_t *window)
{
    static GameWindow_t prevWindow = { 0 };
    
    if(prevWindow.WindowWidth != window->WindowWidth
        || prevWindow.WindowHeight != window->WindowHeight)
    {
        GameWindowSetBgColor(TermColor_NearBlack);
        GameWindowClearScreen();
        GameWindowRenderMap(window);
        GameWindowRenderBorders(window);
        GameWindowRenderCursor(window);

        prevWindow = *window;
    }
    else
    {
        if(prevWindow.MapX != window->MapX || prevWindow.MapY != window->MapY)
        {
            GameWindowRenderMap(window);
            GameWindowRenderCursor(window);
        }

        if(prevWindow.Gold != window->Gold)
        {
            GameWindowRenderBorders(window);
        }
        
        if(prevWindow.CursorX != window->CursorX
            || prevWindow.CursorY != window->CursorY)
        {
            GameWindowRenderCursor(window);
        }

        prevWindow = *window;
    }
}

void GameWindowRenderMap(GameWindow_t *window)
{
    GameWindowCursorHide();
    
    for(uint8_t y = 0;
        y < (window->WindowHeight - (BORDER_WIDTH * 2))
            && (window->MapY + y) < MAP_HEIGHT;
        y++)
    {
        GameWindowCursorMove(BORDER_WIDTH + 1, y + BORDER_WIDTH + 1);
        
        for(uint8_t x = 0;
            x < (window->WindowWidth - (BORDER_WIDTH * 2))
                && (window->MapX + x) < MAP_WIDTH;
            x++)
        {
            MapTileType_t tile =
                GameWindowGetTile(window, window->MapX + x, window->MapY + y); 
            GameWindowRenderTile(tile);
        }
    }

    int16_t baseX = basePosition.X - window->MapX;
    int16_t baseY = basePosition.Y - window->MapY;
    
    if(baseX > 0 && baseX < window->WindowWidth
        && baseY > 0 && baseY < window->WindowHeight)
    {
        GameWindowCursorMove(baseX + BORDER_WIDTH + 1,
            baseY + BORDER_WIDTH + 1);
        GameWindowSetBgColor(TermColor_Gold);
        UartTransmitByte(' ');
    }
}

void GameWindowRenderTile(MapTileType_t tile)
{
    if(tile == Empty)
    {
        GameWindowSetBgColor(TermColor_Black);
        UartTransmitByte(' ');
    }
    else if(tile == Stone)
    {
        GameWindowSetBgColor(TermColor_StoneBg);
        GameWindowSetFgColor(TermColor_StoneFg);
        UartTransmitByte('#');
    }
    else if(tile == Water)
    {
        GameWindowSetBgColor(TermColor_WaterBg);
        GameWindowSetFgColor(TermColor_WaterFg);
        UartTransmitByte('~');
    }
    else if(tile == Grass)
    {
        GameWindowSetBgColor(TermColor_GrassBg);
        GameWindowSetFgColor(TermColor_GrassFg);
        UartTransmitByte('.');
    }
}

void GameWindowRenderTilePosition(MapTileType_t tile, uint8_t x, uint8_t y)
{
    GameWindowCursorMove(x + 1, y + 1);
    GameWindowRenderTile(tile);
}

void GameWindowRenderBorders(GameWindow_t *window)
{
    GameWindowCursorMove(5, 1);
    GameWindowSetBgColor(TermColor_NearBlack);
    GameWindowSetFgColor(TermColor_White);

    char buf[17];
    sprintf(buf, "Gold: %ld", window->Gold);
    UartPrint(buf, strlen(buf));
}

void GameWindowRenderCursor(GameWindow_t *window)
{
    int16_t cursorX = window->CursorX - window->MapX;
    int16_t cursorY = window->CursorY - window->MapY;

    if(cursorX < 0)
    {
        cursorX = 0;
    }
    else if(cursorX > (window->WindowWidth - BORDER_WIDTH))
    {
        cursorX = window->WindowWidth;
    }
    else
    {
        cursorX += BORDER_WIDTH;
    }

    if(cursorY < 0)
    {
        cursorY = 0;
    }
    else if(cursorY > (window->WindowHeight - BORDER_WIDTH))
    {
        cursorY = window->WindowHeight;
    }
    else
    {
        cursorY += BORDER_WIDTH;
    }
    
    GameWindowSetBgColor(TermColor_Black);
    GameWindowSetFgColor(TermColor_White);
    
    // Rows and columns are 1-indexed
    GameWindowCursorMove(cursorX + 1, cursorY + 1);
    GameWindowCursorShow();
}
