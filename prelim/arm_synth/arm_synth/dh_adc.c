#include "sam.h"
#include "dh_adc.h"
void setupADC(void) {
  
	/* Enable the APB clock for the ADC. */
	PM->APBCMASK.reg |= PM_APBCMASK_ADC;
  
	/* Enable GCLK0 (48Mhz) for the ADC */
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |
                      GCLK_CLKCTRL_GEN_GCLK0 |
                      GCLK_CLKCTRL_ID_ADC;
  
	/* Wait for bus synchronization. */
	while (GCLK->STATUS.bit.SYNCBUSY) {};

	/* Reset the ADC. */
	ADC->CTRLA.bit.ENABLE = 0;
	while (ADC->STATUS.bit.SYNCBUSY) {};
	ADC->CTRLA.bit.SWRST = 1;
	while (ADC->CTRLA.bit.SWRST || ADC->STATUS.bit.SYNCBUSY) {};

  /* Load factory calibration. */
	uint32_t bias = (*((uint32_t *) ADC_FUSES_BIASCAL_ADDR) & ADC_FUSES_BIASCAL_Msk) >> ADC_FUSES_BIASCAL_Pos;
	uint32_t linearity = (*((uint32_t *) ADC_FUSES_LINEARITY_0_ADDR) & ADC_FUSES_LINEARITY_0_Msk) >> ADC_FUSES_LINEARITY_0_Pos;
	linearity |= ((*((uint32_t *) ADC_FUSES_LINEARITY_1_ADDR) & ADC_FUSES_LINEARITY_1_Msk) >> ADC_FUSES_LINEARITY_1_Pos) << 5;
  
  /* Wait for bus synchronization. */
	while (ADC->STATUS.bit.SYNCBUSY) {};
  
  /* Write the calibration data. */
	ADC->CALIB.reg = ADC_CALIB_BIAS_CAL(bias) | ADC_CALIB_LINEARITY_CAL(linearity);
  
  /* Wait for bus synchronization. */
	while (ADC->STATUS.bit.SYNCBUSY) {};
  
  /* Use the internal VCC reference. This is 1/2 of what's on VCCA.
     since VCCA is typically 3.3v, this is 1.65v.
  */
	ADC->REFCTRL.reg = ADC_REFCTRL_REFSEL_INTVCC1;
  

	ADC->AVGCTRL.reg = DH_ADC_NUM_SAMPS | DH_ADC_ADJRES; //tweak later for N/R if it's an issue
  
	// prescale div and resolution
	ADC->CTRLB.reg = DH_ADC_PRESCALER_DIV |
					DH_ADC_RESSEL;

  /* Configure the input parameters.
  
     - GAIN_DIV2 means that the input voltage is halved. This is important
       because the voltage reference is 1/2 of VCCA. So if you want to
       measure 0-3.3v, you need to halve the input as well.
  
     - MUXNEG_GND means that the ADC should compare the input value to GND.
  
     - MUXPOS_PINx means that the ADC should read from pin x
	*/
	ADC->INPUTCTRL.reg = 
		ADC_INPUTCTRL_GAIN_DIV2 |
		ADC_INPUTCTRL_MUXNEG_GND |
		ADC_INPUTCTRL_MUXPOS_PIN0; //Will be updated regularly
	  
	//set sample time to change input impedance
	ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(1);
  
	/* Set  __ as an input pin. */
	PORT->Group[0].DIRCLR.reg |= PORT_PA02;
  
	/* Enable the peripheral multiplexer for pins. */
	PORT->Group[0].PINCFG[2].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[2 >> 1].reg = PORT_PMUX_PMUXE_B;
	PORT->Group[0].DIRCLR.reg |= PORT_PA02;
	
	PORT->Group[0].PINCFG[3].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[3 >> 1].reg = PORT_PMUX_PMUXO_B;
	PORT->Group[0].DIRCLR.reg |= PORT_PA03;
	
	PORT->Group[0].PINCFG[4].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[4 >> 1].reg = PORT_PMUX_PMUXE_B;
	PORT->Group[0].DIRCLR.reg |= PORT_PA04;
	
	PORT->Group[0].PINCFG[5].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[5 >> 1].reg = PORT_PMUX_PMUXO_B;
	PORT->Group[0].DIRCLR.reg |= PORT_PA05;
	
	PORT->Group[0].PINCFG[6].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[6 >> 1].reg = PORT_PMUX_PMUXE_B;
	PORT->Group[0].DIRCLR.reg |= PORT_PA06;
	
	PORT->Group[0].PINCFG[7].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[7 >> 1].reg = PORT_PMUX_PMUXO_B;
	PORT->Group[0].DIRCLR.reg |= PORT_PA07;
	
	//
	
	PORT->Group[0].PINCFG[10].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[10 >> 1].reg = PORT_PMUX_PMUXE_B;
	PORT->Group[0].DIRCLR.reg |= PORT_PA10;
	
	PORT->Group[0].PINCFG[11].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[11 >> 1].reg = PORT_PMUX_PMUXO_B;
	PORT->Group[0].DIRCLR.reg |= PORT_PA11;
	
	PORT->Group[0].PINCFG[14].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[14 >> 1].reg = PORT_PMUX_PMUXE_B;
	PORT->Group[0].DIRCLR.reg |= PORT_PA14;
	
	PORT->Group[0].PINCFG[15].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[15 >> 1].reg = PORT_PMUX_PMUXO_B;
	PORT->Group[0].DIRCLR.reg |= PORT_PA15;
  
	/* Wait for bus synchronization. */
	while (ADC->STATUS.bit.SYNCBUSY) {};
  
	/* Enable the ADC. */
	ADC->CTRLA.bit.ENABLE = 1;
	
	
	// run a conversion and throw it away to buffer the output / prevent bad data

	while (ADC->STATUS.bit.SYNCBUSY) {};
		
	/* Start the ADC using a software trigger. */
	ADC->SWTRIG.bit.START = 1;
		
	/* Wait for the result ready flag to be set. */
	while (ADC->INTFLAG.bit.RESRDY == 0);
		
	/* Clear the flag. */
	ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
};

void setADCInput(uint8_t pinNum) {
	while (ADC->STATUS.bit.SYNCBUSY) {};
	ADC->INPUTCTRL.reg =
		ADC_INPUTCTRL_GAIN_DIV2 |
		ADC_INPUTCTRL_MUXNEG_GND |
		ADC_INPUTCTRL_MUXPOS(pinNum);
};

//func to read ADC
uint32_t readADC (void) {
	/* Wait for bus synchronization. */
	while (ADC->STATUS.bit.SYNCBUSY) {};
	
	/* Start the ADC using a software trigger. */
	ADC->SWTRIG.bit.START = 1;
	
	/* Wait for the result ready flag to be set. */
	while (ADC->INTFLAG.bit.RESRDY == 0);
	
	/* Clear the flag. */
	ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
	
	/* Read the value. */
	return ADC->RESULT.reg;
}