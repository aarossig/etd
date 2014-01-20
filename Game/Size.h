/*
 * A size struct to track width and height
 */

#ifndef SIZE_H
#define SIZE_H

#include <stdint.h>

#include "Bool.h"

typedef struct Size_t {
    uint8_t Width;
    uint8_t Height;
} Size_t;

bool SizesEqual(const Size_t *s1, const Size_t *s2);

#endif
