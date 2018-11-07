#include    <avr/io.h>
#include    <stdint.h>

#include    "../libsdcard/sdcard.h"
#include    "../libmp3/mp3.h"
#include    "../adc/adc.h"

#define MUSIC_START_ADDR    (0x00000000)
#define MUSIC_END_ADDR      (0xFFFFFFFF)

static uint8_t mp3_req_flag = 1;
static sdcard_block_t music_buffer;
static uint32_t sd_read_address = MUSIC_START_ADDR;
static uint8_t mp3_vol = 0;

void mp3_callback_fn(void)
{
    mp3_req_flag = 1;
}

void music_bt(void) 
{
    error_t = status;
    
    while(mp3_req_flag != 0){

        status = sdcardReadBlock(sd_read_address, music_buffer);
        if(status == SUCCESS){

            sd_read_address = (sd_read_addres + 32) & (MUSIC_END_ADDR - 1);
            mp3SendMusic(music_buffer);        
        }

        if(mp3_vol != adc_vol){
            mp3_vol = adc_vol;
            mp3SetVolume(mp3_vol);
        }

        mp3_req_flag = mp3Busy(); 
    }
}

