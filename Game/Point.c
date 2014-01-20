/*
 * Point supporting functions implementation
 */

#include "Point.h"

/*
 * Returns a bool indicating whether the supplied points are equal
 */
bool PointsEqual(const Point_t *p1, const Point_t *p2)
{
    return (p1->X == p2->X && p1->Y == p2->Y);
}

/*
 * Returns a bool indicating whether the supplied points are equal.
 * 
 * The second point resides in program memory
 */
bool PointsEqualP(const Point_t *p1, const Point_t *p2)
{
    return (p1->X == pgm_read_byte(&(p2->X))
        && p1->Y == pgm_read_byte(&(p2->Y)));
}

/*
 * Adds twp points together
 */
Point_t PointAdd(const Point_t *p1, const Point_t *p2)
{
    Point_t p = { .X = p1->X + p2->X, .Y = p1->Y + p2->Y };
    return p;
}

/*
 * Adds a direction to a point
 */
void PointAddDirection(Point_t *p, const Direction_t d)
{
    switch(d)
    {
        case Direction_North:
            p->Y--;
            break;
        case Direction_East:
            p->X++;
            break;
        case Direction_South:
            p->Y++;
            break;
        case Direction_West:
            p->X--;
            break;
    }
}

bool PointInSize(const Point_t *p, const Size_t *s)
{
    return (p->X < s->Width && p->Y < s->Height);
}

Point_t PointFromP(const Point_t *p)
{
    Point_t newP = { .X = pgm_read_byte(&p->X), .Y = pgm_read_byte(&p->Y) };
    return newP;
}

