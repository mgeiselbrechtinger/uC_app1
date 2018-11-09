#include    <avr/io.h>
#include    <stdint.h>
#include    <avr/interrupt.h>

#include    "./libglcd/glcd.h"
#include    "./font/Standard5x7.h"
#include    "./gameplay/gameplay.h"

int main(void)
{
    glcdInit();
    sei();
    gameInit();
    for(;;){
        // background tasks
    }
}
