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

Size_t windowSize;
Point_t viewPosition;
Point_t cursorPosition;
int32_t gold;
uint8_t botCount;
uint8_t level;

/* Dirty Below Here... ********************************************************/

#define PARAM_BUF_LEN 5
#define PARAM_COUNT 2

#define BORDER_WIDTH 1

#define MAP_WIDTH 121
#define MAP_HEIGHT 121

/*
 * Game Specifications
 *
 * * Map, Protect Point, Bot Entry Points, Level Specifications etc.
 */

#include "Map.h"

#define MAX_BOTS 32

const Point_t basePosition = { .X = /*51*/9, .Y = /*31*/12 };

#define ENTRY_POINT_COUNT 3
const Point_t entryPoints[] PROGMEM = {{ .X = 23, .Y = 0 },
                                       { .X = 107, .Y = 0},
                                       { .X = 0, .Y = 15}};

#define LEVEL_COUNT 1
const LevelSpec_t levels[] PROGMEM = {{ .LevelName = "Beginners Luck",
                                        .BotCount = 6,
                                        .MoveSpeed = 20,
                                        .HealthPoints = 10,
                                        .AttackSpeed = 10,
                                        .AttackDamage = 2,
                                        .KillReward = 10 }};

/*
 * Game Variables
 *
 * * Bots, Current Level etc.
 */

#define WEIGHT_COUNT 400

uint8_t weightCount = 0;
TileWeight_t weights[WEIGHT_COUNT];
BotInstance_t bots[MAX_BOTS];

uint8_t GameStoreWeight(Point_t *p, uint8_t weight)
{
    if(weightCount < WEIGHT_COUNT)
    {
        weights[weightCount].Position = *p;
        weights[weightCount++].Weight = weight;
    }

    return weightCount;
}

void GameClearWeights()
{
    weightCount = 0;
}

bool GameWeightsFull()
{
    return weightCount >= WEIGHT_COUNT;
}


TileWeight_t *GameWeightByPoint(Point_t *p)
{
    for(uint8_t i = 0; i < weightCount; i++)
    {
        if(PointsEqual(&weights[i].Position, p))
        {
            return &weights[i];
        }
    }

    return NULL;
}

void GameStep(GameWindow_t *window)
{
    static int16_t time = 0;

    uint8_t moveSpeed = pgm_read_byte(&(levels[window->Level].MoveSpeed));
    uint8_t botCount = pgm_read_byte(&(levels[window->Level].BotCount));

    if((time % moveSpeed) == 0)
    {
        for(uint8_t i = 0; i < window->BotCount; i++)
        {
            GameClearWeights();
            GameStoreWeight(&bots[i].Position, 0);

            uint8_t weightResult = 0;
            
            for(uint8_t i = 0; i < weightCount; i++)
            {
                TileWeight_t weight = weights[i];
                Point_t p = weight.Position;
                PointAddDirection(&p, Direction_North);
                
                if(p.Y >= 0 && GameWindowGetTile(window, &p) != Stone)
                {
                    if(!GameWeightByPoint(&p))
                        if(!GameStoreWeight(&p, weight.Weight + 1))
                            break;
                    
                    if(PointsEqual(&p, &basePosition))
                    {
                        weightResult = weight.Weight + 1;
                        break;
                    }
                }
                
                p = weight.Position;
                PointAddDirection(&p, Direction_East);
                
                if(p.X < MAP_WIDTH && GameWindowGetTile(window, &p) != Stone)
                {
                    if(!GameWeightByPoint(&p))
                        if(!GameStoreWeight(&p, weight.Weight + 1))
                            break;

                    if(PointsEqual(&p, &basePosition))
                    {
                        weightResult = weight.Weight + 1;
                        break;
                    }
                }
                
                p = weight.Position;
                PointAddDirection(&p, Direction_South);
                
                if(p.Y < MAP_HEIGHT && GameWindowGetTile(window, &p) != Stone)
                {
                    if(!GameWeightByPoint(&p))
                        if(!GameStoreWeight(&p, weight.Weight + 1))
                            break;

                    if(PointsEqual(&p, &basePosition))
                    {
                        weightResult = weight.Weight + 1;
                        break;
                    }
                }
                
                p = weight.Position;
                PointAddDirection(&p, Direction_West);
                
                if(p.X >= 0 && GameWindowGetTile(window, &p) != Stone)
                {
                    if(!GameWeightByPoint(&p))
                        if(!GameStoreWeight(&p, weight.Weight + 1))
                            break;

                    if(PointsEqual(&p, &basePosition))
                    {
                        weightResult = weight.Weight + 1;
                        break;
                    }
                }
            }
            
            TileWeight_t *weight = NULL;
            const Point_t *workingPoint = &basePosition;
            
            while(--weightResult > 0)
            {
                weight = GameWeightByPointWeight(workingPoint, weightResult);

                if(weight != NULL)
                    workingPoint = &weight->Position;
                else
                    break;
            }

            if(weight != NULL)
            {
                bots[i].Position = weight->Position;
            }
            else
            {
                // Move bots closer to the base along the longer axis
            }
        }
        
        // Place bots if required
        if(window->BotCount < botCount)
        {
            for(uint8_t i = 0; i < ENTRY_POINT_COUNT; i++)
            {
                if(!GameBotAtLocationP(window, &entryPoints[i]))
                {
                    GameNewBotP(window, &entryPoints[i]);
                }
            }
        }
    }
}

