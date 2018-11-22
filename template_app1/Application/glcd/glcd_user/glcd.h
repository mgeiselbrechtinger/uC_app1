#ifndef GLCD_H
#define GLCD_H

#include    "../font/font.h"

typedef struct xy_point_t {
    uint8_t x, y;
} xy_point;

/**
 * Clears one pixel
 *
 * @param: x, coordinate of pixel
 * @param: y, coordinate of pixel
 */
void glcdInit(void);

/**
 * Inverts one pixel
 *
 * @param: x, coordinate of pixel
 * @param: y, coordinate of pixel
 */
void glcdSetPixel(const uint8_t x, const uint8_t y);

/**
 * Auxiliary function for glcdDrawLine
 * 
 * @param: p1, starting point (upper)
 * @param: p2, ending point (lower)
 * @param: drawPx, pixel manipulation function
 */
void glcdClearPixel(const uint8_t x, const uint8_t y);

/** 
 * Auxiliary function for glcdDrawLine
 *
 * @param: p1, starting point (lower)
 * @param: p2, ending point (upper)
 * @param: drawPx, pixel manipulation function
 */
void glcdInvertPixel(const uint8_t x, const uint8_t y);

/** 
 * Bresenham's line algorithm: en.wikipedia.org/wiki/Bresenham's_line_algorithm
 *
 * @param: p1, starting point
 * @param: p2, ending point
 * @param: drawPx, pixel manipulation function
 */
void glcdDrawLine(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y));

/**
 * Draws a rectangle 
 *
 * @param: p1, starting point
 * @param: p2, ending point
 * @param: drawPx, pixel manipulation function
 */
void glcdDrawRect(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y));

/**
 * Fills screen with pattern
 *
 * @param: patter, to be printed to screen
 */
void glcdFillScreen(uint8_t pattern);

/**
 * Sets the glcds RAM start to match yshift address
 *
 * @param: yshift, gets top line in glcd RAM
 */
void glcdSetYShift(uint8_t yshift);

/**
 * Gets current RAM start address of glcd
 *
 * @return: address of current top line
 */
uint8_t glcdGetYShift(void);

/**
 * Bresenham's circle algorithm: members.chello.at/~easyfilter/bresenham.html
 *
 * @param: c, center point of circle
 * @param: radius, of circle
 * @param drawPx, pixel manipulation function
 */
void glcdDrawCircle(const xy_point c, const uint8_t radius, void (*drawPx)(const uint8_t x, const uint8_t y));

/**
 * Draws Line verticaly over whole display
 *
 * @param: x, coordinate of line
 * @param: drawPx, pixel manipulation function
 */
void glcdDrawVertical(const uint8_t x, void (*drawPx)(const uint8_t x, const uint8_t y));

/**
 * Draws Line horizontaly over whole display
 *
 * @param: y, coordinate of line
 * @param: drawPx, pixel manipulation function
 */
void glcdDrawHorizontal(const uint8_t y, void (*drawPx)(const uint8_t x, const uint8_t y));

/**
 * Draws a filled rectangel 
 *
 * @param: p1, starting point
 * @param: p2, ending point
 * @param: drawPx, pixel manipulation function
 */
void glcdFillRect(const xy_point p1, const xy_point p2, void (*drawPx)(const uint8_t x, const uint8_t y));

/**
 * Draws Character on screen
 *
 * @param: c, ASCII character
 * @param: p, lower left position of character
 * @param: f, font definition
 * @param: drawPx, pixel manipulation function
 */
void glcdDrawChar(const char c, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y));

/**
 * Draws Text on screen
 *
 * @brief: no more than X_LEN/charSpacing character and Y_LEN/lineSpacing newlines
 * @param: text, Null terminated array ofASCII character
 * @param: p, lower left position of first character
 * @param: f, font definition
 * @param: drawPx, pixel manipulation function
 */
void glcdDrawText(const char *text, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y));

/**
 * Draws Text from program memory on screen
 *
 * @brief: no more than X_LEN/charSpacing character and Y_LEN/lineSpacing newlines
 * @param: text, pointer to string in programmemory
 * @param: p, lower left position of first character
 * @param: f, font definition
 * @param: drawPx, pixel manipulation function
 */
void glcdDrawTextPgm(PGM_P text, const xy_point p, const font* f, void (*drawPx)(const uint8_t x, const uint8_t y));

#endif
