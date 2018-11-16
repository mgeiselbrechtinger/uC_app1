#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>

#include    "./hal/hal_glcd.h"

// TODO do not forget to enable interrupts when further testing
int main(void)
{
    // debug PORTs
    PORTK = 0x00;
    DDRK  = 0xff;
    PORTL = 0x00;
    DDRL  = 0xff;
	for(uint8_t i = 0; i < 5; i++)
    	halGlcdInit();
	
    halGlcdSetAddress(0x00, 0x00);
    halGlcdWriteData(0xff);
    halGlcdWriteData(0x55);
	/*
    uint8_t data1, data2;
    halGlcdSetAddress(0x3f, 0x01);
    data1 = halGlcdReadData();
    data2 = halGlcdReadData();

    PORTK = data1;
    PORTL = data2;
	*/
    for(;;){

    }
}
