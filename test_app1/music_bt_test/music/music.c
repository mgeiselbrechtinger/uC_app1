#include    <avr/io.h>
#include    <stdint.h>

#include    "../libsdcard/sdcard.h"
#include	"../libsdcard/spi.h"
#include    "../libmp3/mp3.h"
#include    "../adc/adc.h"

#define GAGA_START	(7270240)
#define GAGA_END	(GAGA_START + 860256)
#define MUSIC_START (5052096)
#define MUSIC_END   (MUSIC_START + 132336)

static bool mp3_req_flag;
static sdcard_block_t music_buffer;
static uint32_t sd_read_address;
static uint8_t mp3_vol;

void mp3_callback(void)
{
    mp3_req_flag = false;
}

void music_init(void)
{
	PORTL = 0;
	DDRL = 0xff;

	spiInit();
	PORTL = 0x01;
	while(sdcardInit() != SUCCESS){
	}

	PORTL = 0x02;
	sd_read_address = MUSIC_START;

	mp3Init(&mp3_callback);
	
	mp3_vol = 0xff;
	mp3SetVolume(mp3_vol);
}

void music_bt(void) 
{
    error_t status;
	
    while((mp3_req_flag = mp3Busy()) == false){

        status = sdcardReadBlock(sd_read_address, music_buffer);
        if(status == SUCCESS){

            sd_read_address += 32; 
				if(sd_read_address > MUSIC_END)
					sd_read_address = MUSIC_START;

            mp3SendMusic(music_buffer);        
        }

        // if(mp3_vol != adc_vol){
        //     mp3_vol = adc_vol;
        //     mp3SetVolume(mp3_vol);
        // }
    }
}

