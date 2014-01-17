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
}

/* UART Transmit **************************************************************/

/*
 * Sends a byte down the UART
 */
void UartTransmitByte(uint8_t b)
{
    if(CircularBufferIsEmpty(&txBuf))
    {
        CircularBufferWrite(&txBuf, b);
        UDR0 = CircularBufferRead(&txBuf);
    }
    else
    {
        while(CircularBufferIsFull(&txBuf));
        CircularBufferWrite(&txBuf, b);
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
}

/* Uart Receive ***************************************************************/

/*
 * Gets an element from the UART receive buffer
 */
uint8_t UartReceiveByte(void)
{
    while(CircularBufferIsEmpty(&rxBuf));
    return CircularBufferRead(&rxBuf);
}

/*
 * UART receive complete interrupt handler
 */
void __attribute__((signal)) USART_RX_vect(void)
{
    CircularBufferWrite(&rxBuf, UDR0);
}

