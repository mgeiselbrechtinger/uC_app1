#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <util/atomic.h>
#include    <stdint.h>

#include    "./adc.h"
#include    "../rand/rand.h"

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
    ADCSRB |= (1 << ADTS0) | (1 << ADTS2);
    // set prescaler to 128, start conversion
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

    // set up timer 1, 5ms
    TCNT1  = 0;
    OCR1A  = 79;
    OCR1B  = 79;
    //TIMSK1 = (1 << OCIE1B);
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
}

// maybe dissable ADC to save power ???
ISR(ADC_vect)
{
    // dissable ADC needed for diff mode
    ADCSRA &= ~(1 << ADEN);

    uint8_t data_lo8 = ADCL;
    uint8_t data_hi8 = (ADCH & 0x03); // only first 2 bit

    // result is rand seed
    if(ADMUX & (1 << MUX0))
        rand_feed(data_lo8);
    else
        adc_vol = (data_hi8 << 8) | data_lo8;
    
    // change adc channel
    // last conversion was rand seed
    if(ADMUX & (1 << MUX0))
        ADMUX = (1 << REFS0);
    else
        ADMUX = (1 << REFS0) | (1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3);

    // ADCSRA |= (1 << ADSC);

    // enable ADC again
    ADCSRA |= (1 << ADEN);
}
