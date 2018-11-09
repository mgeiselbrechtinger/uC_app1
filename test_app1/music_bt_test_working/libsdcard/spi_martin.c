#include "spi.h"
#include <avr/io.h>

void spiInit(){
    //Set SCK(PB1), MOSI(PB2) MISO(PB3) to output
    DDRB |= (1<<DDB1)|(1<<DDB2);
    //Set MISO (PB3) to input
    DDRB &= ~(1<<DDB3);
    //Initialize SPI as Master
    SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
    //Use SPI in Mode 0 (CPOL=0, CPHA=0)
    SPCR &= ~((1<<CPOL)|(1<<CPHA));
}


void spiSend(uint8_t data){
    
    SPDR = data;
    //wait for transmission complete
    while(!(SPSR & (1<<SPIF)));

}

uint8_t spiReceive(void){

    //load a dummy value into the SPDR register
    SPDR = 0xff;
    //wait for transmission complete
    while(!(SPSR & (1<<SPIF)));
    //return the received transmission
    return SPDR;

}

void spiSetPrescaler(spi_prescaler_t prescaler){
    
    if(prescaler == SPI_PRESCALER_128){
        SPCR |= (1<<SPR0)|(1<<SPR1);
        SPSR &= ~(1<<SPI2X);
    }
    else if(prescaler == SPI_PRESCALER_4){
        SPCR &= ~((1<<SPR0)|(1<<SPR1));
        SPSR &= ~(1<<SPI2X);
    }

}


