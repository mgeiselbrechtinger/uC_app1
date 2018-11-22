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
#define RW_PIN      (PE5)
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

#define CONTROLLER0     (0)
#define CONTROLLER1     (1)
#define	YPAGE_START	(0)
#define YPAGE_END	(7)
#define C1_XCOL_START   (0)
#define C1_XCOL_END     (63)
#define C2_XCOL_START   (64)
#define C2_XCOL_END     (127)
#define START_LINE	(0)

#define MOD_COL         (127)
#define MOD_PAGE        (7)
#define MOD_SHIFT       (63)

#define ERROR           (0)
#define SUCCESS         (1)

#define DISP_ON		(1)
#define DISP_OFF	(0)

#define NOP()           asm volatile( "nop\n\t" ::);    /* 62.5ns delay */
#define SET_UP_DELAY()  asm volatile( "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" ::); /* > 200ns delay */
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

static void halGlcdCtrlWriteCmd(const uint8_t controller, const display_cmd_t cmd, const uint8_t data);

static void halGlcdCtrlSetCmd(const display_cmd_t cmd, const uint8_t data);

static void halGlcdCtrlSetAddress(const uint8_t controller, const uint8_t x, const uint8_t y);

static void halGlcdCtrlBusyWait();

static void halGlcdCtrlSelect(const uint8_t controller);

static void halGlcdCtrlFillScreen(const uint8_t controller, const uint8_t pattern);

/* implementations */

/**
 * Resets and initializes the controllers
 */
uint8_t halGlcdInit(void)
{
    /* initialize global variables */
    memset(&address, 0, sizeof address);

    /* initialize control port */
    CTRL_PORT &= ~((1 << CS1_PIN) | (1 << CS2_PIN));
    CTRL_PORT &= ~(1 << RW_PIN);
    CTRL_PORT &= ~(1 << E_PIN); 
    CTRL_PORT &= ~(1 << RS_PIN);
    CTRL_PORT &= ~(1 << RST_PIN); 
    CTRL_DDR  |=  (1 << CS1_PIN) | (1 << CS2_PIN) | (1<< RW_PIN) | (1 << E_PIN) | (1 << RS_PIN) | (1 << RST_PIN);

    /* wait 1us befor pulling reset high */
    HOLD_DELAY()
    HOLD_DELAY()
    HOLD_DELAY()
    CTRL_PORT |= (1 << RST_PIN);

    /* turn on controller 0 */
    halGlcdCtrlWriteCmd(CONTROLLER0, DISP_CMD, DISP_ON);
    halGlcdCtrlWriteCmd(CONTROLLER0, START_CMD, START_LINE);

    /* turn on controller 1 */
    halGlcdCtrlWriteCmd(CONTROLLER1, DISP_CMD, DISP_ON);
    halGlcdCtrlWriteCmd(CONTROLLER1, START_CMD, START_LINE);

    /* clear Screen */
    halGlcdFillScreen(0x00);

    return SUCCESS;
}

/**
 * Writes command to glcd
 *
 * @param: controller, index
 * @param: cmd, command
 * @param: data, data to be written
 */
static void halGlcdCtrlWriteCmd(const uint8_t controller, const display_cmd_t cmd, const uint8_t data)
{
    HOLD_DELAY()

    halGlcdCtrlSelect(controller);
    
    halGlcdCtrlBusyWait();

    halGlcdCtrlSetCmd(cmd, data);

    SET_UP_DELAY()
    CTRL_PORT |= (1 << E_PIN);

    HOLD_DELAY()
    CTRL_PORT &= ~(1 << E_PIN);
}

/**
 * Selects controller
 *
 * @param: controller, index
 */
static void halGlcdCtrlSelect(const uint8_t controller)
{
    /* select controller */
    if(controller == CONTROLLER0){
        CTRL_PORT |= (1 << CS2_PIN);
        CTRL_PORT &= ~(1 << CS1_PIN);

    }else{
        CTRL_PORT |= (1 << CS1_PIN);
        CTRL_PORT &= ~(1 << CS2_PIN);
    }

}

/**
 * Waits for busy signal
 */
static void halGlcdCtrlBusyWait(void)
{
    halGlcdCtrlSetCmd(STAT_CMD, 0);

    SET_UP_DELAY()
    CTRL_PORT |= (1 << E_PIN);

    HOLD_DELAY()
    CTRL_PORT &= ~(1 << E_PIN);

    /* busy wait for busy and reset flag, allowed */
    while(DATA_PIN & ((1 << D7_PIN) | (1 << D4_PIN)))
        ;

}

/**
 * Sets up in- and outputs for command
 *
 * @param: cmd, command
 * @param: data, data to be written
 */
