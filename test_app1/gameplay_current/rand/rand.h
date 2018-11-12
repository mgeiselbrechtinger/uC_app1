#ifndef _RAND_H_
#define _RAND_H_


/**
 * Shifts the LSB of in to the LFSR and returns the MSB
 */
uint8_t rand_shift(uint8_t in);

/**
 * Feeds the random LSB to the LFSR
 */
void rand_feed(uint8_t in);

/**
 * Returns one random bit in LSB
 */
uint8_t rand1();

/**
 * Generates random 16bit number
 */
uint16_t rand16();

#endif
