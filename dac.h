#ifndef DAC_H
#define DAC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <LPC17xx.H>                          /* LPC17xx definitions         */

void DAC_init();
void DAC_write(uint16_t value); //10 bits digital value 0-1023

#endif
