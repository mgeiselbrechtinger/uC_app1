#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>
#include    <string.h>
#include    <util/atomic.h>

#include    "./util.h"

#define CTS     (PJ3)
#define RTS     (PJ2)
#define RST     (PJ5)
#define FOSC    (16000000)
#define BAUD    (1000000)
#define UBRR_VAL (FOSC/(16*BAUD)-1)

/* global variables */
typedef struct{
    uint8_t data[32];
    uint8_t len, head, tail, flag;
} rcv_buff_t;
rcv_buff_t rcv_buff;

typedef struct{
    uint8_t data, flag;
} snd_buff_t;
snd_buff_t snd_buff;

static void (*_sndCallback)(void);
static void (*_rcvCallback)(uint8_t rcvData);

/* prototypes */
static void rcv_buff_empty(void);

/* implementations */

/**
 * Initializes UART module
 *
 * @HW: UART, TIMER1
 * @param: sndCallback, function called after data has been sent
 * @param: rcvCallback, function called after data has been received
 * @return: allways SUCCESS
 * @globals: rcv_buff, snd_buff, _sndCallback, _rcvCallback
 */
error_t halWT41FcUartInit(void (*sndCallback)(), void (*rcvCallback)(uint8_t))
{
    /* setup RST out signal */
    DDRJ  |= (1 << RST);
    PORTJ |= (1 << RST);
    /* setup CTS out signal */
    DDRJ  |=  (1 << CTS);
    PORTJ &= ~(1 << CTS);
    /* setup RTS in signal */
    DDRJ  &= ~(1 << RTS);
    PORTJ &= ~(1 << RTS);

    /* setup buffers */
    memset(&rcv_buff, 0, sizeof rcv_buff);
    memset(&snd_buff, 0, sizeof snd_buff);

    /* setup callback functions */
    _sndCallback = sndCallback;
    _rcvCallback = rcvCallback;

    /* setup UART3, atomic cf. man */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        /* set baudrate to 500k */
        UBRR3 = 1;
        /* double baud to 1M */
        UCSR3A |= (1 << U2X3);
        /* enable receiver and transmitter */
        UCSR3B |= (1 << RXEN3) | (1 << TXEN3);
        /* enable rx interrupt */
        UCSR3B |= (1 << RXCIE3);
        /* set frame format, 8data, 1stop bit */
        UCSR3C = (1 << UCSZ30) | (1 << UCSZ31);
    }

    /* setup pin change interrupt for RTS */
    PCMSK1 |= (1 << PCINT11);

    /* setup TIMER1 to 5ms: prescaler=1024, cnt=79 */
    OCR1A = 79;
    TCNT1 = 0;
    TIMSK1 = (1 << OCIE1A);
    TCCR1A = 0;
    /* start timer */
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);

    /* reset BT module */
    PORTJ &= ~(1 << RST);

    return SUCCESS;
}

/**
 * Sends one byte via UART module
 *
 * @param: byte, that gets sent
 * @return: SUCCESS if byte has been sent, ERROR otherwise
 * @globals :snd_buff
 */
error_t halWT41FcUartSend(uint8_t byte)
{
    snd_buff.data = byte;

    /* test if send possible */
    /* reset still acitve */
    if(!(PORTJ & (1 << RST))){
        snd_buff.flag = 1;
        return ERROR;
    }

    /* RTS set */
    if(PORTJ & (1 << RTS)){
        snd_buff.flag = 1;
        /* activat pin change intr */
        PCICR |= (1 << PCINT1);
        return ERROR;
    }

    /* data reg not empty */
    if(!(UCSR3A & (1 << UDRE3))){
        snd_buff.flag = 1;
        /* activate reg empty intr */
        UCSR3B |= (1 << UDRIE3);
        return ERROR;
    }

    /* send possible */
    snd_buff.flag = 0;

    /* send data */
    UDR3 = byte;

    /* enable tx intr for cb */
    UCSR3B |= (1 << TXCIE3);

    return SUCCESS;
}

/**
 * Uart receive interrupt
 *
 * @brief: stores received byte in ringbuffer
 * @globals: rcv_buff
 */
ISR(USART3_RX_vect)
{
    /* store data in ring buffer */
    rcv_buff.data[rcv_buff.head] = UDR3;
    rcv_buff.head = (rcv_buff.head + 1) & 31;
    rcv_buff.len++;

    /* set control flow */
    if((32 - rcv_buff.len) < 5)
        PORTJ |= (1 << CTS);

    /* empty ringbuffer */
    if(rcv_buff.flag == 0)
        rcv_buff_empty();

}

/**
 * Calls callback for every element in ringbuffer
 *
 * @globals: rcv_buff
 */
static void rcv_buff_empty(void)
{
    /* allow access only once */
    rcv_buff.flag = 1;

    /* call cbf for every item in buffer */
    for(; rcv_buff.len > 0; rcv_buff.len--){

        /* re-enable interrupts for cbf */
        sei();
        _rcvCallback(rcv_buff.data[rcv_buff.tail]);
        cli();
        /* deactivate interrupts again */

        rcv_buff.tail = (rcv_buff.tail + 1) & 31;

        /* release control flow */
        if(rcv_buff.len < 16)
            PORTJ &= ~(1 << CTS);
    } 

    rcv_buff.flag = 0;
    /* interrupts re-enabled through ISR_RX return */
}

/**
 * 5ms timer interrupt
 *
 * @globals: snd_buff
 */
ISR(TIMER1_COMPA_vect)
{
    /* stopp timer */
    TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));

    /* re-enable BT module */
    PORTJ |= (1 << RST);

    sei();
    /* check for buffered msg */
    if(snd_buff.flag == 1)
        halWT41FcUartSend(snd_buff.data);

}

/**
 * RTS pin change interrupt
 *
 * @globals: snd_buff
 */
ISR(PCINT1_vect)
{
    /* dissable PCINT1 */
    PCICR &= ~(1 << PCIE1);

    sei();
    /* check for buffered msg */
    if(snd_buff.flag == 1)
        halWT41FcUartSend(snd_buff.data);

}

/**
 * Uart data register empty interrupt
 *
 * @globals: snd_buff
 */
ISR(USART3_UDRE_vect)
{
    /* dissable reg empty intr */
    UCSR3B &= ~(1 << UDRIE3);

    sei();
    /* check for buffered msg */
    if(snd_buff.flag == 1)
        halWT41FcUartSend(snd_buff.data);

}

/**
 * Uart transmitt finished interrupt
 */
ISR(USART3_TX_vect)
{
    /* dissable transmitt intr */
    UCSR3B &= ~(1 << TXCIE3);

    /* re-enable interrupts */
    sei();

    /* call cbf */
    _sndCallback();

}
