#include "wav.h"
#include "fatfs/ff.h"
#include "dac.h"
#include "fifo.h"

#define BUFFER_SIZE 256

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

extern unsigned short AD_last; 

enum states {
	READ_SAMPLE,
	PLAY_SAMPLE,
} current_state = READ_SAMPLE;

FIFO_BUFFER fifo;
uint8_t load_buffer[BUFFER_SIZE];
uint8_t lower_dac_sample;
uint8_t upper_dac_sample;
uint16_t dac_sample;

//TIMER0 config
void timer_init(){
	LPC_SC->PCONP |= (1 << 1); //turn on PCTIM0	 
		
	//clock config
	LPC_SC->PCLKSEL0 |= (1 << 2); // PCLK/1 to TIMER0

	//interrupt config
	LPC_TIM0->MCR |= (1 << 0); //interrupt on MR0
	LPC_TIM0->MCR |= (1 << 1); //auto-reset on MR0

	//init counter
	LPC_TIM0->TCR |= (1 << 0); //timer is enabled to count

	NVIC_EnableIRQ(TIMER0_IRQn);
};

//TIMER0 interrupt handler
void TIMER0_IRQHandler(void){
	LPC_TIM0->IR |= (1 << 0); //clear interrupt flag for MR0

	if(current_state == PLAY_SAMPLE){
		fifo_out(&fifo, &lower_dac_sample);
		fifo_out(&fifo, &upper_dac_sample);
		dac_sample = (upper_dac_sample << 8) | lower_dac_sample; 
	
		//DAC_write((dac_sample * AD_last) >> 12);
		DAC_write(dac_sample);
	}
};

void timer_set_freq(uint32_t freq){
	LPC_TIM0->MR0 = (SystemFrequency/freq);
};

uint8_t wav_loadheader(const char* path, WAVHEADER* header){
	FIL fil;	
	FRESULT fr; 
	UINT br = 0;	

	fr = f_open(&fil, path, FA_READ);

	if(fr)
		return 0;

	f_read(&fil, header, sizeof(WAVHEADER), &br);
	f_close(&fil);

	if(br <= 0)
		return 0;
	
	return 1;
}

uint8_t wav_playfile(const char* path, WAVHEADER* header){
	FIL fil;
	FRESULT fr;
	UINT br = 0;
	uint32_t total_bytes_read = 0;
	uint32_t sample;
	uint32_t i;
	uint8_t* p_buffer;
	
	fr = f_open(&fil, path, FA_READ);

	if(fr)
		return 0;

	f_lseek(&fil, sizeof(WAVHEADER));
	timer_set_freq(header->sampleRate);
	timer_init();

	fifo.read_index = 0;
	fifo.write_index = 0;
	current_state = READ_SAMPLE;

	while(total_bytes_read < header->subchunck2Size){
		fr = f_read(&fil, load_buffer, BUFFER_SIZE, &br);
		if(fr == FR_OK){
			total_bytes_read += BUFFER_SIZE;
	
			p_buffer = &load_buffer[0];
			while(br){
				sample = 0;
				for(i = 0; i < (header->bitsPerSample >> 3); i++){
					sample |= (p_buffer[i] << (i << 3));
				}

				//volume
				sample = (sample * AD_last) >> 12;

				//sample is signed, add offset
				if(header->bitsPerSample > 8){
					sample += (1 << (header->bitsPerSample - 1)) - 1;
				}
	
				if(header->bitsPerSample > 10){
					//sample += 0x7FFF;
					sample = sample >> (header->bitsPerSample - 10);
				}
	
				while(!fifo_in(&fifo, (sample & 0xFF)));
				while(!fifo_in(&fifo, (sample >> 8) & 0x03));
				p_buffer += header->blockAlign;
				br -= header->blockAlign;
			}
			current_state = PLAY_SAMPLE;
		}
	}

	f_close(&fil);
	return 1;
}
