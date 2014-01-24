/*
 * Point supporting functions implementation
 */

#include "Point.h"

/*
 * Returns a bool indicating whether the supplied points are equal
 */
bool PointsEqual(const Point_t p1, const Point_t p2)
{
    return (p1.X == p2.X && p1.Y == p2.Y);
}

/*
 * Adds twp points together
 */
Point_t PointAdd(const Point_t p1, const Point_t p2)
{
    Point_t p = { .X = p1.X + p2.X, .Y = p1.Y + p2.Y };
    return p;
}

/*
 * Adds a direction to a point
 */
bool PointAddDirection(Point_t *p, const Direction_t d)
{
    switch(d)
    {
        case Direction_North:
            if(p->Y == 0)
                return FALSE;
            
            p->Y--;
            break;
        case Direction_East:
            if(p->X == 255)
                return FALSE;
            
            p->X++;
            break;
        case Direction_South:
            if(p->Y == 255)
                return FALSE;
            
            p->Y++;
            break;
        case Direction_West:
            if(p->X == 0)
                return FALSE;
            
            p->X--;
            break;
    }

    return TRUE;
}

bool PointInSize(const Point_t *p, const Size_t *s)
{
    return (p->X < s->Width && p->Y < s->Height);
}

/*
 * Returns the shortest axis difference between these two points
 */
uint8_t PointLongestAxis(const Point_t p1, const Point_t p2)
{
    int16_t x = p1.X - p2.X;
    int16_t y = p1.Y - p2.Y;

    uint8_t absX = x < 0 ? -x : x;
    uint8_t absY = y < 0 ? -y : y;

    if(absX > absY)
    {
        return absX;
    }
    else
    {
        return absY;
    }
}

uint8_t PointDistance(const Point_t p1, const Point_t p2)
{
    int16_t x = p1.X - p2.X;
    int16_t y = p1.Y - p2.Y;

    uint8_t absX = x < 0 ? -x : x;
    uint8_t absY = y < 0 ? -y : y;
    
    return absX + absY;
}

