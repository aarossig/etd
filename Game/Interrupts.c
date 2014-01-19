/*
 * Global interrupts management implementation
 */

#include "Interrupts.h"

/*
 * Returns the status of global interrupts and then disables them
 */
inline bool DisableInterrupts()
{
    bool enabled = (SREG & (1 << FLAG_I)) != 0;
    cli();
    return enabled;
}

/*
 * Enables interrupts conditionally
 */
inline void EnableInterrupts(bool enabled)
{
    if(enabled)
    {
        sei();
    }
}
