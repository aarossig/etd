/*
 * UART Driver Implementation
 */

#include "Uart.h"

#define BAUD_REG ((F_CPU / (8 * BAUD)) - 1)
#define BUF_SIZE 128

/* Tx and Rx Buffers **********************************************************/

volatile CircularBuffer_t rxBuf;
uint8_t rxData[BUF_SIZE];
volatile CircularBuffer_t txBuf;
uint8_t txData[BUF_SIZE];

/* UART Hardware Initialization ***********************************************/

/*
 * Iniitalizes the UART hardware
 */
void UartInit(void)
{
    UCSR0A = (1 << U2X0);
    UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0);
    UBRR0H = (BAUD_REG >> 8);
    UBRR0L = (BAUD_REG);
    
    rxBuf.Size = BUF_SIZE;
    rxBuf.Buf = rxData;
    txBuf.Size = BUF_SIZE;
    txBuf.Buf = txData;
    
    EnableInterrupts(TRUE);
}

/* Helper Functions ***********************************************************/

/*
 * Prints a string out to the UART
 */
void UartPrint(const char *str, uint8_t len)
{
    for(uint8_t i = 0; i < len; i++)
    {
        UartTransmitByte(str[i]);
    }
}

/*
 * Prints a string from PROGMEM out to the UART
 */
void UartPrintP(const char *str, uint8_t len)
{
    for(uint8_t i = 0; i < len; i++)
    {
        UartTransmitByte(pgm_read_byte(&(str[i])));
    }
}

/* UART Transmit *************************************************************/

bool transmitting;

/*
 * Sends a byte down the UART
 */
void UartTransmitByte(uint8_t b)
{
    bool interruptsState = DisableInterrupts();
    
    if(!transmitting)
    {
        transmitting = TRUE;
        UDR0 = b;
        EnableInterrupts(interruptsState);
    }
    else
    {
        uint8_t isFull = CircularBufferIsFull(&txBuf);
        
        EnableInterrupts(interruptsState);
        
        while(isFull)
        {
            interruptsState = DisableInterrupts();
            isFull = CircularBufferIsFull(&txBuf);
            EnableInterrupts(interruptsState);
        }
        
        interruptsState = DisableInterrupts();
        CircularBufferWrite(&txBuf, b);
        EnableInterrupts(interruptsState);
    }
}

/*
 * UART transmit complete interrupt handler
 */
void __attribute__((signal)) USART_TX_vect(void)
{
    if(!CircularBufferIsEmpty(&txBuf))
    {
        UDR0 = CircularBufferRead(&txBuf);
    }
    else
    {
        transmitting = FALSE;
    }
}

/* Uart Receive ***************************************************************/

/*
 * Gets an element from the UART receive buffer
 */
uint8_t UartReceiveByte(void)
{
    bool interruptsState = DisableInterrupts();
    bool isEmpty = CircularBufferIsEmpty(&rxBuf);
    
    EnableInterrupts(interruptsState);
    while(isEmpty)
    {
        interruptsState = DisableInterrupts();
        isEmpty = CircularBufferIsEmpty(&rxBuf);
        EnableInterrupts(interruptsState);
    }
    
    interruptsState = DisableInterrupts();
    uint8_t b = CircularBufferRead(&rxBuf);
    EnableInterrupts(interruptsState);
    
    return b;
}

/*
 * Returns how many bytes are available to be read from the buffer.
 */
uint8_t UartBytesToReceive()
{
    bool interruptsState = DisableInterrupts();
    uint8_t bytes = rxBuf.Count;
    EnableInterrupts(interruptsState);
    
    return bytes;
}

/*
 * UART receive complete interrupt handler
 */
void __attribute__((signal)) USART_RX_vect(void)
{
    CircularBufferWrite(&rxBuf, UDR0);
}

