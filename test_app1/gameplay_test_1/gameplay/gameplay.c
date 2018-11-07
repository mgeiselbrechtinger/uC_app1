#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdint.h>

#include    <avr/pgmspace.h>

#include    "./gameplay.h"
#include    "./text.h"
#include    "../libglcd/glcd.h"
#include    "../font/Standard5x7.h"



// string constants
// max 20 character/line
const char * const user_txt1 PROGMEM  = "Select player K0-4";
const char * const user_txt2 PROGMEM  = "K0 for menu";
const char * const menu_txt1 PROGMEM = "K0 for Highscores";
const char * const menu_txt2 PROGMEM = "K1 to select Player";
const char * const hs_txt1 PROGMEM   = "K1 for menu";


// function pointer
typedef void (*state_fn)(void);
volatile state_fn gameState;

static volatile uint8_t s_tick;
static volatile uint16_t score;
static volatile uint8_t player;
static volatile uint16_t highscore[] = {0, 0, 0, 0, 0};


void gameInit(void)
{
    // user input port
    PORTK |= (1 << PK0) | (1 << PK1) | (1 << PK2) | (1 << PK3) | (1<< PK4);
    DDRK  &= ~((1 << PK0) | (1 << PK1) | (1 << PK2) | (1 << PK3) | (1<< PK4));
    // setup pinchange interrupt
    PCMSK2 = (1 << PCINT16) | (1 << PCINT17) | (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20);
    PCICR |= (1 << PCIE2);

    // setup game tick timer3
    // TCNT3 = 0;
    OCR3A = 3125;
    TCCR3A = 0;
    // TCCR3B = (1 << WGM32) | (1 << CS32);

    // TODO: change to gameWiiInit
    gameState = &gameMenu;
    gameState();
}

void gameWiiInit(void)
{
    // TODO: setup wii callbacks
    // TODO: call wii connect
    // print info to press sync button
}

void gameMenu(void)
{
    // print menu to glcd
    glcdFillScreen(GLCD_CLEAR);
    xy_point p  = { .x = 1, .y = 0 };
    glcdDrawText(menu_txt1, p, &Standard5x7, &glcdSetPixel);
    p.x = 1;
    p.y = 40;
    glcdDrawText(menu_txt2, p, &Standard5x7, &glcdSetPixel);
}

void gameHSTable(void)
{
    char txt_buff[15];
    char int_buff[6];
    uint8_t i;
    xy_point p = {.x = 1, .y = 0};

    glcdFillScreen(GLCD_CLEAR);
    for(i = 0; i <= 4; i++){
        sprintf(int_buff, "%d", highscore[i]);
        strcpy_P(txt_buff, (PGM_P)pgm_read_word(&(hs_table[i])));
        strcat(txt_buff, int_buff);
        glcdDrawText(txt_buff, p, &Standard5x7, &glcdSetPixel);
        p.y += 10;
    }
    glcdDrawTextPgm(hs_table[i], p, &Standard5x7, &glcdSetPixel);
}

void gamePlayerSelect(void)
{
    // print text to glcd
    glcdFillScreen(GLCD_CLEAR);
    xy_point p  = {.x = 1, .y = 20};
    glcdDrawText(user_txt1, p, &Standard5x7, &glcdSetPixel);
}
    
void gameLoop(void)
{
    // TODO: implement game play
    //
    // read accelerometer 
    // adjust ball
    // shift screen
    // 
    // check if game over
    if(score == 5){
        TCCR3B = 0;
        gameState = &gameOver;
        gameState();
    }
}

void gameOver(void)
{
    // set score
    if(score > highscore[player])
        highscore[player] = score;
    // display highscore
    gameState = &gameHSTable;
    gameState();
}

void gameUserInput(uint8_t button)
{
    if(gameState == &gameMenu){
        // enter HS Table
        if(button & (1 << PK0))
            gameState = &gameHSTable;
        // enter User Select
        if(button & (1 << PK1))
            gameState = &gamePlayerSelect;

    }else if(gameState == &gameHSTable){
        // return to menu
        if(button & (1 << PK1))
            gameState = &gameMenu;

    }else if(gameState == &gamePlayerSelect){
        // start game
        gameState = &gameLoop;
        s_tick = 0;
        score  = 0;
        // start game tick timer
        TCNT3  = 0;
        TCCR3B = (1 << WGM32) | (1 << CS32);

        // as player
        switch(button){
            case (1 << PK0): player = 0; break;
            case (1 << PK1): player = 1; break;
            case (1 << PK2): player = 2; break;
            case (1 << PK3): player = 3; break;
            case (1 << PK4): player = 4; break;
            default: TCCR3B = 0; gameState = &gamePlayerSelect ; break;
        }
    }

    gameState();
}

// updates game every 50ms
ISR(TIMER3_COMPA_vect)
{
    s_tick += 1;
    // a second passed
    if(s_tick == 19){
        s_tick = 0;
        score += 1;
    }
    
    sei();

    gameState();
}

// handle user input
ISR(PCINT2_vect)
{
    sei();
    // invert pins
    gameUserInput((~PINK) & 0x1F);
}

