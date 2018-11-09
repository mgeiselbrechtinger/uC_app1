#include    <avr/io.h>
#include    <stdint.h>

#include    "../libsdcard/sdcard.h"
#include	"../libsdcard/spi.h"
#include    "../libmp3/mp3.h"

#define GAGA_START	(7270240)
#define GAGA_END	(GAGA_START + 860256)
#define MUSIC_START (5052096)
#define MUSIC_END   (MUSIC_START + 132336)

uint8_t music_vol_flag;

static bool mp3_req_flag;
static sdcard_block_t music_buffer;
static uint32_t sd_read_address;

void music_set_volume(uint8_t linear_data)
{
	uint8_t log_data;

	/* approximate log with 1 - (1 - x)^4 for x in [0,1] */
	log_data = 0xff - linear_data;
	log_data = (log_data * log_data) >> 8;
	log_data = (log_data * log_data) >> 8;
	log_data = 0xff - log_data;
	
	mp3SetVolume(log_data);
}

void music_mp3_callback(void)
{
    mp3_req_flag = false;
}

void music_init(void)
{
	spiInit();
	while(sdcardInit() != SUCCESS){
	}

	sd_read_address = MUSIC_START;

	mp3Init(&music_mp3_callback);
	
	music_vol_flag = 1;
}

void music_bt(void) 
{
    error_t status;
	
    while(mp3_req_flag == false){
		
		music_vol_flag = 0;

        status = sdcardReadBlock(sd_read_address, music_buffer);
        if(status == SUCCESS){

            sd_read_address += 32; 
				if(sd_read_address > MUSIC_END)
					sd_read_address = MUSIC_START;

            mp3SendMusic(music_buffer);        
        }
	
		mp3_req_flag = mp3Busy();
		music_vol_flag = 1;

    }

}

