#include    <avr/io.h>
#include    <stdint.h>
#include    <util/atomic.h>

#define POLYH   (0x80)
#define POLYL   (0xE3)

/* linear feedback shift register state */
static uint16_t lfsr = 1;

/* implementations */

/**
 * Shifts the LSB of in to the LFSR and returns the MSB
 *
 * @param: in, lsb gets shifted in lfsr
 * @return: lsb holds random bit of lfsr
 * @globals: lfsr
 */
uint8_t rand_shift(uint8_t in)
{
    uint8_t out;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        asm volatile ("\n"
            "mov    r16, %A[lfsr]"      "\n\t"
            "andi   r16, 0x01"          "\n\t"
            "mov    %[out], r16"        "\n\t"
            "lsr    %B[lfsr]"           "\n\t"
            "ror    %A[lfsr]"           "\n\t"
            "bst    %[in], 0"           "\n\t"
            "bld    %B[lfsr], 7"        "\n\t"
            "sbrs   r16, 0"             "\n\t"
            "rjmp   end%="              "\n\t"
            "ldi    r16, %[polyl]"      "\n\t"
            "eor    %A[lfsr], r16"      "\n\t"
            "ldi    r16, %[polyh]"      "\n\t"
            "eor    %B[lfsr], r16"      "\n\t"
            "end%=:"                    "\n\t"
            : [lfsr] "=&r" (lfsr), [out] "=&r" (out)
            : "0" (lfsr), [in] "r" (in), [polyh] "M" (POLYH), [polyl] "M" (POLYL)
            : "r16" 
        );
    }

    return out;
}

/**
 * Feeds the random LSB to the LFSR
 *
 * @param: in, lsb gets shifted in lfsr
 */
void rand_feed(uint8_t in)
{
    rand_shift(in);
}

/**
 * Returns one random bit in LSB
 *
 * @return: lsb hold random bit of lfsr
 */
uint8_t rand1(void)
{
    return rand_shift(0);
}

/**
 * Generates random 8bit nuber
 *
 * @return: a 8bit random number
 */
uint8_t rand8(void)
{
    uint8_t i, num = 0;
    
    for(i = 0; i < 7; i++){
        num |= (rand_shift(0) << i);
    }

    return num;
}

/**
 * Generates random 16bit number
 *
 * @return: a 16bit random number
 */
uint16_t rand16(void)
{
    return (rand8() << 8) | rand8(); 
}   
