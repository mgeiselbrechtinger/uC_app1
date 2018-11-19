#include    <avr/io.h>
#include    <avr/pgmspace.h>
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

    glcdDrawLine(p_start, p_end, drawPx);

    for(p_start.y += 1; p_start.y < p_end.y; p_start.y++){
        drawPx(p_start.x, p_start.y);
        
        if(p_start.x != p_end.x)
            drawPx(p_end.x, p_start.y);
    }

    if(p_start.y == p_end.y)
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
    //int8_t r, x, y, err;
    //r = radius;
    //x = -r;             /* start with second quadrant */
    //y = 0; 
    //err = 2 - 2*radius; /* error of first step */

    //do{
    //    drawPx(c.x-x, c.y+y);   /*   I. Quadrant */
    //    drawPx(c.x-y, c.y-x);   /*  II. Quadrant */
    //    drawPx(c.x+x, c.y-y);   /* III. Quadrant */
    //    drawPx(c.x+y, c.y+x);   /*  IV. Quadrant */
    //
    //    r = err;

    //    /* update y */       
    //    if(r <= y){
    //        y++;
    //        err += y*2 + 1;
    //    }

    //    /* update x */
    //    if(r > x || err > y){
    //        x++;
    //        err += x*2 + 1;
    //    }

    //while(x < 0);
}

/* bresenham algorithm */
void glcdDrawEllipse(const xy_point c, const uint8_t radiusX, const uint8_t radiusY, void (*drawPx)(const uint8_t x, const uint8_t y))
{
//    int8_t radiusY_1, x0, x1, y0, y1;
//    int16_t dx, dy, err, e2; 
//    
//    radiusY_1 = radiusY & 1;
//    dx = 4*(1-radiusX)*b*b;
//    dy = 4*(radiusY_1 + 1)*a*a;
//    err = dx + dy + radiusY_1*a*a;
//
//    x0 = c - radiusX;
//    y0 = c - radiusY;
//    x1 = c + radiusX;
//    y1 = c + radiusY;
//    
//    /* starting point */
//    y0 *= (radiusY + 1)/2;
//    y1 = y0 - radiusY_1;
//
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

    glcdDrawLine(p_start, p_end, drawPx);
    
    p_tmp.x = p_end.x;

    for(p_start.y += 1; p_start.y < p_end.y; p_start.y++){
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
    for(i = 0; i < f->width; i ++){
        /* load byte from program memory */
        data = pgm_read_byte(c_p + i);
        
        pos.x += i;
        /* for each bit */
        for(j = 0; j < f->height; j++){
            pos.y += j;
            
            /* if bit set */
            if(data & (1 << j))
                drawPx(pos.x, pos.y);

        }
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
    memcpy_P(&txt_p, &text, sizeof (PGM_P));
    strcpy_P(buff, txt_p);

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
