#ifndef HAL_GLCD_H
#define HAL_GLCD_H

uint8_t halGlcdInit(void);

uint8_t halGlcdSetAddress(const uint8_t xCol, const uint8_t yPage);

uint8_t halGlcdWriteData(const uint8_t data);

uint8_t halGlcdReadData(void);

uint8_t halGlcdFillScreen(uint8_t pattern);

uint8_t halGlcdSetYShift(uint8_t yshift);

uint8_t halGlcdGetYShift(void);

#endif
