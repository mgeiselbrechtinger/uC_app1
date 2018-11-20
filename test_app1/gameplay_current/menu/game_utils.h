#ifndef TEXT_H
#define TEXT_H

#include    <avr/pgmspace.h>

/* glcd position constants */
#define XSTART      (1)
#define XEND        (122)
#define XMID        (XEND/2)
#define YSTART      (63)
#define YEND        (0)
#define YLEN		(64)
#define YMOD		(63)
#define XSTART_TXT  (3)
#define	YSTART_TXT  (10)
#define YLINE_TXT   (10)

/* wii constants */
/* first (high) byte of button */
#define ARROW_DOWN	(0x04)
#define	ARROW_UP	(0x08)
#define ARROW_LEFT	(0x01)
#define	ARROW_RIGHT	(0x02)
#define	BUTTON_PLUS	(0x10)
/* second (low) byte of button */
#define	BUTTON_1	(0x02)
#define BUTTON_2	(0x01)
#define	BUTTON_A	(0x08)
#define	BUTTON_B	(0x04)
#define	BUTTON_MINUS (0x10)
#define BUTTON_HOME	(0x80)

#define	TILT_LEFT	(110)
#define TILT_RIGHT	(146)

/* menu state types */
typedef enum{
    M_WII_INIT,
    M_HOME,
    M_HS_TABLE,
    M_PLAYER_SELECT,
    M_GAME_LOOP,
} M_STATE;

/* intern state types */
typedef enum{
    I_INIT,
    I_DISCONNECTED,
    I_CONNECTED,
    I_SELECT,
    I_PLAY,
    I_GAME_OVER,
    I_IDLE,
} I_STATE;

#define WII_INIT_TABLE_LEN (2)
const char sync_txt[] PROGMEM = "Please press sync!";
const char connect_txt[] PROGMEM = "connecting...";

PGM_P const wii_init_table[] = 
{
    sync_txt,
    connect_txt
};

#define MENU_TABLE_LEN	(3)
const char  menu_hs_txt[] PROGMEM = "HS Table: 1";
const char  menu_sel_txt[] PROGMEM = "Select Player: 2";
const char  menu_title_txt[] PROGMEM = "FALLING DOWN BALL";

PGM_P const menu_table[] PROGMEM =
{
    menu_title_txt,
    menu_hs_txt,
    menu_sel_txt
};

#define USER_NAME_LEN (9*6)
#define USER_SELECT_TABLE_LEN (6)
const char  sel0_txt[] PROGMEM = "Player 0";
const char  sel1_txt[] PROGMEM = "Player 1";
const char  sel2_txt[] PROGMEM = "Player 2";
const char  sel3_txt[] PROGMEM = "Player 3";
const char  sel4_txt[] PROGMEM = "Player 4";
const char  ret_txt[] PROGMEM = "Return: B";
const char  usr_txt[] PROGMEM = "Select User: Arrows";

PGM_P const user_select_table[] PROGMEM =
{
    sel0_txt,
    sel1_txt,
    sel2_txt,
    sel3_txt,
    sel4_txt,
    ret_txt
};

#define HS_TABLE_LEN (6)
const char  hs0_txt[] PROGMEM = "Player0: ";
const char  hs1_txt[] PROGMEM = "Player1: ";
const char  hs2_txt[] PROGMEM = "Player2: ";
const char  hs3_txt[] PROGMEM = "Player3: ";
const char  hs4_txt[] PROGMEM = "Player4: ";

PGM_P const hs_table[] PROGMEM =
{
    hs0_txt,
    hs1_txt,
    hs2_txt,
    hs3_txt,
    hs4_txt,
    ret_txt
};

/* game platforms */
/* game platform number has to be power of 2 */
#define GAME_PLATFORM_NR (4) 
/* game platform coords has to be multiple of 2 */
#define GAME_PLATFORM_COORDS (6)
/* three platforms per line with at least 6 pixel gaps = 6 x-coords out of [0,127] */
const uint8_t game_platform_templates[GAME_PLATFORM_NR][GAME_PLATFORM_COORDS] = { 
    {0, 5, 15, 97, 110, 127},
    {8, 44, 52, 100, 108, 127},
    {0, 66, 75, 111, 118, 120}, 
    {0, 20, 29, 88, 110, 127} 
};


#endif
