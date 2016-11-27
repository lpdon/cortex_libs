#ifndef FIFO_H
#define FIFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <LPC17xx.H>                          /* LPC17xx definitions         */

#define FIFO_BUFFER_SIZE 32

typedef struct{
	uint8_t buffer[FIFO_BUFFER_SIZE];
	uint8_t read_index;
	uint8_t write_index;
}FIFO_BUFFER;

uint8_t fifo_in(FIFO_BUFFER* fifo, uint8_t byte);
uint8_t fifo_out(FIFO_BUFFER* fifo, uint8_t* byte);

#endif
