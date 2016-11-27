# cortex_libs

Some code used in the past for playing with LPC17xx (Cortex-M3) boards (I used this one: https://developer.mbed.org/media/uploads/wim/landtiger_v2.0_-_manual__v1.1.pdf)

- BMP.h/c: allocates space and loads a .bmp file. 
- dac-h/c: how to init the DAC and write a value.
- fir.asm: assembly implementation of a FIR filter. Parameters are according to Cortex-M3 conventions (registers, stack, etc):
	- fir(s_in, size_i, coef, size_coef, out);
		- s_in: samples array address
		- size_i: samples size
		- coef: coefs array address
		- size_coef: coefs size
		- out: output array address
- sd.h/c: interface for SD cards. As it uses SPI calls, it is necessary to have your own SPI driver. 
- fifo.h/c: simple FIFO implementation
- uart.c/h: FIFO'ed UART. Uses the fifo files. 
- wav.c/h: reads and plays a WAV file from the SD card. Uses the fatfs library (R0.10a) and SD interface.