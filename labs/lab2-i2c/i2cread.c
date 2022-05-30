#include <inttypes.h>

#include "si5351a.h"

#define I2C_WRITE 0b11000000            // I2C address for writing to the Si5351A
#define I2C_READ  0b11000001            // I2C address for reading to the Si5351A


void setupPLL(uint8_t pll, uint8_t mult, uint32_t num, uint32_t denom)
{
        uint32_t P1;                                    // PLL config register P1
        uint32_t P2;                                    // PLL config register P2
        uint32_t P3;                                    // PLL config register P3

        P1 = (uint32_t)(128 * ((float)num / (float)denom));
        P1 = (uint32_t)(128 * (uint32_t)(mult) + P1 - 512);
        P2 = (uint32_t)(128 * ((float)num / (float)denom));
        P2 = (uint32_t)(128 * num - denom * P2);
        P3 = denom;
        i2c_write(pll + 2, (P1 & 0x00030000) >> 16);
        i2c_write(pll + 3, (P1 & 0x0000FF00) >> 8);
        i2c_write(pll + 4, (P1 & 0x000000FF));
        i2c_write(pll + 5, ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
        i2c_write(pll + 6, (P2 & 0x0000FF00) >> 8);
        i2c_write(pll + 7, (P2 & 0x000000FF));
}
void setupMultisynth(uint8_t synth, uint32_t divider, uint8_t rDiv)
{
        uint32_t P1;                                    // Synth config register P1
        uint32_t P2;                                    // Synth config register P2
        uint32_t P3;                                    // Synth config register P3

        P1 = 128 * divider - 512;
        P2 = 0;                                                 // P2 = 0, P3 = 1 forces an integer value for the divider
        P3 = 1;

        i2c_write(synth + 0,   (P3 & 0x0000FF00) >> 8);
        i2c_write(synth + 1,   (P3 & 0x000000FF));
        i2c_write(synth + 2,   ((P1 & 0x00030000) >> 16) | rDiv);
        i2c_write(synth + 3,   (P1 & 0x0000FF00) >> 8);
        i2c_write(synth + 4,   (P1 & 0x000000FF));
        i2c_write(synth + 5,   ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
        i2c_write(synth + 6,   (P2 & 0x0000FF00) >> 8);
        i2c_write(synth + 7,   (P2 & 0x000000FF));
}

void si5351aSetFrequency(uint32_t frequency)
{
        uint32_t pllFreq;
        uint32_t xtalFreq = XTAL_FREQ;
        uint32_t l;
        float f;
        uint8_t mult;
        uint32_t num;
        uint32_t denom;
        uint32_t divider;

        //      i2c_init();                                             

        divider = 900000000 / frequency;
                                                                        
        if (divider % 2) divider--;             

        pllFreq = divider * frequency;  

        mult = pllFreq / xtalFreq;              
        l = pllFreq % xtalFreq;                 
        f = l;                                                  
        f *= 1048575;                                   
        f /= xtalFreq;                                  
        num = f;                                                
        denom = 1048575;                                
  
        setupPLL(SI_SYNTH_PLL_A, mult, num, denom);

                                                                
        setupMultisynth(SI_SYNTH_MS_0, divider, SI_R_DIV_1); // add another one

        setupMultisynth(SI_SYNTH_MS_1, divider, SI_R_DIV_1); // new one
                
        i2c_write(SI_PLL_RESET, 0xA0);  

  			uint32_t p90;
        i2c_write(SI_CLK0_CONTROL, 0x4F | SI_CLK_SRC_PLL_A);    
          p90 = 0.5 + 1.0/(4.0 * frequency) * (4.0 * pllFreq);
          i2c_write(166, p90);// change the clock phase 
        // got rid of i2c exit                                          
}
