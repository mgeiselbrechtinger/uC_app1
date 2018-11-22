#ifndef HAL_WT41_FC_UART
#define HAL_WT41_FC_UART

#include <stdint.h>
#include <util.h>

/**
 * Initializes UART module
 *
 * @HW: UART, TIMER1
 * @param: sndCallback, function called after data has been sent
 * @param: rcvCallback, function called after data has been received
 * @return: allways SUCCESS
 * @globals: rcv_buff, snd_buff, _sndCallback, _rcvCallback
 */
error_t halWT41FcUartInit(void (*sndCallback)(), void (*rcvCallback)(uint8_t));

/**
 * Sends one byte via UART module
 *
 * @param: byte, that gets sent
 * @return: SUCCESS if byte has been sent, ERROR otherwise
 * @globals :snd_buff
 */
error_t halWT41FcUartSend(uint8_t byte);

#endif
