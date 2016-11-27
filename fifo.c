#include "fifo.h"

uint8_t fifo_in(FIFO_BUFFER* fifo, uint8_t byte){
	if(fifo->read_index == fifo->write_index + 1 || 
		(fifo->read_index == 0 && fifo->write_index + 1 == FIFO_BUFFER_SIZE)){
		return 0;
	}	   		
	
	fifo->buffer[fifo->write_index++] = byte;

	if(fifo->write_index >= FIFO_BUFFER_SIZE)
		fifo->write_index = 0;		
	
	return 1;
};

uint8_t fifo_out(FIFO_BUFFER* fifo, uint8_t* byte){	
	if(fifo->write_index == fifo->read_index)
		return 0;

	*byte = fifo->buffer[fifo->read_index++];
	
	if(fifo->read_index >= FIFO_BUFFER_SIZE)
		fifo->read_index = 0;	

	return 1;
};
