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

#define NOP()           asm volatile( "nop\n\t" ::);
#define DELAY_140()     asm volatile( "nop\n\t" "nop\n\t" "nop\n\t" ::);
#define DELAY_450()     asm volatile( "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" ::);

/* global variables */
typedef struct{
    uint8_t x, y;
} address_t;
static address_t address;

/* internal prototypes */
static void halGlcdCtrlWriteData(const uint8_t controller, const uint8_t data);

static uint8_t halGlcdCtrlReadData(const uint8_t controller);

static void halGlcdCtrlWriteCmd(const uint8_t controller, const uint8_t data);

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
    
    /* read status instruction */
    DATA_PORT = 0;
    DATA_DDR  = (1 << D7_PIN) | (1 << D5_PIN) | (1 << D4_PIN); 
    CTRL_PORT |= (1 << RW_PIN);
    CTRL_PORT &= ~(1 << RS_PIN);

    /* enable controller 1 */
    CTRL_PORT |= (1 << CS2_PIN);
    CTRL_PORT &= ~(1 << CS1_PIN);   

    DELAY_140()

    /* enable instruction */
    CTRL_PORT |= (1 << E_PIN);

    /* reset controller */
    CTRL_PORT &= ~(1 << RST_PIN);

    /* busy wait for reset, allowed */
    while(!(DATA_PIN & (1 << D4_PIN)))
        ;

    CTRL_PORT |= (1 << RST_PIN);
        
    DELAY_450()

    /* dissable instruction */
    CTRL_PORT &= ~(1 << E_PIN);

    /* display on instruction */
    DATA_DDR  = 1;
    DATA_PORT = 1;
    DATA_PORT &= ~((1 << D7_PIN) | (1 << D6_PIN));
    CTRL_PORT &= ~((1 << RW_PIN) | (1 << RS_PIN));

    DELAY_140()

    /* enable instruction */
    CTRL_PORT |= (1 << E_PIN);
    
    DELAY_450()

    /* dissable instruction */
    CTRL_PORT &= ~(1 << E_PIN);
    
    /* initialize global variables */
    memset(&address, 0, sizeof address);

    return SUCCESS;
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
    /* select controller */
    if(controller == CONTROLLER1){
        CTRL_PORT |= (1 << CS2_PIN);
        CTRL_PORT &= ~(1 << CS1_PIN);

    }else{
        CTRL_PORT |= (1 << CS1_PIN);
        CTRL_PORT &= ~(1 << CS2_PIN);
    }

    /* control bits for both instructions */
    CTRL_PORT &= ~((1 << RS_PIN) | (1 << RW_PIN));

    /* set up x address */
    DATA_DDR  = 1;
    DATA_PORT = x;
    DATA_PORT |= (1 << D6_PIN);
    DATA_PORT &= ~(1 << D7_PIN);

    DELAY_140()

    /* enable instruction */
    CTRL_PORT |= (1 << E_PIN);
    
    DELAY_450()

    /* dissable instruction */
    CTRL_PORT &= ~(1 << E_PIN);

    /* set up y page */
    DATA_PORT = y;
    DATA_PORT |= (1 << D7_PIN);
    DATA_PORT &= ~(1 << D6_PIN);

    DELAY_140()

    /* enable instruction */
    CTRL_PORT |= (1 << E_PIN);
    
    DELAY_450()

    /* dissable instruction */
    CTRL_PORT &= ~(1 << E_PIN);

}
