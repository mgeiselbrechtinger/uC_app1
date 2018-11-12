#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <util/atomic.h>
#include    <stdint.h>

#include	"./adc.h"
#include	"../music/music.h"
#include	"../rand/rand.h"

#define ADCDIF  ((1 << REFS0) | (1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3))
#define ADCVOL  ((1 << REFS0))

void adcInit(void)
{   
    // PF0 for volume and PF2-3 for rand seed
    // turn off all leds and pullups c.f. bigavr man
    PORTF = 0;
    DDRF  = 0;
    // clear power reduction bit
    PRR0 &= ~(1 << PRADC);   
    // set first conversion
    ADMUX = (1 << REFS0);
    ADCSRB &= ~(1 << MUX5);
    // set COMP0A auto trigger
    ADCSRB |= (1 << ADTS0) | (1 << ADTS1);
    // set prescaler to 128, start conversion
    ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

    // set up timer 0, 5ms
    TCNT0  = 0;
    OCR0A  = 78;
    TIMSK0 = (1 << OCIE0A);
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS00) | (1 << CS02);
}

ISR(TIMER0_COMPA_vect)
{
    // enable ADC
    ADCSRA |= (1 << ADEN);
}

ISR(ADC_vect)
{
    // dissable ADC needed for diff mode
    ADCSRA &= ~(1 << ADEN);
    
    uint16_t adc_data = ADC;
    
    // conversion was ADCDIF
    if((ADMUX & (1 << MUX0))){
        ADMUX = ADCVOL;
		
		sei();

        rand_shift((uint8_t)adc_data);

    // conversion was ADCVOL
    }else{
        
        ADMUX = ADCDIF;
		
		sei();

		if(music_vol_flag == 1)
        	music_set_volume((adc_data >> 2));

    }

}
