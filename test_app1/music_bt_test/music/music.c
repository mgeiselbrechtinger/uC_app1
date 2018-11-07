#include    <avr/io.h>
#include    <stdint.h>

#include    "../libsdcard/sdcard.h"
#include    "../libmp3/mp3.h"
#include    "../adc/adc.h"

#define MUSIC_START_ADDR    (5052096)
#define MUSIC_END_ADDR      (MUSIC_START_ADDR + 132336)

uint8_t mp3_req_flag = 1;
static sdcard_block_t music_buffer;
static uint32_t sd_read_address = MUSIC_START_ADDR;
static uint8_t mp3_vol = 0x0f;

void mp3_callback(void)
{
    mp3_req_flag = 1;
}

void music_bt(void) 
{
    error_t status;
    while(mp3_req_flag == 1){

        status = sdcardReadBlock(sd_read_address, music_buffer);
        //if(status == SUCCESS){

            sd_read_address += 32; //(sd_read_address + 32) & (MUSIC_END_ADDR - 1);
				if(sd_read_address > MUSIC_END_ADDR)
					sd_read_address = MUSIC_START_ADDR;

            mp3SendMusic(music_buffer);        
        //}

        // if(mp3_vol != adc_vol){
        //     mp3_vol = adc_vol;
        //     mp3SetVolume(mp3_vol);
        // }
		if(mp3Busy())
			mp3_req_flag = 0;
    }
}

