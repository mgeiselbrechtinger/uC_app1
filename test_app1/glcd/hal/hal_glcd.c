#include    <avr/io.h>
#include    <stdint.h>
#include    <string.h>

#include    "./hal_glcd.h"

/* constants */
#define CTRL_PORT   (PORTE)
#define CTRL_DDR    (DDRE)
#define DATA_PORT   (PORTA)
#define DATA_DDR    (DDRA)
#define DATA_PIN    (PINA)
#define CS1_PIN     (PE2)
#define CS2_PIN     (PE3)
#define RS_PIN      (PE4)
#define RW_PIN     (PE5)
#define E_PIN       (PE6)
#define RST_PIN     (PE7)
#define D0_PIN      (PA0)
#define D1_PIN      (PA1)
#define D2_PIN      (PA2)
#define D3_PIN      (PA3)
#define D4_PIN      (PA4)
#define D5_PIN      (PA5)
#define D6_PIN      (PA6)
#define D7_PIN      (PA7)

#define CONTROLLER1     (1)
#define CONTROLLER2     (2)
#define C1_XCOL_START   (0)
#define C1_XCOL_END     (63)
#define C2_XCOL_START   (64)
#define C2_XCOL_END     (127)

#define MOD_COL         (127)
#define MOD_PAGE        (7)

#define ERROR           (0)
#define SUCCESS         (1)

#define NOP()           asm volatile( "nop\n\t" ::);    /* 62.5ns delay */
#define SET_UP_DELAY()  asm volatile( "nop\n\t" "nop\n\t" "nop\n\t" ::); /* > 140ns delay */
#define DATA_RDY_DELAY() asm volatile( "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" ::); /* > 320ns delay */
#define HOLD_DELAY()    asm volatile( "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" ::);  /* > 450ns delay */

/* global variables */
typedef struct{
    uint8_t x, y;
} address_t;
static address_t address;

typedef enum{
    DISP_CMD,
    COL_CMD,
    PAGE_CMD,
    START_CMD,
    STAT_CMD,
    WR_CMD,
    RD_CMD
} display_cmd_t;

/* internal prototypes */
static void halGlcdCtrlWriteData(const uint8_t controller, const uint8_t data);

static uint8_t halGlcdCtrlReadData(const uint8_t controller);

static void halGlcdCtrlWriteCmd(const display_cmd_t cmd, const uint8_t data);

static void halGlcdCtrlSetAddress(const uint8_t controller, const uint8_t x, const uint8_t y);

static void halGlcdCtrlBusyWait(const uint8_t controller);

static void halGlcdCtrlSelect(const uint8_t controller);

/* implementations */

/**
 * resets and initializes the controllers
 */
uint8_t halGlcdInit(void)
{
    /* initialize control port */
    CTRL_PORT |=  (1 << CS1_PIN) | (1 << CS2_PIN);
    CTRL_PORT &= ~(1 << RW_PIN);
    CTRL_PORT &= ~(1 << E_PIN); 
    CTRL_PORT &= ~(1 << RS_PIN);
    CTRL_PORT |=  (1 << RST_PIN);
    CTRL_DDR  |=  (1 << CS1_PIN) | (1 << CS2_PIN) | (1<< RW_PIN) | (1 << E_PIN) | (1 << RS_PIN) | (1 << RST_PIN);
    
    /* reset controller */
    CTRL_PORT &= ~(1 << RST_PIN);
	
	halGlcdCtrlSelect(1);
	halGlcdCtrlWriteCmd(STAT_CMD, 0);
	
	SET_UP_DELAY()
	
	CTRL_PORT |= (1 << E_PIN);

    
	while(!(DATA_PIN & (D4_PIN)))
		;

	HOLD_DELAY()

	PORTK = 1; 

    /* dissable instruction */
    CTRL_PORT &= ~(1 << E_PIN);
	
    /* initialize global variables */
    memset(&address, 0, sizeof address);

    return SUCCESS;
}

void halGlcdCtrlSelect(const uint8_t controller)
{
    /* select controller */
    if(controller == CONTROLLER1){
        CTRL_PORT |= (1 << CS2_PIN);
        CTRL_PORT &= ~(1 << CS1_PIN);

    }else{
        CTRL_PORT |= (1 << CS1_PIN);
        CTRL_PORT &= ~(1 << CS2_PIN);
    }

}

