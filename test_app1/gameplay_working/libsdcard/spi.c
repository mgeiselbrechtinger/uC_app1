#include    <avr/io.h>
#include    <stdint.h>

#include    "./spi.h"

/**
 * Initializes hardware for SPI
 */
void spiInit(void)
{
    /* port initialization */
    PORTB &= ~((1 << PB3) | (1 << PB2) | (1 << PB1));
    /* set MOSI and SCK as output */
    DDRB  |= (1 << PB2) | (1 << PB1);
    /* set MISO as input */
    DDRB &= ~(1 << PB3);
    /* dissable double speed */
    SPSR &= ~(1<< SPI2X);
    /* enable SPI, mode 0, MSB first */
    SPCR = (1 << SPE) | (1 << MSTR);
}

/**
 * Sends data over SPI
 *
 * @param: data, byte that has to be send
 */
void spiSend(uint8_t data)
{
    SPDR = data;
    /* busy wait till sent, (allowed) */
    while(!(SPSR & (1 << SPIF)))
        ;
}

/**
 * Receives data over SPI
 *
 * @return: byte that has been received
 */
uint8_t spiReceive(void)
{
    /* send dummy pattern to sdcard while receiving */
    SPDR = 0xFF;
    /* busy wait till received, (allowed) */
    while(!(SPSR & (1 << SPIF)))
        ;
    return SPDR;
}

/**
 * Sets prescaler of SPI
 */
void spiSetPrescaler(spi_prescaler_t prescaler)
{
    SPCR &= ~((1 << SPR0) | (1 << SPR1));
    SPCR |= prescaler;
}
