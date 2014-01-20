/*
 * Terminal manipulation functions and defines
 */

#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <string.h>

#include "Uart.h"
#include "Point.h"

#define TERMINAL_DEF_WIDTH 80
#define TERMINAL_DEF_HEIGHT 24

/*
 * Some terminal colors
 */
typedef enum TermColor_t {
    TermColor_FFFFFF = 15,
    TermColor_000000 = 16,
    TermColor_005FFF = 27,
    TermColor_00AF00 = 34,
    TermColor_00AFFF = 39,
    TermColor_5FFF00 = 82,
    TermColor_D70000 = 160,
    TermColor_D7AF00 = 178,
    TermColor_FF5F00 = 202,
    TermColor_262626 = 235,
    TermColor_3A3A3A = 237,
    TermColor_606060 = 241,
} TermColor_t;

/*
 * Terminal Functions
 */
void TerminalRequestSize();

void TerminalCursorHome();
void TerminalCursorMove(const Point_t *p);
void TerminalCursorHide();
void TerminalCursorShow();

void TerminalUseAlternateBuffer();
void TerminalClearScreen();

void TerminalSetFgColor(TermColor_t color);
void TerminalSetBgColor(TermColor_t color);


#endif
