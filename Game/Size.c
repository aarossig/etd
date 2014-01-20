/*
 * Size implementation
 */

#include "Size.h"

bool SizesEqual(const Size_t *s1, const Size_t *s2)
{
    return (s1->Width == s2->Width && s1->Height == s2->Height);
}
