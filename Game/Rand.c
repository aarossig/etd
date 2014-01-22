/*
 * Random number generator implementation
 */

#include "Rand.h"

/*
 * Configures the ADC to produce random numbers
 */
void RandInit()
{
    ADMUX = (1 << REFS1) | (1 << REFS0) | (1 << ADLAR);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
}

bool RandGetBit()
{
    bool firstRead = FALSE;
    bool secondRead = FALSE;
    
    while(firstRead == secondRead)
    {
        firstRead = RandGetUnfilteredBit();
        secondRead = RandGetUnfilteredBit();
    }

    return firstRead;
}

/*
 * Makes a conversion and returns the least significant digit of the result
 */
bool RandGetUnfilteredBit()
{
    // Start a conversion
    ADCSRA |= (1 << ADSC);
    while(ADCSRA & (1 << ADSC));
    return (ADCH & 0x01);
}

/*
 * Returns a random byte
 */
uint8_t RandGetByte()
{
    uint8_t value = 0;

    for(uint8_t i = 0; i < 8; i++)
    {
        value |= (RandGetBit() << i);
    }

    return value;
}
