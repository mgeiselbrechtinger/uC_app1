#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>

#include	"./libsdcard/sdcard.h"
#include    "./libsdcard/spi.h"
#include    "./music/music.h"
#include	"./adc/adc.h"
#include	"./libmp3/mp3.h"

int main (void)
{
	adcInit();
	music_init();
	sei();
    for(;;){
		music_bt();
    }
}
