#include	<avr/io.h>
#include	<avr/interrupt.h>
#include	<stdint.h>

#include    "./music/music.h"
#include	"./adc/adc.h"
#include	"./menu/menu.h"
#include	"./libglcd/glcd.h"

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
    PORTK = 0;
    DDRK = 0xff;

	adcInit();
	music_init();
    glcdInit();	
    game_tick_init();
    sei();

    for(;;){
		//music_bt();
    }
}

ISR(TIMER3_COMPA_vect)
{
    sei();
    menu_fn();
}
