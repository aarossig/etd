/*
 * Game Implementation
 */

#include "GameWindow.h"
#include "Map.h"

/* Constants ******************************************************************/

const Point_t basePosition PROGMEM = { .X = 51, .Y = 31 };

#define LEVEL_COUNT 1
const LevelSpec_t levels[] PROGMEM = {{ .LevelName = "Beginners Luck",
                                        .MoveSpeed = 20,
                                        .HealthPoints = 10,
                                        .AttackSpeed = 10,
                                        .AttackDamage = 2,
                                        .KillReward = 10 }};

#define ENTRY_POINT_COUNT 3
const Point_t entryPoints[] PROGMEM = {{ .X = 23, .Y = 0 },
                                       { .X = 107, .Y = 0},
                                       { .X = 0, .Y = 15}};

/* Game Variables *************************************************************/

Size_t windowSize = { .Width = TERMINAL_DEF_WIDTH,
                      .Height = TERMINAL_DEF_HEIGHT };

int32_t gold = INITIAL_GOLD;
Point_t viewPosition;
Point_t cursorPosition;
uint8_t level;

uint8_t botCount;
Bot_t bots[MAX_BOTS];

#define VISITED_POINTS_COUNT 60
uint16_t visitedPointsCount = 0;
VisitedPoint_t visitedPoints[VISITED_POINTS_COUNT];


/* Game Functions *************************************************************/

/*
 * Gets the background color for a given tile type
 */
TermColor_t GameGetTileBgColor(const MapTileType_t t)
{
    switch(t)
    {
        case Tile_Stone:
            return TermColor_3A3A3A;
        case Tile_Water:
            return TermColor_005FFF;
        case Tile_Grass:
            return TermColor_00AF00;
        case Tile_Empty:
            return TermColor_000000;
        default:
            return TermColor_262626;
    }
}

/*
 * Gets the foreground color for a given tile type
 */
TermColor_t GameGetTileFgColor(const MapTileType_t t)
{
    switch(t)
    {
        case Tile_Stone:
            return TermColor_606060;
        case Tile_Water:
            return TermColor_00AFFF;
        case Tile_Grass:
            return TermColor_5FFF00;
        case Tile_Empty:
            return TermColor_000000;
        default:
            return TermColor_262626;
    }
}

/*
 * Gets the character to print for a map tile type
 */
char GameGetTileCharacter(const MapTileType_t t)
{
    switch(t)
    {
        case Tile_Stone:
            return '%';
        case Tile_Water:
            return '~';
        case Tile_Grass:
            return '.';
        case Tile_Empty:
            return ' ';
        default:
            return 'X';

    }
}

/*
 * Gets the type of map tile given a point
 */
MapTileType_t GameGetTile(const Point_t *p)
{
    uint16_t tileNum = (p->Y * MAP_WIDTH) + p->X;
    uint8_t tile = pgm_read_byte(&(mapTiles[tileNum >> 2]));
    return (tile >> ((tileNum & 0b11) << 1)) & 0b11;
}

/*
 * Creates a new bot
 */
void GameNewBot(const Point_t *p)
{
    bots[botCount].HealthPoints = pgm_read_byte(&(levels[level]));
    bots[botCount].Position = PointFromP(p);
    GameRenderBot(&bots[botCount++]);
}

/*
 * Returns a bool indicating whether there is a bot at the point provided
 */
