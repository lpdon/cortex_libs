#include "sd.h"

extern unsigned char  clock_100ms;

void delay_100ms(){
	clock_100ms = 0;
	while(1){
		if(clock_100ms){
			clock_100ms = 0;
			return;
		}
	}	
}

uint8_t sd_detect(){
	LPC_GPIO3->FIODIR   &=  ~(1<<25);            /* P3.25 is INPUT             */
	if(LPC_GPIO3->FIOPIN & (1<<25))
		return 0;
	else{
		LPC_GPIO3->FIODIR   |=  (1<<26);    
		LPC_GPIO3->FIOPIN   |= (1<<26);            
		return 1;
	}	
}

uint8_t sd_send_byte(uint8_t data){
	return spi_send(data);
}

void sd_send_command(uint8_t command, uint32_t param){
	uint8_t send[6];
	uint8_t i;

	send[0] = command | 0x40;
	send[1] = param >> 24;
	send[2] = param >> 16;
	send[3] = param >> 8;
	send[4] = param;
	send[5] = (sd_crc(send, 5, 0) << 1) | 1; 

	for(i = 0; i < 6; i++)
		sd_send_byte(send[i]);
}

uint8_t sd_read_resp(void){
	uint8_t v = 0xFF, i = 0;

	do{                 
		v = sd_send_byte(0xFF); 
	}while(i++ < 128 && (v == 0xFF));

	return v; 
}

uint8_t sd_command_response(uint8_t command, uint32_t param){
	uint8_t ret;          

	sd_send_byte(0xFF); 
	sd_send_command(command, param); 
	ret = sd_read_resp();

	return ret; 
}

uint8_t sd_crc(uint8_t* chr, uint8_t cnt, uint8_t crc){
	uint8_t i, a; 
	uint8_t data; 
	
	for(a = 0; a < cnt; a++){           
		data = chr[a];           
		for(i = 0; i < 8; i++){                
			crc <<= 1; 
			if( (data & 0x80) ^ (crc & 0x80) ) {crc ^= 0x09;}                
			data <<= 1; 
	  } 
	}      
	return crc & 0x7F; 
}

uint8_t sd_init(void){
	uint8_t i, v, tries = 0; 
	uint8_t SD; 

	spi_init(); // init SPI
	spi_hi_speed(false); // slow clock       
	spi_ss(true); // CS inactive (active low)

	for(v = 0; v < 200; v++) { 
  		sd_send_byte(0xFF);     //lots of clocks to give card time to init 
	} 
	spi_ss(false); // CS active (active low)

	v = sd_command_response(0, 0); 
	if(v != 1){return 2;} 

	delay_100ms();
	v = sd_command_response(8, 0x000001AA); 
	delay_100ms();

	while(v != 0x00){
		v = sd_command_response(55, 0);   
		delay_100ms();
		sd_command_response(41, (1 << 30));   
		delay_100ms(); 
	}

	if(v){return 3;} 

	v = sd_command_response(16, 512);          //set sector size 
	if(v){return 4;} 
	delay_100ms();
	v = sd_command_response(59, 0);          //crc off 
	if(v){return 5;} 
	delay_100ms();

	// now set the sd card up for full speed 
	spi_hi_speed(true);                     
	return 1; 
}

uint8_t sd_read_block(uint32_t sec){
	uint8_t v;

	v = sd_command_response(17, sec);  

	if(v == 0x00){ //valid sector
		v = sd_send_byte(0xFF);
		while(v == 0xFF){
			v = sd_send_byte(0xFF);
		}

		if(v == 0xFE)
			return true;
	}

	return false;
}

void sd_read_sector_buffer(uint32_t sec, uint8_t* buffer){
	uint16_t i = 0;

	spi_ss(false);

	if(sd_read_block(sec)){
		for(i = 0; i < SD_SECTOR_SIZE; i++)
			buffer[i] = sd_send_byte(0xFF);
	}

	//read the 2 CRC bytes, even if they are useless
	sd_send_byte(0xFF);
	sd_send_byte(0xFF);
	spi_ss(true);
}
