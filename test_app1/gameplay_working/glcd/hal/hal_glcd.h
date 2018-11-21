#ifndef HAL_GLCD_H
#define HAL_GLCD_H

/**
 * Resets and initializes the controllers
 */
uint8_t halGlcdInit(void);

/**
 * Sets address of RAM
 *
 * @param: xCol, address of column
 * @param: yPage, address of page
 * @return: allways SUCCESS 
 * @globals: address
 */
uint8_t halGlcdSetAddress(const uint8_t xCol, const uint8_t yPage);

/**
 * Writes data to RAM
 *
 * @param: data, to be written
 * @return: allways SUCCESS
 * @globals: address
 */
uint8_t halGlcdWriteData(const uint8_t data);

/**
 * Reads data of RAM
 *
 * @return: data, that has been read
 * @globals: address
 */
uint8_t halGlcdReadData(void);

/**
 * Fills screen with pattern
 *
 * @param: patter, to be written accross screen
 * @return: allways SUCCESS
 */
uint8_t halGlcdFillScreen(uint8_t pattern);

/**
 * Set y address of RAM
 *
 * @param: yshift, address of top line
 * @return: allways SUCCESS
 * @globals: address
 */
uint8_t halGlcdSetYShift(uint8_t yshift);

/**
 * Get y address of RAM
 *
 * @return current yshift of RAM
 * @globals address
 */ 
uint8_t halGlcdGetYShift(void);

#endif
