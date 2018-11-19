#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdint.h>

#include	"./glcd_user/glcd.h"
#include    "./hal/hal_glcd.h"

// TODO do not forget to enable interrupts when further testing
int main(void)
{
    // debug PORTs
    PORTK = 0x00;
    DDRK  = 0xff;
    PORTL = 0x00;
    DDRL  = 0xff;
    /*
	halGlcdInit();
	
	halGlcdFillScreen(0xff);
    //halGlcdSetAddress(0x00, 0x00);
	//halGlcdWriteData(0xff);
	halGlcdSetAddress(0x3f, 0x00);
    halGlcdWriteData(0xf0);
    halGlcdWriteData(0x0f);
	
    uint8_t data1, data2;
    halGlcdSetAddress(0x3f, 0x00);
    data1 = halGlcdReadData();
    data2 = halGlcdReadData();

    PORTK = data1;
    PORTL = data2;
	*/

	glcdInit();
	xy_point p1, p2;
	p1.x = 10;
	p1.y = 30;
	p2.x = 10;
	p2.y = 60;

	glcdDrawLine(p1, p2, &glcdInvertPixel);
	//glcdDrawRect(p1, p2, &glcdSetPixel);

    for(;;){

    }
}
