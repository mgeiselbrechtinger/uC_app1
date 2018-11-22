#include    <avr/io.h>
#include    <avr/sleep.h>
#include    <avr/interrupt.h>
#include    <stdint.h>

#include    "./music/music.h"
#include    "./adc/adc.h"
#include    "./menu/menu.h"
#include    "./glcd/glcd_user/glcd.h"


uint8_t game_timer;

/**
 * Main function
 *
 * @brief: Initializes required modules and starts game
 *         plays music as background task
 */
int main(void)
{
    /* initializations */
    adcInit();
    music_init();
    glcdInit();
    menu_init();

    set_sleep_mode(SLEEP_MODE_IDLE);
    
    sei();

    for(;;){
        /* call game play every 50ms */
        if(game_enable == 1 && game_timer == 1){
            game_timer = 0;
            menu_fn();
        }
        /* play music in background */
        music_bt();
        /* go to sleep */
        sleep_mode();
    }
}

/**
 * 50ms timer interrupt
 */
ISR(TIMER3_COMPA_vect)
{
    game_timer = 1; 
}
