#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>
#include    <util/atomic.h>

#include    "./hal_wt41_fc_uart.h"
#include    "./util.h"

#define CTS     (PJ3)
#define RTS     (PJ2)
#define RST     (PJ5)
#define FOSC    (16000000)
#define BAUD    (1000000)
#define UBRR_VAL (FOSC/(16*BAUD)-1)

static uint8_t buff_active_flag;
static uint8_t rcvBuff[32];
static uint8_t rcv_buff_len;
static uint8_t head;
static uint8_t tail;

static uint8_t sndBuff;
static uint8_t snd_flag;

static void (*_sndCallback)(void);
static void (*_rcvCallback)(uint8_t rcvData);

error_t halWT41FcUartInit(void (*sndCallback)(), void (*rcvCallback)(uint8_t))
{
    // set RTS as input
    PORTJ &= ~((1 << RTS));
	DDRJ  &= ~((1 << RTS));
	// set CTS as output
    DDRJ  |= (1 << CTS);
	PORTJ &= ~(1 << CTS);
	// set RST as output
	DDRJ  |= (1 << RST);
	PORTJ |= (1 << RST);
	
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        // set baudrate to 500k
        UBRR3 = 1;
		// double baud to 1M
		UCSR3A |= (1 << U2X3);
        // UBRR3H = 0x0F & (int8_t)(UBRR_VAL >> 8);
        // UBRR3L = (int8_t)UBRR_VAL;
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
    
    // reset BT module
    PORTJ &= ~(1 << RST);
    
	// setup pinchange interrupt for RTS pin
    PCMSK1 |= (1 << PCINT11);
    
    // setup buffers
    rcv_buff_len = 0;
    head = 0;
    tail = 0;
    snd_flag = 0;
    buff_active_flag = 0;
    
    // add callback functions
    _sndCallback = sndCallback;
    _rcvCallback = rcvCallback;

    return SUCCESS;
}

error_t halWT41FcUartSend(uint8_t byte)
{
    sndBuff = byte;
    snd_flag = 1;
    
	// test if send possible
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
            UCSR3B |= (1 << UDRIE3);
            return SUCCESS;
        }
    }

    // send possible
    snd_flag = 0;   
    UDR3 = byte;
    // enable empty interrupt
    UCSR3B |= (1 << UDRIE3);
	
	// TODO debug
	PORTB |= 1;

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
    
    sei();
    // check for buffered msg
    if(snd_flag == 1)
        halWT41FcUartSend(sndBuff);
}

// UART receive interrupt
ISR(USART3_RX_vect)
{
    rcvBuff[head] = UDR3;
	head = (head + 1) & 31;
	rcv_buff_len++;
	
	if((32 - rcv_buff_len) < 5)
		PORTJ |= (1 << CTS);

    // empty ringbuffer
    if(buff_active_flag == 0)
        empty_buffer();

}

static void empty_buffer()
{
    buff_active_flag = 1;
	// re-enable interrupts
	sei();
   	// TODO debug
   	PORTB |= 32;

	for(; rcv_buff_len > 0; rcv_buff_len--){
		_rcvCallback(rcvBuff[tail]);
		tail = (tail + 1) & 31;

		if(rcv_buff_len < 16)
			PORTJ &= ~(1 << CTS);
	} 
	// TODO debug
	PORTB |= 8;
    buff_active_flag = 0;
}

// RTS pin change interrupt
ISR(PCINT1_vect)
{
    // dissable pinchange interrupt
    PCICR &= ~(1 << PCIE1);
    sei();
    halWT41FcUartSend(sndBuff);
        
}

// UART tx reg empty
ISR(USART3_UDRE_vect)
{
	// TODO debug
	PORTB |= 128;
    // dissable reg empty interrupt
    UCSR3B &= ~(1 << UDRIE3);
    sei();
	if(snd_flag == 1)
    	halWT41FcUartSend(sndBuff);
	else
		_sndCallback();
}
