#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>

#include    "./libsdcard/spi.h"
#include    "./libmp3/mp3.h"

int main (void)
{
    void (*cbf)(void) = 0;
    spiInit();
    mp3Init(cbf);
    sei();
	mp3StartSineTest();
    for(;;){
    }
}
