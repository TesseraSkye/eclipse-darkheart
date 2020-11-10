#pragma once

#define ECLIPSE_ADC_NUM_SAMPS ADC_AVGCTRL_SAMPLENUM_8		// number of samps for better averaging (MAKE SURE TO UPDATE ADJRES) {Power of 2!}
#define ECLIPSE_ADC_ADJRES ADC_AVGCTRL_ADJRES(3)				// compensate for the new sample number
#define ECLIPSE_ADC_RESSEL ADC_CTRLB_RESSEL_16BIT			// has to be 16 bit if multiple samps are being taken
#define ECLIPSE_ADC_PRESCALER_DIV ADC_CTRLB_PRESCALER_DIV4	// Prescaler on clock

void setupADC(void);

void setADCInput(uint8_t);

uint32_t readADC (void);
