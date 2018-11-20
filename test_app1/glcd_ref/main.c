#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>

#include	"./glcd_user/glcd.h"
#include    "./hal/hal_glcd.h"
#include    "./font/Standard5x7.h"

const char foo[] PROGMEM = "Foo\nNext Foo";
const char bar[] PROGMEM = "Bar";

PGM_P const array[2] PROGMEM = {
   foo,
   bar
};

#define WII_INIT_TABLE_LEN (2)
const char sync_txt[] PROGMEM = "Please press sync!";
const char connect_txt[] PROGMEM = "connecting...";

PGM_P const wii_init_table[] PROGMEM = 
{
    sync_txt,
    connect_txt
};

// TODO do not forget to enable interrupts when further testing
int main(void)
{
    // debug PORTs
    PORTK = 0x00;
    DDRK  = 0xff;
    PORTL = 0x00;
    DDRL  = 0xff;
	
    glcdInit();
    xy_point p1, p2;
	p1.x = 5;
	p1.y = 10;
	p2.x = 120;
	p2.y = 40;
	/*
	glcdFillRect(p1, p2, &glcdInvertPixel);

    p1.x = 60;
    p1.y = 30;
    p2.x = 100;
    p2.y = 30;

	glcdDrawRect(p1, p2, &glcdInvertPixel);

	p1.x = 10;
	p1.y = 0;
	p2.x = 10;
	p2.y = 40;

	glcdDrawRect(p1, p2, &glcdInvertPixel);
	*/

	glcdDrawTextPgm(array[0], p1, &Standard5x7, &glcdInvertPixel);

	for(;;){

    }
}
