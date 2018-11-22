#ifndef RAND_H
#define RAND_H

/**
 * Shifts the LSB of in to the LFSR and returns the MSB
 *
 * @param: in, lsb gets shifted in lfsr
 * @return: lsb holds random bit of lfsr
 * @globals: lfsr
 */
uint8_t rand_shift(uint8_t in);

/**
 * Feeds the random LSB to the LFSR
 *
 * @param: in, lsb gets shifted in lfsr
 */
void rand_feed(uint8_t in);

/**
 * Returns one random bit in LSB
 *
 * @return: lsb hold random bit of lfsr
 */
uint8_t rand1(void);

/**
 * Generates random 8bit nuber
 *
 * @return: a 8bit random number
 */
uint8_t rand8(void);

/**
 * Generates random 16bit number
 *
 * @return: a 16bit random number
 */
uint16_t rand16(void);

#endif