static void halGlcdCtrlSetCmd(const display_cmd_t cmd, const uint8_t data)
{
    switch(cmd){

        case DISP_CMD:  CTRL_PORT &= ~((1 << RS_PIN) | (1 << RW_PIN));
                        DATA_PORT = data;
                        DATA_PORT |= 0xfe;
                        DATA_PORT &= ~((1 << D7_PIN) | (1 << D6_PIN));
                        DATA_DDR = 0xff;
                        break;

        case PAGE_CMD:  CTRL_PORT &= ~((1 << RS_PIN) | (1 << RW_PIN));
                        DATA_PORT = data;
                        DATA_PORT |= (1 << D7_PIN) | (1 << D5_PIN) | (1 << D4_PIN) | (1 << D3_PIN);
                        DATA_PORT &= ~(1 << D6_PIN);
                        DATA_DDR = 0xff;
                        break;

        case COL_CMD:   CTRL_PORT &= ~((1 << RS_PIN) | (1 << RW_PIN));
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

/**
 * Sets address of RAM
 *
 * @param: xCol, address of column
 * @param: yPage, address of page
 * @return: allways SUCCESS 
 * @globals: address
 */
uint8_t halGlcdSetAddress(const uint8_t xCol, const uint8_t yPage)
{
    /* update address */
    address.x = xCol & MOD_COL;
    address.y = yPage & MOD_PAGE;

    if(address.x < C2_XCOL_START)
        halGlcdCtrlSetAddress(CONTROLLER0, address.x, address.y);
    else
        halGlcdCtrlSetAddress(CONTROLLER1, address.x - C2_XCOL_START, address.y);

    return SUCCESS;
}

/**
 * Auxiliary function to setAdress
 *
 * @param: controller, index
 * @param: x, address in RAM
 * @param: y, address in RAM
 */
static void halGlcdCtrlSetAddress(const uint8_t controller, const uint8_t x, const uint8_t y)
{
    halGlcdCtrlWriteCmd(controller, COL_CMD, x);

    halGlcdCtrlWriteCmd(controller, PAGE_CMD, y);

}

/**
 * Writes data to RAM
 *
 * @param: data, to be written
 * @return: allways SUCCESS
 * @globals: address
 */
uint8_t halGlcdWriteData(const uint8_t data)
{

    if(address.x < C2_XCOL_START)
        halGlcdCtrlWriteData(CONTROLLER0, data);
    else
        halGlcdCtrlWriteData(CONTROLLER1, data);

    /* post increment of write function */
    address.x = (address.x + 1) & MOD_COL;
    /* enable controller when crossing borders */
    if(address.x == C1_XCOL_START || address.x == C2_XCOL_START)
        halGlcdSetAddress(address.x, address.y);

    return SUCCESS;
}

/**
 * Auxiliary function to writeData
 *
 * @param: controller, index
 * @param: data, to be written
 */
static void halGlcdCtrlWriteData(const uint8_t controller, const uint8_t data)
{
    halGlcdCtrlWriteCmd(controller, WR_CMD, data);

}

/**
 * Reads data of RAM
 *
 * @return: data, that has been read
 * @globals: address
 */
uint8_t halGlcdReadData(void)
{
    uint8_t data;

    if(address.x < C2_XCOL_START)
        data = halGlcdCtrlReadData(CONTROLLER0);
    else
        data = halGlcdCtrlReadData(CONTROLLER1);

    /* post increment of write function */
    address.x = (address.x + 1) & MOD_COL;
    /* enable controller when crossing borders */
    if(address.x == C1_XCOL_START || address.x == C2_XCOL_START)
        halGlcdSetAddress(address.x, address.y);

    return data;
}

/**
 * Auxiliary function to readData
 *
 * @param: controller, index
 */
uint8_t halGlcdCtrlReadData(const uint8_t controller)
{
    uint8_t data;

    halGlcdCtrlSelect(controller);

    halGlcdCtrlBusyWait();

    /* dummy read */
    halGlcdCtrlSetCmd(RD_CMD, 0);
 
    SET_UP_DELAY()
    CTRL_PORT |= (1 << E_PIN);

    HOLD_DELAY()
    CTRL_PORT &= ~(1 << E_PIN);

    halGlcdCtrlBusyWait();

    /* actual read */
    halGlcdCtrlSetCmd(RD_CMD, 0);

    SET_UP_DELAY()
    CTRL_PORT |= (1 << E_PIN);

    HOLD_DELAY()
    data = DATA_PIN;

    /* short delay before reseting enable */
    HOLD_DELAY()
    CTRL_PORT &= ~(1 << E_PIN);
    
    return data;
}

/**
 * Auxiliary function to fillScreen
 *
 * @param: controller, index
 * @param: pattern, to be written accross screen
 */
static void halGlcdCtrlFillScreen(const uint8_t controller, const uint8_t pattern)
{
    uint8_t x, y;

    for(y = 0; y <= 7; y++){
        halGlcdCtrlSetAddress(controller, 0 , y);

        for(x = 0; x <= 63; x++){
            halGlcdCtrlWriteData(controller, pattern);
        }
    }

}

/**
 * Fills screen with pattern
 *
 * @param: patter, to be written accross screen
 * @return: allways SUCCESS
 */
uint8_t halGlcdFillScreen(const uint8_t pattern)
{
    halGlcdCtrlFillScreen(CONTROLLER0, pattern);
    halGlcdCtrlFillScreen(CONTROLLER1, pattern);

    return SUCCESS;
}

/**
 * Set y address of RAM
 *
 * @param: yshift, address of top line
 * @return: allways SUCCESS
 * @globals: address
 */
uint8_t halGlcdSetYShift(const uint8_t yshift)
{
    address.y = yshift & MOD_SHIFT;

    halGlcdCtrlWriteCmd(CONTROLLER0, START_CMD, address.y);

    halGlcdCtrlWriteCmd(CONTROLLER1, START_CMD, address.y);

    return SUCCESS;
}

/**
 * Get y address of RAM
 *
 * @return current yshift of RAM
 * @globals address
 */ 
uint8_t halGlcdGetYShift(void)
{
    return address.y;

}

