#include    <avr/io.h>
#include    <stdint.h>

#include    "./gameplay/gameplay.h"
#include    "./libglcd/glcd.h"

int main(void)
{
    glcdInit();
    gameInit();
    for(;;){
        // background tasks
    }
}
