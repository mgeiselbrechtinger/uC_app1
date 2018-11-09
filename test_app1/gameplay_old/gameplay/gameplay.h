#ifndef GAMEPLAY_H
#define GAMEPLAY_H

// prototypes
void gameInit(void);
void gameWiiInit(void);
void gameMenu(void);
void gameHSTable(void);
void gamePlayerSelect(void);
void gameLoop(void);
void gameOver(void);
void gameUserInput(uint8_t button);

void wii_rcv_button(uint8_t wii, uint16_t buttonStates);
void wii_rcv_accel(uint8_t wii, uint16_t x, uint16_t y, uint16_t z);

#endif
