/*
 * Global Interrupts Management
 * 
 * Allows interrupts to be enabled and disabled in a safe way.
 */

#include <avr/interrupt.h>

#include "Bool.h"

#define FLAG_I 7

bool DisableInterrupts();
void EnableInterrupts(bool enabled);
