#include    <avr/io.h>
#include    <avr/sleep.h>
#include    <avr/interrupt.h>
#include    <stdint.h>

#include    "./music/music.h"
#include    "./adc/adc.h"
#include    "./menu/menu.h"
#include    "./libglcd/glcd.h"

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
        /* play music in background */
        music_bt();
        /* go to sleep */
        sleep_enable();
        sleep_cpu();
    }
}

/**
 * 5ms timer interrupt
 */
ISR(TIMER3_COMPA_vect)
{
    sei();
    /* update gameplay */
    menu_fn();
}
