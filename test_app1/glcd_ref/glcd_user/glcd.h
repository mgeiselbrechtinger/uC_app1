#ifndef GLCD_H
#define GLCD_H

#include    "../font/font.h"

void glcdInit(void);

void glcdSetPixel(const uint8_t x, const uint8_t y);

void glcdClearPixel(const uint8_t x, const uint8_t y);

void glcdInvertPixel(const uint8_t x, const uint8_t y);

typedef struct xy_point_t {
    uint8_t x, y;
} xy_point;

void glcdDrawLine(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawRect(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdFillScreen(uint8_t pattern);

void glcdSetYShift(uint8_t yshift);

uint8_t glcdGetYShift(void);

void glcdDrawCircle(const xy_point c, const uint8_t radius, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawEllipse(const xy_point c, const uint8_t radiusX, const uint8_t radiusY, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawVertical(const uint8_t x, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawHorizontal(const uint8_t y, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdFillRect(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawChar(const char c, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawText(const char *text, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y));

void glcdDrawTextPgm(PGM_P text, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y));

#endif
