#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <LPC17xx.H>                          /* LPC17xx definitions         */
#include "fifo.h"

#define UART_OK 1
#define UART_ERROR 0

uint8_t uart_init();
uint8_t uart_write(const uint8_t byte);
uint8_t uart_read(uint8_t* byte);
uint8_t uart_status();

#endif