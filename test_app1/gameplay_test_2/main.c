#include	<avr/io.h>
#include	<avr/interrupt.h>
#include	<stdint.h>

#include	"./menu/menu.h"
#include	"./libglcd/glcd.h"

int main(void)
{
	glcdInit();	
    // setup game tick timer3
    TIMSK3 |= (1 << OCIE3A);
    TCNT3 = 0;
    OCR3A = 3125;
    TCCR3A = 0;
    TCCR3B = (1 << WGM32) | (1 << CS32);
	
	PORTK = 0;
	DDRK = 0xff;
	PORTK |= 1;
	sei();
	
	for(;;){
		
	}
}

ISR(TIMER3_COMPA_vect)
{
	sei();
	menu_fn();
}
