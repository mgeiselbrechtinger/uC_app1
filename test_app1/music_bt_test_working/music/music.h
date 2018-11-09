#ifndef MUSIC_H
#define MUSIC_H

extern uint8_t music_vol_flag; 

void music_init(void);
void music_bt(void);
void music_mp3_callback(void);
void music_set_volume(uint8_t linear_data);

#endif 
