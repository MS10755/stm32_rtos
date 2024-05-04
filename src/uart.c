#include "stm32f10x.h"                  // Device header



void uart_init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	USART_InitTypeDef USART_InitStructure={0};
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure={0};
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

typedef struct{
	uint8_t * txBuf;
	uint16_t dmaIndex1S;
	uint16_t dmaIndex2E;
	uint16_t bufSize;
	uint8_t isFull;
}uart_dma_tx_fifo_t;

uint16_t uart_tx_fifo_put(uart_dma_tx_fifo_t * txFifo,uint8_t * data,uint16_t len){
	

}

uint16_t uart_get_tx_fifo_len(uart_dma_tx_fifo_t * txFifo){
	if(txFifo->isFull){
		return txFifo->bufSize;
	}else if(txFifo->dmaIndex2E >= txFifo->dmaIndex1S){
		return (txFifo->bufSize - 1) - txFifo->dmaIndex2E + txFifo->dmaIndex1S;
	}
}


void uart_dma_TC_callback(){
	

}


void uart_dma_init(uint8_t * rxBuf,uint16_t rxSize,uart_dma_tx_fifo_t * txFifo){
	if(txFifo){
		txFifo->dmaIndex1S = txFifo->dmaIndex2E = 0;
	}
	
	DMA_InitTypeDef DMA_InitStructure={0};
	if(txFifo && txFifo->txBuf && txFifo->bufSize){
		DMA_InitStructure.DMA_BufferSize = txFifo->bufSize;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)txFifo->txBuf;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_Init(DMA1_Channel4,&DMA_InitStructure);
		DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
	}
}