TileWeight_t *GameWeightByPointWeight(const Point_t *p, uint8_t weight)
{
    TileWeight_t *tile = NULL;
    Point_t pTest = *p;
    pTest.Y -= 1;
    
    tile = GameWeightByPoint(&pTest);
    if(tile != NULL && tile->Weight == weight)
        return tile;
    
    tile = NULL;
    pTest = *p;
    pTest.X += 1;
    
    tile = GameWeightByPoint(&pTest);
    if(tile != NULL && tile->Weight == weight)
        return tile;
    
    tile = NULL;
    pTest = *p;
    pTest.Y += 1;
    
    tile = GameWeightByPoint(&pTest);
    if(tile != NULL && tile->Weight == weight)
        return tile;
    
    tile = NULL;
    pTest = *p;
    pTest.X -= 1;
    
    tile = GameWeightByPoint(&pTest);
    if(tile != NULL && tile->Weight == weight)
        return tile;

    return NULL;
}

void GameNewBotP(GameWindow_t *window, const Point_t *p)
{
    bots[window->BotCount].HealthPoints =
        pgm_read_byte(&(levels[window->Level]));
    
    bots[window->BotCount].Position.X =
        pgm_read_byte(&(p->X));

    bots[window->BotCount].Position.Y =
        pgm_read_byte(&(p->Y));

    GameRenderBot(window, &bots[window->BotCount++]);
}

void GameRenderBot(GameWindow_t *window, BotInstance_t *bot)
{
    int16_t botX = bot->Position.X - window->MapPos.X;
    int16_t botY = bot->Position.Y - window->MapPos.Y;
    
    if(botX >= 0
        && botX < window->WindowWidth - (BORDER_WIDTH * 2)
        && botY >= 0
        && botY < window->WindowHeight - (BORDER_WIDTH * 2))
    {
        GameWindowCursorMove(botX + BORDER_WIDTH + 1, botY + BORDER_WIDTH + 1);
        GameWindowSetBgColor(TermColor_BotBg);
        GameWindowSetFgColor(TermColor_BotFg);
        UartTransmitByte('%');
    }
}

