#include    <avr/io.h>
#include    <stdint.h>
#include    <string.h>

#include    "../hal/hal_glcd.h"
#include    "../font/font.h"

#define X_START     (0)
#define X_END       (127)
#define Y_START     (0)
#define Y_END       (63)

#define X_MOD       (127)
#define Y_MOD       (63)

#define PAGE_SIZE   (8)
#define PAGE_MOD    (7)


void glcdInit(void)
{
    halGlcdInit();
}

void glcdSetPixel(const uint8_t x, const uint8_t y)
{
    uint8_t page_address, page_data, pixel;
    
    page_address = y/PAGE_SIZE;
    pixel = y & PAGE_MOD;

    halGlcdSetAddress(x, page_address);

    page_data = halGlcdReadData();

    page_data |= (1 << pixel);

    halGlcdSetAddress(x, page_address);
    halGlcdWriteData(page_data);
}

void glcdClearPixel(const uint8_t x, const uint8_t y)
{    
    uint8_t page_address, page_data, pixel;
    
    page_address = y/PAGE_SIZE;
    pixel = y & PAGE_MOD;

    halGlcdSetAddress(x, page_address);

    page_data = halGlcdReadData();

    page_data &= ~(1 << pixel);

    halGlcdSetAddress(x, page_address);
    halGlcdWriteData(page_data);
}

void glcdInvertPixel(const uint8_t x, const uint8_t y)
{    
    uint8_t page_address, page_data, pixel;
    
    page_address = y/PAGE_SIZE;
    pixel = y & PAGE_MOD;

    halGlcdSetAddress(x, page_address);

    page_data = halGlcdReadData();

    page_data ^= (1 << pixel);

    halGlcdSetAddress(x, page_address);
    halGlcdWriteData(page_data);
}

void glcdDrawLine(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    uint8_t x, y;
    
    x = p1.x;
    y = p2.y;

    while(x != p2.x && y != p2.y){
        drawPx(x, y);
        
        if(x < p2.x)
            x = (x + 1) & X_MOD;
        
        if(x > p2.x)
            x = (x - 1) & X_MOD;

        if(y < p2.y)
            y = (y + 1) & Y_MOD;

        if(y > p2.y)
            y = (y - 1) & Y_MOD;
    }
}

void glcdDrawRect(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    xy_point p_start, p_end;
    uint8_t x_start, y_start, x_end, y_end;
    
    /* sort x coordinate */
    if(p2.x < p1.x){
        x_start = p2.x;
        x_end = p1.x;
    
    }else{
        x_start = p1.x;
        x_end = p2.x;
    }
    /* sort y coordinate */
    if(p2.y < p1.y){
        y_start = p2.y;
        y_end = p1.y;
    
    }else{
        y_start = p1.y;
        y_end = p2.y
    }

    p_start.x = x_start;
    p_start.y = y_start;

    p_end.x = x_end;
    p_end.y = y_end;

    glcdDrawLine(p_start, p_end, drawPx);

    if(y_start < y_end){

    }
}
    

void glcdFillScreen(uint8_t pattern);

void glcdSetYShift(uint8_t yshift);

uint8_t glcdGetYShift(void);

void glcdDrawCircle(const xy_point c, const uint8_t radius, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawEllipse(const xy_point c, const uint8_t radiusX, const uint8_t radiusY, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawVertical(const uint8_t x, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    uint8_t y;
    
    for(y = Y_START; y <= Y_END; y++)    
        drawPx(x, y);

}


void glcdDrawHorizontal(const uint8_t y, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    uint8_t x;
    
    for(x = X_START; x <= X_END; x++)    
        drawPx(x, y);

}

void glcdFillRect(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawChar(const char c, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawText(const char *text, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawTextPgm(PGM_P text, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y));
