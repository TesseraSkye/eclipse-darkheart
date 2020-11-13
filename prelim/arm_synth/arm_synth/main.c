/*
 * arm_synth.c
 *
 * Created: 11/9/2020 8:54:38 PM
 * Author : Tess
 */ 

#include "sam.h"
#include "dh_synth.h"


int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
    
    //user init
    setupADC();
    //setupI2C();

    while (1) {
		
		uint16_t adcRes[DH_CONTROL_NUM] = {0,0,0,0};
		//for(uint8_t i = 0; i<DH_CONTROL_NUM; i++) {
		setADCInput(2);
		adcRes[0] = readADC();
		//}
    }
}
