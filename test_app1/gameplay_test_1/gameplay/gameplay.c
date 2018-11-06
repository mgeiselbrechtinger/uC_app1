#include    <avr/io.h>
#include    <avr/pgmspace.h>
#include    <avr/interrupt.h>
#include    <stdint.h>

#include    "./gameplay.h"
#include    "../libglcd/glcd.h"
#include    "../font/Standard5x7.h"

static volatile uint8_t s_tick;
static volatile uint16_t score;
static volatile uint8_t cur_player;
static volatile uint16_t highscore[] = {0, 0, 0, 0, 0};

typedef void (*state_fn)(void);

static volatile state_fn state;

void gameInit(void)
{
    // todo: initialization
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
    // change state
    state = &gameMenu;
    state();
}

void gameMenu(void)
{
    // print text to glcd
    glcdFillScreen(GLCD_CLEAR);
    xy_point p  = { .x = 1, .y = 30 };
    glcdDrawText(menu_txt1, p, &Standard5x7, &glcdSetPixel);
    p.x = 1;
    p.y = 40;
    glcdDrawText(menu_txt2, p, &Standard5x7, &glcdSetPixel);
}

void gameHSTable(void)
{
    // todo: print highscore table
}

void gamePlayerSelect(void)
{
    // print text to glcd
    glcdFillScreen(GLCD_CLEAR);
    xy_point p  = {.x = 1, .y = 30};
    glcdDrawText(menu_txt1, p, &Standard5x7, &glcdSetPixel);
}
    
void gameLoop(void)
{
    // todo: implement game play

    // stop game tick timer
    TCCR3B = 0;
    state = &gameOver;
    state();
}

void gameOver(void)
{
    // set score
    if(score > highscore[cur_player])
        highscore[cur_player] = score;
    // display highscore
    state = &gameHSTable;
    state();
}

void gameUserInput(uint8_t button)
{
    if(state == &gameMenu){
        // enter HS Table
        if(!(button & (1 << PK0)))
            state = &gameHSTable;
        // enter User Select
        if(!(button & (1 << PK1)))
            state = &gamePlayerSelect;

    }else if(state == &gameHSTable){
        // return to menu
        if(!(button & (1 << PK1)))
            state = &gameMenu;

    }else if(state == &gamePlayerSelect){
        // start game
        state = &gameLoop;
        s_tick = 0;
        score  = 0;
        // start game tick timer
        TCNT3  = 0;
        TCCR3B = (1 << WGM32) | (1 << CS32);

        // as player
        switch(button){
            case (1 << PK0): cur_player = 0; break;
            case (1 << PK1): cur_player = 1; break;
            case (1 << PK2): cur_player = 2; break;
            case (1 << PK3): cur_player = 3; break;
            default:         cur_player = 4; break;
        }
    }

    state();
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

    state();
}

// handle user input
ISR(PCINT2_vect)
{
    sei();

    gameUserInput(PINK & 0x1F);
}

