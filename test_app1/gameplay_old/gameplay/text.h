#ifndef TEXT_H
#define TEXT_H

#include    <avr/pgmspace.h>

#define WII_INIT_TABLE_LEN (2)
const char sync_txt[] PROGMEM = "Please press sync!";
const char connect_txt[] PROGMEM = "connecting...";

PGM_P const wii_init_table[] PROGMEM = 
{
	sync_txt,
	connect_txt
};

#define MENU_TABLE_LEN	(2)
const char  menu_hs_txt[] PROGMEM = "K0 for HS Table";
const char  menu_sel_txt[] PROGMEM = "K1 for Player select";

PGM_P const menu_table[] PROGMEM =
{
    menu_hs_txt,
    menu_sel_txt
};

#define USER_SELECT_TABLE_LEN (2)
const char  ret_txt[] PROGMEM = "Return K0";
const char  usr_txt[] PROGMEM = "Select User K0-4";

PGM_P const user_select_table[] PROGMEM =
{
    usr_txt,
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