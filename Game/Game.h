/*
 * The core game logic of etd.
 * 
 * Keeps track of window size, manages the cursor, sets colors, works on the
 * map and more.
 */

#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "Uart.h"
#include "Point.h"
#include "Size.h"
#include "Terminal.h"
#include "Rand.h"

#define MAP_WIDTH 121
#define MAP_HEIGHT 48

#define BORDER_WIDTH 1
#define BORDER_PAD 4

#define COLOR_BASE TermColor_D7AF00
#define COLOR_BORDER TermColor_262626
#define COLOR_BOT_BG TermColor_D70000
#define COLOR_BOT_FG TermColor_FF5F00
#define COLOR_TOWER_BG TermColor_AF00FF
#define COLOR_TOWER_FG TermColor_FFFFFF

#define INITIAL_GOLD 30000

#define TOWER_BUILD_COST 1500

#define BOT_ATTACK_DISTANCE 6

#define MAX_BOTS 32
#define MAX_TOWERS 128

#define MAX_FLOOD_ATTEMPTS 5
#define BOT_CHAR '%'
#define BASE_CHAR ' '

#define PARAM_BUF_LEN 5
#define PARAM_COUNT 2

/* Map Tiles ******************************************************************/

typedef enum TileType_t {
    Tile_Empty,
    Tile_Stone,
    Tile_Water,
    Tile_Grass
} TileType_t;

TermColor_t GameGetTileBgColor(const TileType_t t);
TermColor_t GameGetTileFgColor(const TileType_t t);
char GameGetTileCharacter(const TileType_t t);
TileType_t GameGetTile(const Point_t p);

/* Towers *********************************************************************/

typedef struct Tower_t {
    Point_t Position;
    uint8_t Level;
} Tower_t;

Tower_t *GameTowerByPoint(const Point_t p);
uint8_t GameTowerAttackDamage(Tower_t *tower);
void GameNewTower();

/* Bots ***********************************************************************/

typedef struct Bot_t {
    uint8_t HealthPoints;
    Point_t Position;
    uint8_t FloodAttempts;
} Bot_t;

void GameNewBot(const Point_t p);
void GameRandomizeBot(Bot_t *bot);
Bot_t *GameBotByPoint(const Point_t p);
void GameAttackBot(const uint8_t botIndex, const uint8_t damage);

/* Rendering and UI ***********************************************************/

void GameRender();
void GameRenderTile(const TileType_t tile);
void GameRenderTilePosition(const Point_t p);
void GameRenderMap();
void GameRenderBot(const Bot_t *bot);
void GameRenderTowers();
void GameRenderTower(const Tower_t *tower);
void GameRenderBots();
void GameRenderBase();
void GameRenderBorders();
void GameRenderCursor();
void GameClearStatus();
void GameRenderStatusP(const char *status);
void GameRenderStatus(const char *status);

void GameParseInput();

/* Levels *********************************************************************/

typedef struct LevelSpec_t {
    const char *LevelName;
    uint8_t MoveSpeed;
    uint8_t HealthPoints;
    uint8_t AttackSpeed;
    uint8_t AttackDamage;
    uint8_t KillReward;
} LevelSpec_t;

/* Path Finding ***************************************************************/

typedef struct VisitedPoint_t {
    Point_t Position;
    uint8_t Weight;
} VisitedPoint_t;

VisitedPoint_t *VisitedPointStore(const Point_t *p, const uint8_t weight);
void VisitedPointsClear();

VisitedPoint_t *VisitedPointByPoint(const Point_t *p);
VisitedPoint_t *VisitedPointByPointWeight(const Point_t *p,
    const uint8_t weight);

/* Time Stepping **************************************************************/

void GameStep();
uint16_t GameAbs(int16_t v);
bool GameSimpleMove(Bot_t *bot);
bool GameComplexMove(Bot_t *bot);
bool GameMoveBot(Bot_t *bot, Direction_t direction);

#endif
