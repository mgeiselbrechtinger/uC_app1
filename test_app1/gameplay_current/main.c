#include	<avr/io.h>
#include	<avr/sleep.h>
#include	<avr/interrupt.h>
#include	<stdint.h>

#include    "./music/music.h"
#include	"./adc/adc.h"
#include	"./menu/menu.h"

#include	"./glcd_user/glcd.h"

static void game_tick_init(void)
{
    /* setup TIMER3: 20Hz game ticks */
    TIMSK3 |= (1 << OCIE3A);
    TCNT3 = 0;
    OCR3A = 3125;
    TCCR3A = 0;
    TCCR3B = (1 << WGM32) | (1 << CS32);
}

int main(void)
{
	// TODO debug
	PORTL = 0;
	DDRK = 0xff;

	/* initializations */
    //adcInit();
    //music_init();
    glcdInit();
	menu_init();
    game_tick_init();
	set_sleep_mode(SLEEP_MODE_IDLE);
	PORTL = 0;
    sei();

    for(;;){
		/* play music in background */
        //music_bt();
		/* go to sleep */
		sleep_enable();
		sleep_cpu();
    }
}

ISR(TIMER3_COMPA_vect)
{
    sei();
	/* update gameplay */
    menu_fn();
}