bool GameBotAtLocationP(GameWindow_t *window, const Point_t *p)
{
    for(uint8_t i = 0; i < window->BotCount; i++)
    {
        if(bots[i].Position.X == pgm_read_byte(&(p->X))
            && bots[i].Position.Y == pgm_read_byte(&(p->Y)))
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*
 * Initializes a terminal window
 */
void GameWindowInit(GameWindow_t *window)
{
    window->WindowWidth = TERMINAL_DEF_WIDTH;
    window->WindowHeight = TERMINAL_DEF_HEIGHT;
    
    window->MapPos.X = 0;
    window->MapPos.Y = 0;
    
    window->CursorPos.X = (window->WindowWidth >> 1);
    window->CursorPos.Y = (window->WindowHeight >> 1);
    
    window->Gold = DEFAULT_MONEY;
    
    window->BotCount = 0;
    window->Level = 0;
}

/*
 * Gets the type of map tile given x and y
 */
MapTileType_t GameWindowGetTile(GameWindow_t *window, const Point_t *p)
{
    uint16_t tileNum = (p->Y * MAP_WIDTH) + p->X;
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
            if(window->MapPos.Y <
                        (MAP_HEIGHT - window->WindowHeight + (BORDER_WIDTH * 2))
                && window->WindowHeight < MAP_HEIGHT)
            {
                window->MapPos.Y++;
            }

            csCount = 0;
        }
        // Up Arrow
        else if(b == 0x41 && csCount == 2)
        {
            // Move map up
            window->MapPos.Y = (window->MapPos.Y == 0)
                ? 0 : window->MapPos.Y - 1;
            csCount = 0;
        }
        // Left Arrow
        else if(b == 0x44 && csCount == 2)
        {
            window->MapPos.X = (window->MapPos.X == 0)
                ? 0 : window->MapPos.X - 1;
            csCount = 0;
        }
        // Right Arrow
        else if(b == 0x43 && csCount == 2)
        {
            if(window->MapPos.X <
                          (MAP_WIDTH - window->WindowWidth + (BORDER_WIDTH * 2))
                && window->WindowWidth < MAP_WIDTH)
            {
                window->MapPos.X++;
            }

            csCount = 0;
        }
        // Cursor Up (w)
        else if(b == 'w' && csCount == 0)
        {
            window->CursorPos.Y =
                (window->CursorPos.Y == 0) ? 0 : window->CursorPos.Y - 1;
        }
        // Cursor Down (s)
        else if(b == 's' && csCount == 0)
        {
            window->CursorPos.Y = (window->CursorPos.Y == (MAP_HEIGHT - 1))
                ? window->CursorPos.Y : window->CursorPos.Y + 1;
        }
        // Cursor Right (d)
        else if(b == 'd' && csCount == 0)
        {
            window->CursorPos.X = (window->CursorPos.X == (MAP_WIDTH - 1))
                ? window->CursorPos.X : window->CursorPos.X + 1;
        }
        // Cursor Left (a)
        else if(b == 'a' && csCount == 0)
        {
            window->CursorPos.X = (window->CursorPos.X == 0)
                ? 0 : window->CursorPos.X - 1;
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
                window->MapPos.X = 0;
                window->MapPos.Y = 0;
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
        if(!PointsEqual(&prevWindow.MapPos, &window->MapPos))
        {
            GameWindowRenderMap(window);
            GameWindowRenderCursor(window);
        }

        if(prevWindow.Gold != window->Gold)
        {
            GameWindowRenderBorders(window);
        }
        
        if(!PointsEqual(&prevWindow.CursorPos, &window->CursorPos))
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
            && (window->MapPos.Y + y) < MAP_HEIGHT;
        y++)
    {
        GameWindowCursorMove(BORDER_WIDTH + 1, y + BORDER_WIDTH + 1);
        
        for(uint8_t x = 0;
            x < (window->WindowWidth - (BORDER_WIDTH * 2))
                && (window->MapPos.X + x) < MAP_WIDTH;
            x++)
        {
            Point_t p = { .X = window->MapPos.X + x,
                          .Y = window->MapPos.Y + y };

            MapTileType_t tile = GameWindowGetTile(window, &p); 
            GameWindowRenderTile(tile);
        }
    }
    
    GameWindowRenderBase(window);
    GameWindowRenderBots(window);
}

void GameWindowRenderBase(GameWindow_t *window)
{
    int16_t baseX = basePosition.X - window->MapPos.X;
    int16_t baseY = basePosition.Y - window->MapPos.Y;
    
    if(baseX > BORDER_WIDTH && baseX < window->WindowWidth - BORDER_WIDTH
        && baseY > BORDER_WIDTH && baseY < window->WindowHeight - BORDER_WIDTH)
    {
        GameWindowCursorMove(baseX + BORDER_WIDTH + 1,
            baseY + BORDER_WIDTH + 1);
        GameWindowSetBgColor(TermColor_Gold);
        UartTransmitByte(' ');
    }
}

void GameWindowRenderBots(GameWindow_t *window)
{
    for(uint8_t i = 0; i < window->BotCount; i++)
    {
        GameRenderBot(window, &bots[i]);
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
    int16_t cursorX = window->CursorPos.X - window->MapPos.X;
    int16_t cursorY = window->CursorPos.Y - window->MapPos.Y;

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
