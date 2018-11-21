/**
 * author:	Andreas Hagmann <ahagmann@ecs.tuwien.ac.at>
 * date:	21.01.2012
 */

#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

typedef enum {
	SPI_PRESCALER_128	= 3,
	SPI_PRESCALER_4		= 0,
	SPI_PRESCALER_16	= 1,
} spi_prescaler_t;

/**
 * Initializes hardware for SPI
 */
void spiInit(void);

/**
 * Sends data over SPI
 *
 * @param: data, byte that has to be send
 */
void spiSend(uint8_t data);

/**
 * Receives data over SPI
 *
 * @return: byte that has been received
 */
uint8_t spiReceive(void);

/**
 * Sets prescaler of SPI
 */
void spiSetPrescaler(spi_prescaler_t prescaler);

#endif