static void halGlcdCtrlWriteCmd(const display_cmd_t cmd, const uint8_t data)
{
   switch(cmd){

        case DISP_CMD:  CTRL_PORT &= ~((1 << RS_PIN) | (1 << RW_PIN));
                        DATA_PORT = data;
                        DATA_PORT |= 0xfe;
                        DATA_PORT &= ~((1 << D7_PIN) | (1 << D6_PIN));
                        DATA_DDR = 0xff;
                        break;

        case COL_CMD:   CTRL_PORT &= ~((1 << RS_PIN) | (1 << RW_PIN));
                        DATA_PORT = data;
                        DATA_PORT &= ~(1 << D6_PIN);
                        DATA_PORT |=  (1 << D7_PIN);
                        DATA_DDR = 0xff;
                        break;

        case PAGE_CMD:  CTRL_PORT &= ~((1 << RS_PIN) | (1 << RW_PIN));
                        DATA_PORT = data;
                        DATA_PORT &= ~(1 << D7_PIN);
                        DATA_PORT |=  (1 << D6_PIN);
                        DATA_DDR = 0xff;
                        break;

        case START_CMD: CTRL_PORT &= ~((1 << RS_PIN) | (1 << RW_PIN));
                        DATA_PORT = data;
                        DATA_PORT |=  (1 << D7_PIN) | (1 << D6_PIN);
                        DATA_DDR = 0xff;
                        break;

        case STAT_CMD:  CTRL_PORT &= ~(1 << RS_PIN);
                        CTRL_PORT |=  (1 << RW_PIN);
                        DATA_PORT = 0x00;
                        DATA_DDR  = 0xff;
                        DATA_DDR &= ~((1 << D7_PIN) | (1 << D5_PIN) | (1 << D4_PIN));
                        break;

        case WR_CMD:    CTRL_PORT &= ~(1 << RW_PIN);
                        CTRL_PORT |=  (1 << RS_PIN);
                        DATA_PORT = data;
                        DATA_DDR  = 0xff;
                        break;

        case RD_CMD:    CTRL_PORT |= (1 << RS_PIN) | (1 << RW_PIN);
                        DATA_PORT = 0x00;
                        DATA_DDR  = 0x00;
                        break;

        default:        break;

   }

}

uint8_t halGlcdSetAddress(const uint8_t xCol, const uint8_t yPage)
{
    /* update address */
    address.x = xCol & MOD_COL;
    address.y = yPage & MOD_PAGE;
	
    if(address.x < C2_XCOL_START)
        halGlcdCtrlSetAddress(CONTROLLER1, address.x, address.y);
    else
        halGlcdCtrlSetAddress(CONTROLLER2, address.x - C2_XCOL_START, address.y);

    return SUCCESS;
}

static void halGlcdCtrlSetAddress(const uint8_t controller, const uint8_t x, const uint8_t y)
{
    halGlcdCtrlBusyWait(controller);

    /* set x-column address */
    halGlcdCtrlWriteCmd(COL_CMD, x);

    SET_UP_DELAY()

    /* enable instruction */
    CTRL_PORT |= (1 << E_PIN);
    
    HOLD_DELAY()

    /* dissable instruction */
    CTRL_PORT &= ~(1 << E_PIN);

    /* data hold timing */
    NOP()

    /* set y-page address */
    halGlcdCtrlWriteCmd(PAGE_CMD, y);

    SET_UP_DELAY()

    /* enable instruction */
    CTRL_PORT |= (1 << E_PIN);
    
    HOLD_DELAY()

    /* dissable instruction */
    CTRL_PORT &= ~(1 << E_PIN);

}

uint8_t halGlcdWriteData(const uint8_t data)
{

    if(address.x < C2_XCOL_START)
        halGlcdCtrlWriteData(CONTROLLER1, data);
    else
        halGlcdCtrlWriteData(CONTROLLER2, data);

    /* post increment of write function */
    address.x = (address.x + 1) & MOD_COL;

    return SUCCESS;
}

void halGlcdCtrlWriteData(const uint8_t controller, const uint8_t data)
{
    halGlcdCtrlBusyWait(controller);

    halGlcdCtrlWriteCmd(WR_CMD, data);

    SET_UP_DELAY()

    /* enable instruction */
    CTRL_PORT |= (1 << E_PIN);

    HOLD_DELAY()

    /* dissable instruction */
    CTRL_PORT &= ~(1 << E_PIN);

}

uint8_t halGlcdReadData(void)
{
    uint8_t data;

    if(address.x < C2_XCOL_START)
        data = halGlcdCtrlReadData(CONTROLLER1);
    else
        data = halGlcdCtrlReadData(CONTROLLER2);

    /* post increment of read function */
    address.x = (address.x + 1) & MOD_COL;

    return data;
}

uint8_t halGlcdCtrlReadData(const uint8_t controller)
{
	halGlcdCtrlBusyWait(controller);

    uint8_t data;

    halGlcdCtrlWriteCmd(RD_CMD, 0);

    SET_UP_DELAY()

    /* enable instruction */
    CTRL_PORT |= (1 << E_PIN);

    DATA_RDY_DELAY()

    /* data valid */
    data = DATA_PIN;

    NOP()

    /* dissable instruction */
    CTRL_PORT &= ~(1 << E_PIN);

    return data;
}

static void halGlcdCtrlBusyWait(const uint8_t controller)
{
	halGlcdCtrlSelect(controller);

	/* test if busy */
	halGlcdCtrlWriteCmd(STAT_CMD, 0);
	
	SET_UP_DELAY()
	
    /* enable instruction */
    CTRL_PORT |= (1 << E_PIN);
    
	while(DATA_PIN & (1 << D5_PIN) || (!(DATA_PIN & (1 << D4_PIN))))
		;

    HOLD_DELAY()

    /* dissable instruction */
    CTRL_PORT &= ~(1 << E_PIN);

}
