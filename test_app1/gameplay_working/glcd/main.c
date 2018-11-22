#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>

#include    "./glcd_user/glcd.h"
#include    "./hal/hal_glcd.h"
#include    "./font/Standard5x7.h"
#include    "../menu/game_utils.h"

uint8_t flag;

int main(void)
{
    // TODO debug
    PORTK = 0;
    DDRK = 0xff;
    PORTL = 0;
    DDRL = 0xff;

    /* setup TIMER3: 20Hz game ticks */
    TIMSK3 |= (1 << OCIE3A);
    TCNT3 = 0;
    OCR3A = 65535;
    TCCR3A = 0;
    TCCR3B = (1 << WGM32) | (1 << CS32);
    
    glcdInit();

    /* space for testing */
    /*
    xy_point p1, p2;
    p1.x = 0;
    p2.x = 127;
    p1.y = 0;
    p2.y = 63;

    glcdDrawLine(p1, p2, &glcdInvertPixel);

    p1.y = 30;
    glcdDrawText("Hello World", p1, &Standard5x7, &glcdInvertPixel);
    */
    uint8_t i;
    xy_point p = { .x = XSTART_TXT, .y = YSTART_TXT };

    glcdFillScreen(GLCD_CLEAR);
    /*
    for(i = 0; i < WII_INIT_TABLE_LEN; i++){
        glcdDrawTextPgm(wii_init_table[i], p, &Standard5x7, &glcdSetPixel);
        p.y += YLINE_TXT;
    }
    */
    glcdDrawTextPgm(wii_init_table[0], p, &Standard5x7, &glcdSetPixel);
    p.y += YLINE_TXT;
    glcdDrawTextPgm(wii_init_table[1], p, &Standard5x7, &glcdSetPixel);

    flag = 0;

    //halGlcdFillScreen(0x00);

    sei();

    for(;;){
        if(flag == 1){
            halGlcdSetAddress(0x00, 0x00);
            halGlcdFillScreen(~halGlcdReadData());
            flag = 0;
        }
    }
}

ISR(TIMER3_COMPA_vect)
{
    flag = 0;
}
