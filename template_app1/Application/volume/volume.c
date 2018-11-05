#include    <avr/io.h>
#include    <stdint.h>

uint8_t vol_scale(uint8_t data_lo8, uint8_t data_hi8)
{
    uint8_t data = (data_hi8 << 6) | (data_lo8 >> 2);
    uint16_t data = (data_hi8 << 8) | data_lo8;

    // y in [0,1], y = 1-(1-x)^4, x in [0,1]
    // y in [0,1023],  1023y = 1 - (1 - x/1023)^4, x in [0,1023]        

}
