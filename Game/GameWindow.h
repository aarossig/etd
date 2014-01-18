/*
 * The core game logic of etd.
 * 
 * Keeps track of window size, manages the cursor, sets colors, works on the
 * map and more.
 */

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <avr/pgmspace.h>

#include "Uart.h"

#define TERMINAL_DEF_WIDTH 80
#define TERMINAL_DEF_HEIGHT 24

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
} GameWindow_t;

void GameWindowInit(GameWindow_t *window);

void GameWindowParseInput(GameWindow_t *window);
void GameWindowRenderScreen(GameWindow_t *window);

void GameWindowRequestSize();
void GameWindowCursorHome();
void GameWindowCursorHide();

MapTileType_t GameWindowGetTile(GameWindow_t *window, uint16_t x, uint16_t y);

#endif
