#include    <avr/io.h>
#include    <stdint.h>

#include    "./spi.h"

void spiInit(void)
{
    // clear pull-up's
    PORTB &= (1 << PB6) | (1 << PB7);
    // set MOSI, SCK and MP3_CS output
    DDRB  |= (1 << PB0) | (1 << PB1) | (1 << PB2);
    // set DREQ as input
    PORTD &= ~(1 << PD0);
    DDRD  &= ~(1 << PD0);
    // set MMC_CS as output
    PORTG &= ~(1 << PG1);
    DDRG  |=  (1 << PG1);
    // enable SPI, mode 0, MSB first
    SPCR = (1 << SPE) | (1 << MSTR);
}

void spiSend(uint8_t data)
{
    SPDR = data;
    // busy wait (allowed)
    while(!(SPSR & (1 << SPIF)))
        ;
}

uint8_t spiReceive(void)
{
    // send dummypattern to SD while receiving
    SPDR = 0xFF;
    // busy wait (allowed)
    while(!(SPSR & (1 << SPIF)))
        ;
    return SPDR;
}

void spiSetPrescaler(spi_prescaler_t prescaler)
{
    SPSR &= ~(1 << SPI2X);

    if(prescaler = SPI_PRESCALER_128)
        SPCR |= (1 << SPR0) | (1 << SPR1);
    else // SPI_PRESCALER_4
        SPCR &= ~((1 << SPR0) | (1 << SPR1));
}
