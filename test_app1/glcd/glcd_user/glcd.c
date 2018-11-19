#include    <avr/io.h>
#include    <avr/pgmspace.h>
#include    <stdlib.h>
#include    <stdint.h>
#include    <string.h>

#include    "./glcd.h"
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

/* prototypes */
static void glcdDrawLineLow(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y));

static void glcdDrawLineHigh(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y));

/* implementations */
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

static void glcdDrawLineLow(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    int8_t dx, dy, yi, D;
    uint8_t x, y;

    dx = p2.x - p1.x;
    dy = p2.y - p1.y;
    
    yi = 1;
    if(dy < 0){
        yi = -1;
        dy = -dy;
    }

    D = 2*dy - dx;
    y = p1.y;

    for(x = p1.x; x <= p2.x; x++){
        drawPx(x, y);

        if(D > 0){
            y += yi;
            D -= 2*dx;
        }

        D += 2*dy;
    }
}

static void glcdDrawLineHigh(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    int8_t dx, dy, xi, D;
    uint8_t x, y;

    dx = p2.x - p1.x;
    dy = p2.y - p1.y;
    
    xi = 1;
    if(dx < 0){
        xi = -1;
        dx = -dx;
    }

    D = 2*dx - dy;
    x = p1.x;

    for(y = p1.y; y <= p2.y; y++){
        drawPx(x, y);

        if(D > 0){
            x += xi;
            D -= 2*dy;
        }

        D += 2*dx;
    }
}

/* Bresenham algorithm */
void glcdDrawLine(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    if(abs(p2.y - p1.y) < abs(p2.x - p1.x)){
        if(p1.x > p2.x)
            glcdDrawLineLow(p2, p1, drawPx);
        else
            glcdDrawLineLow(p1, p2, drawPx);
    
    }else{
        if(p1.y > p2.y)
            glcdDrawLineHigh(p2, p1, drawPx);
        else
            glcdDrawLineHigh(p1, p2, drawPx);
    }
}   

// TODO corner cases
void glcdDrawRect(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    xy_point p_start, p_end;
    
    /* first horizontal */
    p_start.x = p1.x;
    p_start.y = p1.y;
    p_end.x = p2.x;
    p_end.y = p1.y;
    glcdDrawLine(p_start, p_end, drawPx);

    /* second horizontal */
    p_start.y = p2.y;
    p_end.y = p2.y;    
    glcdDrawLine(p_start, p_end, drawPx);

    /* first vertical */
    p_start.x = p1.x;
    p_start.y = p1.y;
    p_end.x = p1.x;
    p_end.y = p2.y;
    glcdDrawLine(p_start, p_end, drawPx);

    /* second vertical */
    p_start.x = p2.x;
    p_end.x = p2.x;
    glcdDrawLine(p_start, p_end, drawPx);

}
    

void glcdFillScreen(uint8_t pattern)
{
    halGlcdFillScreen(pattern);

}

void glcdSetYShift(uint8_t yshift)
{
    halGlcdSetYShift(yshift);

}

uint8_t glcdGetYShift(void)
{
    return halGlcdGetYShift();

}

/* bresenham algorithm */
void glcdDrawCircle(const xy_point c, const uint8_t radius, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    int8_t r, x, y, err;
    r = radius;
    x = -r;             /* start with second quadrant */
    y = 0; 
    err = 2 - 2*radius; /* error of first step */

    do{
        drawPx(c.x-x, c.y+y);   /*   I. Quadrant */
        drawPx(c.x-y, c.y-x);   /*  II. Quadrant */
        drawPx(c.x+x, c.y-y);   /* III. Quadrant */
        drawPx(c.x+y, c.y+x);   /*  IV. Quadrant */
    
        r = err;

        /* update y */       
        if(r <= y){
            y++;
            err += y*2 + 1;
        }

        /* update x */
        if(r > x || err > y){
            x++;
            err += x*2 + 1;
        }

    }while(x < 0);
}

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

void glcdFillRect(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    xy_point p_start, p_end, p_tmp;
    
    /* sort x coordinate */
    if(p2.x < p1.x){
        p_start.x = p2.x;
        p_end.x = p1.x;
    
    }else{
        p_start.x = p1.x;
        p_end.x = p2.x;
    }
    /* sort y coordinate */
    if(p2.y < p1.y){
        p_start.y = p2.y;
        p_end.y = p1.y;
    
    }else{
        p_start.y = p1.y;
        p_end.y = p2.y;
    }
    
    p_tmp.x = p_end.x;

    for(; p_start.y < p_end.y; p_start.y++){
        p_tmp.y = p_start.y;
        glcdDrawLine(p_start, p_tmp, drawPx);
    }

    if(p_start.y == p_end.y)
        glcdDrawLine(p_start, p_end, drawPx);

}

void glcdDrawChar(const char c, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    xy_point pos;
    const uint8_t *c_p;
    uint8_t offset, data, i, j;

    /* check range of char */
    if(c >= f->startChar && c <= f->endChar)
        offset = c - f->startChar;
    else 
        return;

    c_p = f->font;
    c_p += f->width * offset;
    pos.x = p.x;
    pos.y = p.y;

    /* for each byte */
    for(i = 0; i < f->width; i++){
        /* load byte from program memory */
        data = pgm_read_byte(c_p + i);
        
        pos.y = p.y;

        /* for each bit */
        for(j = 0; j < f->height; j++){
            
            /* if bit set */
            if(data & (1 << j))
                drawPx(pos.x, pos.y);
            
            pos.y++;
        }

        pos.x++;
    }
}

void glcdDrawText(const char *text, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    xy_point pos;
    uint8_t i;
    
    pos.x = p.x;
    pos.y = p.y;
    i = 0;

    while(text[i] != '\0'){
        if(text[i] == '\n'){
            pos.y += f->lineSpacing;
        
        }else{
            glcdDrawChar(text[i], pos, f, drawPx); 
        
            pos.x += f->charSpacing;
        }

        i++;
    }
}

/* no more than X_LEN/charSpacing character and Y_LEN/lineSpacing newlines */
void glcdDrawTextPgm(PGM_P text, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y))
{
    xy_point pos;
    PGM_P txt_p;
    uint8_t i;
    char buff[32]; // TODO adjust

    pos.x = p.x;
    pos.y = p.y;
    i = 0;

    /* load string from PGM */
    strcpy_P(buff, text);

    while(buff[i] != '\0'){
        if(buff[i] == '\n'){
            pos.y += f->lineSpacing;
        
        }else{
            glcdDrawChar(buff[i], pos, f, drawPx); 
        
            pos.x += f->charSpacing;
        }

        i++;
    }
}
