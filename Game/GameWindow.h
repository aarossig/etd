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

#define TERMINAL_DEF_WIDTH 80
#define TERMINAL_DEF_HEIGHT 24
#define DEFAULT_MONEY 30000

typedef enum MapTileType_t {
    Empty,
    Stone,
    Water,
    Grass
} MapTileType_t;

typedef struct MapTile_t {
    uint8_t Length;
    MapTileType_t TileType;
} MapTile_t;

typedef struct GameWindow_t {
    uint16_t WindowWidth;
    uint16_t WindowHeight;
    uint16_t MapX;
    uint16_t MapY;
    uint16_t CursorX;
    uint16_t CursorY;
    int32_t Gold;
} GameWindow_t;

typedef enum TermColor_t {
    TermColor_Black = 16,
    TermColor_NearBlack = 235,
    TermColor_White = 15,
    TermColor_WaterBg = 27,
    TermColor_WaterFg = 39,
    TermColor_StoneBg = 237,
    TermColor_StoneFg = 241,
    TermColor_GrassBg = 34,
    TermColor_GrassFg = 82
} TermColor_t;

void GameWindowInit(GameWindow_t *window);

void GameWindowParseInput(GameWindow_t *window);
void GameWindowRenderScreen(GameWindow_t *window);
void GameWindowRenderMap(GameWindow_t *window);
void GameWindowRenderBorders(GameWindow_t *window);
void GameWindowRenderCursor(GameWindow_t *window);
void GameWindowRenderTile(MapTileType_t tile);
void GameWindowRenderTilePosition(MapTileType_t tile, uint16_t x, uint16_t y);

void GameWindowRequestSize();
void GameWindowCursorHome();
void GameWindowCursorHide();
void GameWindowCursorShow();
void GameWindowUseAlternateBuffer();
void GameWindowCursorMove(uint16_t x, uint16_t y);
void GameWindowSetFgColor(TermColor_t color);
void GameWindowSetBgColor(TermColor_t color);
void GameWindowClearScreen();

MapTileType_t GameWindowGetTile(GameWindow_t *window, uint16_t x, uint16_t y);

#endif