bool GameBotAtLocationP(const Point_t *p)
{
    for(uint8_t i = 0; i < botCount; i++)
    {
        if(PointsEqualP(&bots[i].Position, p))
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*
 * Renders the game
 */
void GameRender()
{
    static Size_t prevWindowSize;
    static int32_t prevGold;
    static Point_t prevViewPosition;
    static Point_t prevCursorPosition;

    if(!SizesEqual(&prevWindowSize, &windowSize))
    {
        TerminalSetBgColor(COLOR_BORDER);
        TerminalClearScreen();
        GameRenderMap();
        GameRenderBorders();
        GameRenderCursor();

        prevWindowSize = windowSize;
        prevGold = gold;
        prevViewPosition = viewPosition;
        prevCursorPosition = cursorPosition;
    }
    else
    {
        if(!PointsEqual(&prevViewPosition, &viewPosition))
        {
            GameRenderMap();
            GameRenderCursor();
            prevViewPosition = viewPosition;
        }

        if(prevGold != gold)
        {
            GameRenderBorders();
            prevGold = gold;
        }
        
        if(!PointsEqual(&prevCursorPosition, &cursorPosition))
        {
            GameRenderCursor();
        }
    }
}

/*
 * Renders a map tile
 */
void GameRenderTile(const MapTileType_t tile)
{
    TermColor_t color = GameGetTileBgColor(tile);
    TerminalSetBgColor(color);

    color = GameGetTileFgColor(tile);
    TerminalSetFgColor(color);

    char symbol = GameGetTileCharacter(tile);
    UartTransmitByte(symbol);
}

/*
 * Renders a map tile at a position
 */
void GameRenderTilePosition(const MapTileType_t tile, const Point_t *p)
{
    TerminalCursorMove(p);
    GameRenderTile(tile);
}

/*
 * Renders the map tiles
 */
void GameRenderMap()
{
    TerminalCursorHide();
    
    for(uint8_t y = 0; y < (windowSize.Height - (BORDER_WIDTH * 2))
        && (viewPosition.Y + y) < MAP_HEIGHT; y++)
    {
        Point_t startPos = { .X = BORDER_WIDTH, .Y = y + BORDER_WIDTH };
        TerminalCursorMove(&startPos);
        
        for(uint8_t x = 0; x < (windowSize.Width - (BORDER_WIDTH * 2))
            && (viewPosition.X + x) < MAP_WIDTH; x++)
        {
            Point_t p = { .X = viewPosition.X + x,
                          .Y = viewPosition.Y + y };

            MapTileType_t tile = GameGetTile(&p); 
            GameRenderTile(tile);
        }
    }
    
    GameRenderBase();
    GameRenderBots();
}

/*
 * Renders a bot onto the map
 */
void GameRenderBot(const Bot_t *bot)
{
    int16_t botX = bot->Position.X - viewPosition.X;
    int16_t botY = bot->Position.Y - viewPosition.Y;
    
    if(botX >= 0 && botX < windowSize.Width - (BORDER_WIDTH * 2)
        && botY >= 0 && botY < windowSize.Height - (BORDER_WIDTH * 2))
    {
        Point_t p = { .X = botX + BORDER_WIDTH, .Y = botY + BORDER_WIDTH };
        TerminalCursorMove(&p);
        TerminalSetBgColor(COLOR_BOT_BG);
        TerminalSetFgColor(COLOR_BOT_FG);
        UartTransmitByte(BOT_CHAR);
    }
}

void GameRenderBots()
{
    for(uint8_t i = 0; i < botCount; i++)
    {
        GameRenderBot(&bots[i]);
    }
}

/*
 * Renders the base
 */
void GameRenderBase()
{
    Point_t bPos = PointFromP(&basePosition);
    int16_t baseX = bPos.X - viewPosition.X;
    int16_t baseY = bPos.Y - viewPosition.Y;
    
    if(baseX > BORDER_WIDTH
        && baseX < windowSize.Width - BORDER_WIDTH
        && baseY > BORDER_WIDTH
        && baseY < windowSize.Height - BORDER_WIDTH)
    {
        Point_t p = { .X = baseX + BORDER_WIDTH, .Y = baseY + BORDER_WIDTH };

        TerminalCursorMove(&p);
        TerminalSetBgColor(COLOR_BASE);
        UartTransmitByte(BASE_CHAR);
    }
}

void GameRenderBorders()
{
    Point_t p = { .X = 4, .Y = 0 };
    TerminalCursorMove(&p);
    TerminalSetBgColor(COLOR_BORDER);
    TerminalSetFgColor(TermColor_FFFFFF);

    char buf[17];
    sprintf(buf, "Gold: %ld", gold);
    UartPrint(buf, strlen(buf));
}

void GameRenderCursor()
{
    int16_t cursorX = cursorPosition.X - viewPosition.X;
    int16_t cursorY = cursorPosition.Y - viewPosition.Y;

    if(cursorX < 0)
    {
        cursorX = 0;
    }
    else if(cursorX > (windowSize.Width - BORDER_WIDTH))
    {
        cursorX = windowSize.Width - 1;
    }
    else
    {
        cursorX += BORDER_WIDTH;
    }

    if(cursorY < 0)
    {
        cursorY = 0;
    }
    else if(cursorY > (windowSize.Height - BORDER_WIDTH))
    {
        cursorY = windowSize.Height - 1;
    }
    else
    {
        cursorY += BORDER_WIDTH;
    }
    
    Point_t p = { .X = cursorX, .Y = cursorY };
    TerminalCursorMove(&p);
    TerminalCursorShow();
}

/*
 * Parses user input
 */
void GameParseInput()
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
            if(viewPosition.Y <
                    (MAP_HEIGHT - windowSize.Height + (BORDER_WIDTH * 2))
                && windowSize.Height < MAP_HEIGHT)
            {
                viewPosition.Y++;
            }

            csCount = 0;
        }
        // Up Arrow
        else if(b == 0x41 && csCount == 2)
        {
            // Move map up
            viewPosition.Y = (viewPosition.Y == 0) ? 0 : viewPosition.Y - 1;
            csCount = 0;
        }
        // Left Arrow
        else if(b == 0x44 && csCount == 2)
        {
            viewPosition.X = (viewPosition.X == 0) ? 0 : viewPosition.X - 1;
            csCount = 0;
        }
        // Right Arrow
        else if(b == 0x43 && csCount == 2)
        {
            if(viewPosition.X <
                    (MAP_WIDTH - windowSize.Width + (BORDER_WIDTH * 2))
                && windowSize.Width < MAP_WIDTH)
            {
                viewPosition.X++;
            }

            csCount = 0;
        }
        // Cursor Up (w)
        else if(b == 'w' && csCount == 0)
        {
            cursorPosition.Y =
                (cursorPosition.Y == 0) ? 0 : cursorPosition.Y - 1;
        }
        // Cursor Down (s)
        else if(b == 's' && csCount == 0)
        {
            cursorPosition.Y = (cursorPosition.Y == (MAP_HEIGHT - 1))
                ? cursorPosition.Y : cursorPosition.Y + 1;
        }
        // Cursor Right (d)
        else if(b == 'd' && csCount == 0)
        {
            cursorPosition.X = (cursorPosition.X == (MAP_WIDTH - 1))
                ? cursorPosition.X : cursorPosition.X + 1;
        }
        // Cursor Left (a)
        else if(b == 'a' && csCount == 0)
        {
            cursorPosition.X = (cursorPosition.X == 0)
                ? 0 : cursorPosition.X - 1;
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
            
            if(windowSize.Width != width || windowSize.Height != height)
            {
                viewPosition.X = 0;
                viewPosition.Y = 0;
            }

            windowSize.Width = width;
            windowSize.Height = height;

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

/*
 * Returns a bool indicating whether the number of visited points is full
 */
bool VisitedPointsFull()
{
    return visitedPointsCount >= VISITED_POINTS_COUNT;
}

/*
 * Stores a point and returns a bool indicating that there was enough space
 */
bool VisitedPointStore(const Point_t *p, const uint8_t weight)
{
    if(visitedPointsCount < VISITED_POINTS_COUNT)
    {
        visitedPoints[visitedPointsCount].Position = *p;
        visitedPoints[visitedPointsCount++].Weight = weight;
        return TRUE;
    }

    return FALSE;
}

/*
 * Clears the visited points used for path finding
 */
void VisitedPointsClear()
{
    visitedPointsCount = 0;
}

/*
 * Returns a visited point given a point
 */
VisitedPoint_t *VisitedPointByPoint(const Point_t *p)
{
    for(uint8_t i = 0; i < visitedPointsCount; i++)
    {
        if(PointsEqual(&visitedPoints[i].Position, p))
        {
            return &visitedPoints[i];
        }
    }

    return NULL;
}

/*
 * Returns a visited point adjacent to a tile with a given weight
 */
VisitedPoint_t *VisitedPointByPointWeight(const Point_t *p,
    const uint8_t weight)
{
    VisitedPoint_t *visitedPoint = NULL;
    Point_t pTest = *p;
    PointAddDirection(&pTest, Direction_North);
    
    visitedPoint = VisitedPointByPoint(&pTest);
    if(visitedPoint != NULL && visitedPoint->Weight == weight)
        return visitedPoint;
    
    visitedPoint = NULL;
    pTest = *p;
    PointAddDirection(&pTest, Direction_East);
    
    visitedPoint = VisitedPointByPoint(&pTest);
    if(visitedPoint != NULL && visitedPoint->Weight == weight)
        return visitedPoint;
    
    visitedPoint = NULL;
    pTest = *p;
    PointAddDirection(&pTest, Direction_South);
    
    visitedPoint = VisitedPointByPoint(&pTest);
    if(visitedPoint != NULL && visitedPoint->Weight == weight)
        return visitedPoint;
    
    visitedPoint = NULL;
    pTest = *p;
    PointAddDirection(&pTest, Direction_West);
    
    visitedPoint = VisitedPointByPoint(&pTest);
    if(visitedPoint != NULL && visitedPoint->Weight == weight)
        return visitedPoint;

    return NULL;
}

/*
 * Steps through the game time
 */
void GameStep()
{
    static int16_t time = 0;

    uint8_t moveSpeed = pgm_read_byte(&(levels[level].MoveSpeed));

    if((time % moveSpeed) == 0)
    {
        for(uint8_t i = 0; i < botCount; i++)
        {
            VisitedPointsClear();
            VisitedPointStore(&bots[i].Position, 0);
            
            uint8_t weightResult = 0;
            
            for(uint8_t i = 0; i < visitedPointsCount; i++)
            {
                VisitedPoint_t visitedPoint = visitedPoints[i];
                Point_t p = visitedPoint.Position;
                PointAddDirection(&p, Direction_North);
                
                if(p.Y >= 0 && GameGetTile(&p) != Tile_Stone)
                {
                    if(!VisitedPointByPoint(&p))
                        if(!VisitedPointStore(&p, visitedPoint.Weight + 1))
                            break;
                    
                    if(PointsEqualP(&p, &basePosition))
                    {
                        weightResult = visitedPoint.Weight + 1;
                        break;
                    }
                }
                
                p = visitedPoint.Position;
                PointAddDirection(&p, Direction_East);
                
                if(p.X < MAP_WIDTH && GameGetTile(&p) != Tile_Stone)
                {
                    if(!VisitedPointByPoint(&p))
                        if(!VisitedPointStore(&p, visitedPoint.Weight + 1))
                            break;

                    if(PointsEqualP(&p, &basePosition))
                    {
                        weightResult = visitedPoint.Weight + 1;
                        break;
                    }
                }
                
                p = visitedPoint.Position;
                PointAddDirection(&p, Direction_South);
                
                if(p.Y < MAP_HEIGHT && GameGetTile(&p) != Tile_Stone)
                {
                    if(!VisitedPointByPoint(&p))
                        if(!VisitedPointStore(&p, visitedPoint.Weight + 1))
                            break;

                    if(PointsEqualP(&p, &basePosition))
                    {
                        weightResult = visitedPoint.Weight + 1;
                        break;
                    }
                }
                
                p = visitedPoint.Position;
                PointAddDirection(&p, Direction_West);
                
                if(p.X >= 0 && GameGetTile(&p) != Tile_Stone)
                {
                    if(!VisitedPointByPoint(&p))
                        if(!VisitedPointStore(&p, visitedPoint.Weight + 1))
                            break;

                    if(PointsEqualP(&p, &basePosition))
                    {
                        weightResult = visitedPoint.Weight + 1;
                        break;
                    }
                }
            }
            
            VisitedPoint_t *visitedPoint = NULL;
            Point_t workingPoint = PointFromP(&basePosition);
            
            while(--weightResult > 0)
            {
                visitedPoint =
                    VisitedPointByPointWeight(&workingPoint, weightResult);

                if(visitedPoint != NULL)
                    workingPoint = visitedPoint->Position;
                else
                    break;
            }

            if(visitedPoint != NULL)
            {
                bots[i].Position = visitedPoint->Position;
            }
            else
            {
                // Move bots closer to the base along the longer axis
            }
        }
        
        // Place bots if required
        if(botCount < MAX_BOTS)
        {
            for(uint8_t i = 0; i < ENTRY_POINT_COUNT; i++)
            {
                if(!GameBotAtLocationP(&entryPoints[i]))
                {
                    GameNewBot(&entryPoints[i]);
                }
            }
        }
    }
}

