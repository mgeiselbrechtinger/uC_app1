#ifndef _GAMEPLAY_H_
#define _GAMEPLAY_H_

// string constants
// max 25 character/line
static const __flash  char    user_txt[] =  "Select player K0-4";
static const __flash  char    menu_txt1[] = "Press K0 for Highscores";
static const __flash  char    menu_txt2[] = "Press K1 to select Player";

// prototypes
void gameInit(void);
void gameMenu(void);
void gameHSTable(void);
void gamePlayerSelect(void);
void gameLoop(void);
void gameOver(void);
void gameUserInput(uint8_t button);

#endif
