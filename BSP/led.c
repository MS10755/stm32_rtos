#include "bsp.h"
#include "stm32f10x.h"                  // Device header

void led_init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure={0};
	GPIO_InitStructure.GPIO_Pin = LED0_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(LED0_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(LED1_PORT,&GPIO_InitStructure);
}

void led_set(int ledNumber,int value){
	if(ledNumber == 0){
		GPIO_WriteBit(LED0_PORT,LED0_PIN,(BitAction)value);
	}else if(ledNumber == 1){
		GPIO_WriteBit(LED1_PORT,LED1_PIN,(BitAction)value);
	}
}

void led_toggle(int ledNumber){
	int lastValue;
	if(ledNumber == 0){
		lastValue = GPIO_ReadOutputDataBit(LED0_PORT,LED0_PIN);
		GPIO_WriteBit(LED0_PORT,LED0_PIN,(BitAction)!lastValue);

	}else if(ledNumber == 1){
		lastValue = GPIO_ReadOutputDataBit(LED1_PORT,LED1_PIN);
		GPIO_WriteBit(LED1_PORT,LED1_PIN,(BitAction)!lastValue);
	}
}
