#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>
#include    <util/atomic.h>

#include    "./util.h"

#define CTS     (PJ2)
#define RTS     (PJ3)
#define RST     (PJ5)
#define FOSC    (16000000)
#define BAUD    (1000000)
#define UBRR_VAL (FOSC/(16*BAUD)-1)

static uint8_t rcvBuff[32];
static uint8_t head;
static uint8_t tail;
static uint8_t rnd_flag;

static uint8_t sndBuff;
static uint8_t snd_flag;

static void (*_sndCallback)(void);
static void (*_rcvCallback)(uint8_t rcvData);

error_t halWT41FcUartInit(void (*sndCallback)(), void (*rcvCallback)(uint8_t))
{
    // port initialization
    PORTJ = 0;
    DDRJ  = (1 << PJ0) | (1 << CTS) | (1 << RST);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        // set baudrate to 1M
        UBRR3H = 0x0F & (int8_t)(UBRR_VAL >> 8);
        UBRR3L = (int8_t)UBRR_VAL;
        // enable receiver, transmitter and interrupts
        UCSR3B = (1 << RXEN3) | (1 << TXEN3) | (1 << RXCIE3);
        // set frame format, 8data, 1stop bit 
        UCSR3C = (1 << UCSZ30) | (1 << UCSZ31);
    }

    // setup timer 1, 5ms, prescaler=1024, cnt=79
    OCR1A = 79;
    TCNT1 = 0;
    TIMSK1 = (1 << OCIE1A);
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
    
    // setup pinchange interrupt for RTS pin
    PCMSK1 |= (1 << PCINT12);
    
    // reset BT module
    PORTJ &= ~(1 << RST);
    
    // setup receive buffer
    head = 0;
    tail = 0;
    rnd_flag = 0;
    snd_flag = 0;
    
    // add callback functions
    _sndCallback = sndCallback;
    _rcvCallback = rcvCallback;

    return SUCCESS;
}

error_t halWT41FcUartSend(uint8_t byte)
{
    sndBuff = byte;
    snd_flag = 1;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        // reset still active
        if(!(PORTJ  & (1 << RST)))
            return SUCCESS;
        // RTS set
        if(PORTJ & (1 << RTS)){
            // enable pinchange intr on RTS
            PCICR  |= (1 << PCIE1);
            return SUCCESS;
        }
        // UART tx reg full
        if(!(UCSR3A & (1 << UDRE3))){
            // enable intr on reg empty
            UCSR3B |=(1 << UDRIE3);
            return SUCCESS;
        }
    }

    snd_flag = 0;   
    UDR3 = byte;
    _sndCallback();
    return SUCCESS;
}

// reset timer interrupt
ISR(TIMER1_COMPA_vect)
{
    // dissable timer
    TIMSK1 = 0;
    TCCR1B = 0;

    // re-enable BT module
    PORTJ |= (1 << RST);

    // check for buffered msg
    if(snd_flag == 1)
        halWT41FcUartSend(sndBuff);

}

// UART receive interrupt
ISR(USART3_RX_vect)
{
    uint8_t free;

    rcvBuff[head] = UDR3;
    head += 1;

    if(head > 31){
        head = 0;
        rnd_flag = 1;
    }

    if(rnd_flag == 0)
        free = 32 - head - tail;
    else
        free = tail - head;
    
    // buffer too full, set CTS
    if(free < 5)
        PORTJ |= (1 << CTS);
    // buffer empty enough, release CTS
    if(free > 16)
        PORTJ &= ~(1 << CTS);

    // callback with interrupts enabled
    // and re-entering this ISR dissabled 
    UCSR3B &= ~(1 << RXCIE3);
    sei();
    _rcvCallback(rcvBuff[head-1]);
    cli();
    UCSR3B |= (1 << RXCIE3);
}

// RTS pin change interrupt
ISR(PCINT1_vect)
{
    // dissable pinchange interrupt
    PCICR &= ~(1 << PCIE1);
    halWT41FcUartSend(sndBuff);
        
}

// UART tx reg empty
ISR(USART3_UDRE_vect)
{
    // dissable reg empty interrupt
    UCSR3B &= ~(1 << UDRIE3);
    halWT41FcUartSend(sndBuff);
}
