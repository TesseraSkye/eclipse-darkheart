/*
 * arm_synth.c
 *
 * Created: 11/9/2020 8:54:38 PM
 * Author : Tess
 */ 


#include "sam.h"


int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
    
    //user init
    setupADC();
    //setupI2C();

    /* Replace with your application code */
    while (1) 
    {
		uint16_t res = readADC();
    }
}
