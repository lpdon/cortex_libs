#ifndef SD_H
#define SD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <LPC17xx.H>                          /* LPC17xx definitions         */

#define false 0
#define true 1
#define SD_SECTOR_SIZE 512


uint8_t sd_detect(void);
uint8_t sd_send_byte(uint8_t data);
void sd_send_command(uint8_t command, uint32_t param);
uint8_t sd_read_resp(void);
uint8_t sd_command_response(uint8_t command, uint32_t param);
uint8_t sd_crc(uint8_t* chr, uint8_t cnt, uint8_t crc); 

uint8_t sd_init(void);
uint8_t sd_read_block(uint32_t sec);
void sd_read_sector_buffer(uint32_t sec, uint8_t* buffer);

#endif SD_H
