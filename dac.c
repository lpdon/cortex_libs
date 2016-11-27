#include "dac.h"

void DAC_init(){	 	
	//clock config
	LPC_SC->PCLKSEL0 |= (1 << 22); // PCLK/1 to DAC

	//pin config
	LPC_PINCON->PINSEL1 |= (2 << 20); //DAC function to P0.26
	LPC_PINCON->PINMODE1 |= (2 << 20); //without pull-up/pull-down	
};

void DAC_write(uint16_t value){
	LPC_DAC->DACR = ((value & 0x3FF) << 6);
};