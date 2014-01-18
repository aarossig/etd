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

typedef enum TermColor_t {
    TermColor_Black,
    TermColor_Red,
    TermColor_Green,
    TermColor_Yellow,
    TermColor_Blue,
    TermColor_Magenta,
    TermColor_Cyan,
    TermColor_White
} TermColor_t;

void GameWindowInit(GameWindow_t *window);

void GameWindowParseInput(GameWindow_t *window);
void GameWindowRenderScreen(GameWindow_t *window);

void GameWindowRequestSize();
void GameWindowCursorHome();
void GameWindowCursorHide();
void GameWindowUseAlternateBuffer();
void GameWindowCursorMove(uint16_t x, uint16_t y);
void GameWindowSetColor(TermColor_t bgColor, TermColor_t fgColor);

MapTileType_t GameWindowGetTile(GameWindow_t *window, uint16_t x, uint16_t y);

#endif
