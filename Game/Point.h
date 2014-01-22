/*
 * Point structure
 *
 * An 8-bit tuple of X and Y coordinates.
 */

#ifndef POINT_H
#define POINT_H

#include <stdint.h>
#include <avr/pgmspace.h>

#include "Bool.h"
#include "Direction.h"
#include "Size.h"

typedef struct Point_t {
    uint8_t X;
    uint8_t Y;
} Point_t;

bool PointsEqual(const Point_t p1, const Point_t p2);
Point_t PointAdd(const Point_t p1, const Point_t p2);
bool PointAddDirection(Point_t *p, const Direction_t d);
bool PointInSize(const Point_t *p, const Size_t *s);
uint8_t PointShortestAxis(const Point_t p1, const Point_t p2);

#endif
