#include "stm32f10x.h"                  // Device header


void key_init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure={0};
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
}

int key_read(int key){
	uint8_t val;
	if(key == 0){
		val = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4);
	}else if(key == 1){
		val = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3);
	}else if(key == 2){
		val = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2);
	}
	return val;
}
