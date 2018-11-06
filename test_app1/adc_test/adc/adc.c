#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <util/atomic.h>
#include    <stdint.h>

static volatile uint16_t cnt = 0;

void adcInit(void)
{   
    // PF0 for volume and PF2-3 for rand seed
    // turn off all leds and pullups c.f. bigavr man
    PORTF = 0;
    DDRF  = 0;
    
    // test port
    PORTC = 0x00;
    DDRC  = 0xFF;
    PORTA = 0x00;
    DDRA  = 0xFF;
    PORTB = 0x00;
    DDRB  = 0xFF;

    // clear power reduction bit
    PRR0 &= ~(1 << PRADC);   
    // set first conversion
    ADMUX = (1 << REFS0);
    ADCSRB &= ~(1 << MUX5);
    // set COMP0A auto trigger
    ADCSRB |= (1 << ADTS0) | (1 << ADTS1);
    // set prescaler to 128, start conversion
    ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

    // set up timer 1, 5ms
    TCNT0  = 0;
    OCR0A  = 78;
    TIMSK0 = (1 << OCIE0A);
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS00) | (1 << CS02);
}

ISR(TIMER0_COMPA_vect)
{
    ADCSRA |= (1 << ADEN);
}

// maybe dissable ADC to save power ???
ISR(ADC_vect)
{
    // dissable ADC needed for diff mode
    ADCSRA &= ~(1 << ADEN);

    uint8_t data_lo8 = ADCL;
    uint8_t data_hi8 = (ADCH & 0x03); // only first 2 bit

    if((ADMUX & (1 << MUX0))){
        cnt += 1;

        if(cnt == 50){ 
            PORTC = data_lo8;
            cnt = 0;
        }
        ADMUX = (1 << REFS0);

    }else{
        PORTA = data_hi8;
        PORTB = data_lo8;
        
        ADMUX = (1 << REFS0) | (1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3);
    }

    // change ADC channel
}
