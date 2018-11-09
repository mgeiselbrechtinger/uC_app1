#ifndef TEXT_H
#define TEXT_H

#include    <avr/pgmspace.h>

#define XSTART	(3)
#define	YSTART	(10)
#define YLINE	(10)


#define WII_INIT_TABLE_LEN (2)
const char sync_txt[] PROGMEM = "Please press sync!";
const char connect_txt[] PROGMEM = "connecting...";

PGM_P const wii_init_table[] PROGMEM = 
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

#endif
