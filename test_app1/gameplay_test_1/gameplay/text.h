#ifndef TEXT_H
#define TEXT_H

#include    <avr/pgmspace.h>

const char * const menu_hs_txt PROGMEM = "K0 for HS Table";
const char * const menu_sel_txt PROGMEM = "K1 for Player select";

PGM_P const menu_table[] PROGMEM =
{
    menu_hs_txt,
    menu_sel_txt
};

const char * const ret_txt PROGMEM = "Return K0";
const char * const usr_txt PROGMEM = "Select User K0-4";

PGM_P const user_select_table[] PROGMEM =
{
    ret_txt,
    usr_txt
};

const char * const hs0_txt PROGMEM = "Player0: ";
const char * const hs1_txt PROGMEM = "Player1: ";
const char * const hs2_txt PROGMEM = "Player2: ";
const char * const hs3_txt PROGMEM = "Player3: ";
const char * const hs4_txt PROGMEM = "Player4: ";

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
