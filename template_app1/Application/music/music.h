#ifndef MUSIC_H
#define MUSIC_H

extern uint8_t music_vol_flag; 

/**
 * Initializes music module
 *
 * @globals: music_vol_flag, sd_read_address
 */
void music_init(void);

/**
 * Feeds data from sdcard to mp3 module
 *
 * @globals: mp3_req_flag, sd_read_address, music_vol_flag
 */
void music_bt(void);

/**
 * Callback function for music request of mp3 module
 *
 * @globals: mp3_req_flag
 */
void music_mp3_callback(void);

/**
 * Scales and forwardeds volume to mp3 module
 *
 * @param: linear_data, poti state from adc module
 */
void music_set_volume(uint8_t linear_data);

#endif 
