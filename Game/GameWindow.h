/*
 * The core game logic of etd.
 * 
 * Keeps track of window size, manages the cursor, sets colors, works on the
 * map and more.
 */

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "Uart.h"
#include "Point.h"
#include "Size.h"

#define TERMINAL_DEF_WIDTH 80
#define TERMINAL_DEF_HEIGHT 24
#define DEFAULT_MONEY 30000

typedef struct LevelSpec_t {
    const char *LevelName;
    uint8_t BotCount;
    uint8_t MoveSpeed;
    uint8_t HealthPoints;
    uint8_t AttackSpeed;
    uint8_t AttackDamage;
    uint8_t KillReward;
} LevelSpec_t;

typedef struct BotInstance_t {
    uint8_t HealthPoints;
    Point_t Position;
} BotInstance_t;

typedef enum MapTileType_t {
    Empty,
    Stone,
    Water,
    Grass
} MapTileType_t;

typedef struct TileWeight_t {
    Point_t Position;
    uint8_t Weight;
} TileWeight_t;

/*
typedef struct GameWindow_t {
    uint8_t WindowWidth;
    uint8_t WindowHeight;
    Point_t MapPos;
    Point_t CursorPos;
    int32_t Gold;
    uint8_t BotCount;
    uint8_t Level;
} GameWindow_t;
*/

typedef enum TermColor_t {
    TermColor_Black = 16,
    TermColor_NearBlack = 235,
    TermColor_White = 15,
    TermColor_Gold = 178,
    TermColor_WaterBg = 27,
    TermColor_WaterFg = 39,
    TermColor_StoneBg = 237,
    TermColor_StoneFg = 241,
    TermColor_GrassBg = 34,
    TermColor_GrassFg = 82,
    TermColor_BotBg = 160,
    TermColor_BotFg = 202 
} TermColor_t;

void GameWindowInit(GameWindow_t *window);

void GameWindowParseInput(GameWindow_t *window);
void GameWindowRenderScreen(GameWindow_t *window);
void GameWindowRenderMap(GameWindow_t *window);
void GameWindowRenderBorders(GameWindow_t *window);
void GameWindowRenderCursor(GameWindow_t *window);
void GameWindowRenderTile(MapTileType_t tile);
void GameWindowRenderTilePosition(MapTileType_t tile, uint8_t x, uint8_t y);

void GameWindowRequestSize();
void GameWindowCursorHome();
void GameWindowCursorHide();
void GameWindowCursorShow();
void GameWindowUseAlternateBuffer();
void GameWindowCursorMove(uint8_t x, uint8_t y);
void GameWindowSetFgColor(TermColor_t color);
void GameWindowSetBgColor(TermColor_t color);
void GameWindowClearScreen();

void GameStep(GameWindow_t *window);
bool GameBotAtLocationP(GameWindow_t *window, const Point_t *p);
void GameNewBotP(GameWindow_t *window, const Point_t *p);
void GameRenderBot(GameWindow_t *window, BotInstance_t *bot);
void GameWindowRenderBase(GameWindow_t *window);
void GameWindowRenderBots(GameWindow_t *window);

uint8_t GameStoreWeight(Point_t *p, uint8_t weight);
void GameClearWeights();
bool GameWeightsFull();
TileWeight_t *GameWeightByPoint(Point_t *p);
TileWeight_t *GameWeightByPointWeight(const Point_t *p, uint8_t weight);

MapTileType_t GameWindowGetTile(GameWindow_t *window, const Point_t *p);

#endif
