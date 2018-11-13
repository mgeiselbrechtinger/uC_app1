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

static uint8_t rcv_buff[32];
static uint8_t rcv_buff_len;
static uint8_t rcv_buff_head;
static uint8_t rcv_buff_tail;
static uint8_t rcv_buff_flag;

static uint8_t snd_buff;
static uint8_t snd_buff_flag;

static void (*_sndCallback)(void);
static void (*_rcvCallback)(uint8_t rcvData);

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
    rcv_buff_len = 0;
    rcv_buff_head = 0;
    rcv_buff_tail = 0;
    rcv_buff_flag = 0;
    snd_buff_flag = 0;
    
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

error_t halWT41FcUartSend(uint8_t byte)
{
	snd_buff = byte;

	/* test if send possible */
	/* reset still acitve */
	if(!(PORTJ & (1 << RST))){
		snd_buff_flag = 1;
		return ERROR;
	}
	
	/* RTS set */
	if(PORTJ & (1 << RTS)){
		snd_buff_flag = 1;
		/* activat pin change intr */
		PCICR |= (1 << PCINT1);
		return ERROR;
	}

	/* data reg not empty */
	if(!(UCSR3A & (1 << UDRE3))){
		snd_buff_flag = 1;
		/* activate reg empty intr */
		UCSR3B |= (1 << UDRIE3);
		return ERROR;
	}
	
	/* send possible */
	snd_buff_flag = 0;

	/* send data */
    UDR3 = byte;

	/* enable tx intr for cb */
	UCSR3B |= (1 << TXCIE3);

	return SUCCESS;
}

ISR(USART3_RX_vect)
{
	/* store data in ring buffer */
    rcv_buff[rcv_buff_head] = UDR3;
	rcv_buff_head = (rcv_buff_head + 1) & 31;
	rcv_buff_len++;
	
	/* set control flow */
	if((32 - rcv_buff_len) < 5)
		PORTJ |= (1 << CTS);
	
    /* empty ringbuffer */
    if(rcv_buff_flag == 0)
    	rcv_buff_empty();

}

static void rcv_buff_empty(void)
{
	/* allow access only once */
    rcv_buff_flag = 1;
	
	/* call cbf for every item in buffer */
	for(; rcv_buff_len > 0; rcv_buff_len--){

		/* re-enable interrupts for cbf */
		sei();
		_rcvCallback(rcv_buff[rcv_buff_tail]);
		cli();
		/* deactivate interrupts again */

		rcv_buff_tail = (rcv_buff_tail + 1) & 31;
		
		/* release control flow */
		if(rcv_buff_len < 16)
			PORTJ &= ~(1 << CTS);
	} 
    
	rcv_buff_flag = 0;
	/* interrupts re-enabled through ISR_RX return */
}


ISR(TIMER1_COMPA_vect)
{
	/* stopp timer */
	TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));

    /* re-enable BT module */
    PORTJ |= (1 << RST);
	
	sei();
	/* check for buffered msg */
	if(snd_buff_flag == 1)
		halWT41FcUartSend(snd_buff);

}

ISR(PCINT1_vect)
{
	/* dissable PCINT1 */
	PCICR &= ~(1 << PCIE1);
	
	sei();
	/* check for buffered msg */
	if(snd_buff_flag == 1)
		halWT41FcUartSend(snd_buff);

}

ISR(USART3_UDRE_vect)
{
	/* dissable reg empty intr */
	UCSR3B &= ~(1 << UDRIE3);

	sei();
	/* check for buffered msg */
	if(snd_buff_flag == 1)
		halWT41FcUartSend(snd_buff);

}

ISR(USART3_TX_vect)
{
	/* dissable transmitt intr */
	UCSR3B &= ~(1 << TXCIE3);

	/* re-enable interrupts */
	sei();

	/* call cbf */
	_sndCallback();

}
