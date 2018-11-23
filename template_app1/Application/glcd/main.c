#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>

#include    "./glcd_user/glcd.h"
#include    "./hal/hal_glcd.h"
#include    "./font/Standard5x7.h"
#include    "../menu/game_utils.h"

int main(void)
{
    glcdInit();

    /* space for testing */
    
    sei();

    for(;;)
        ;
}

