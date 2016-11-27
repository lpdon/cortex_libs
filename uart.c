#include "uart.h"

#define UARTFIFO		2 //8 chars
#define PCUART0 		(1 << 3)
#define PCLK_UART0 		(1 << 6)
#define DLAB 			(1 << 7)
#define DIVADDVAL 		(1 << 0)
#define	MULVAL			(2 << 4)
#define TXD0			(1 << 4)
#define RXD0			(1 << 6)
#define P002MODE		(2 << 4)
#define P003MODE		(2 << 6)
#define FIFOENABLE		(1 << 0)
#define RXTRIGGERLEVEL	(UARTFIFO << 6) //8 chars before interrupt
#define RBRIE			(1 << 0)
#define THREIE			(1 << 1)
#define RLS				0x03
#define RDA				0x02
#define CTI				0x06
#define THREI			0x01
#define RXFIFORESET		(1 << 1)
#define RDR				(1 << 0)
#define THRE			(1 << 5)
#define INTID 			(LPC_UART0->IIR >> 1) & 0x03
#define WLS				(3 << 0)

int16_t i;

FIFO_BUFFER rx_buffer;
FIFO_BUFFER tx_buffer;

uint8_t tx_byte;

void UART0_IRQHandler(void){

	switch(INTID){
		case RDA:
			/*for(i = 0; i < UARTFIFO; i++){
				fifo_in(&rx_buffer, LPC_UART0->RBR); 				
			}
			LPC_UART0->FCR |= RXFIFORESET;*/
			while(LPC_UART0->LSR & RDR)
				fifo_in(&rx_buffer, LPC_UART0->RBR);
			break;
		case THREI:
			if(fifo_out(&tx_buffer, &tx_byte))
				LPC_UART0->THR = tx_byte;	

			break;	
		default:
			break;				 
	}
}

// Baud-rate: 115200bps (FR_est = 1.507, DL_est = 36, DIVADDVAL = 1, MULVAL = 2)
uint8_t uart_init(){
	rx_buffer.read_index = 0;
	rx_buffer.write_index = 0;
	tx_buffer.read_index = 0;
	tx_buffer.write_index = 0;

	//power
	LPC_SC->PCONP 		|= PCUART0;

	//clock
   	LPC_SC->PCLKSEL0 	|= PCLK_UART0;

	//baud-rate
	LPC_UART0->LCR 		|= DLAB; 
	LPC_UART0->DLL 		= 36;
	LPC_UART0->DLM 		= 0;
	LPC_UART0->FDR 		= (DIVADDVAL | MULVAL);

	//word length
	LPC_UART0->LCR		|= WLS;

	//FIFO
	LPC_UART0->FCR	   	|= RXTRIGGERLEVEL;

	//pins
	LPC_PINCON->PINSEL0	|= (TXD0 | RXD0);
	//LPC_PINCON->PINMODE0 |=	(P002MODE | P003MODE);

	//interrupts
	LPC_UART0->LCR 		&= ~DLAB; 
	LPC_UART0->IER		|= (RBRIE | THREIE);

	NVIC_EnableIRQ(UART0_IRQn);
}

uint8_t uart_write(const uint8_t byte){
	//if(fifo_in(&tx_buffer, byte)){
		//return UART_OK;
	if(LPC_UART0->LSR & THRE){
		LPC_UART0->THR = byte;
		return UART_OK;	
	}
	else{
		if(fifo_in(&tx_buffer, byte))
			return UART_OK;
	}

	return UART_ERROR;
}

uint8_t uart_read(uint8_t* byte){
	if(fifo_out(&rx_buffer, byte))
		return UART_OK;

	return UART_ERROR;
}

uint8_t uart_status(){

}