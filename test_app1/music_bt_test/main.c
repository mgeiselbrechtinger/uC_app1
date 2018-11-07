#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>

#include	"./libsdcard/sdcard.h"
#include    "./libsdcard/spi.h"
#include    "./music/music.h"
#include	"./adc/adc.h"
#include	"./libmp3/mp3.h"

sdcard_block_t data;
uint32_t addr = 3200000;
uint8_t flag = 1;

void cb(void)
{
	flag = 1;
}

int main (void)
{
	PORTL = 0;
	DDRL = 0xFF;

    spiInit();
	adcInit();
	PORTL |= 1;
	sdcardInit();
    mp3Init(cb);
    mp3SetVolume(0xF0);
	//while(sdcardInit() != SUCCESS){
	//}
	PORTL |= 2;
    sei();
    for(;;){
		// music_bt();
		if(flag == 1){
			sdcardReadBlock(addr, data);
			addr += 32;
			mp3SendMusic(data);
			if(mp3Busy())
				flag = 0;
		}
    }
}
