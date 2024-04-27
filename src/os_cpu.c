#include "stm32f10x.h"                  // Device header


#define NVIC_INT_CTRL *((volatile uint32_t*)0xE000ED04) //;中断控制寄存器
#define NVIC_SYSPRI14 *((volatile uint32_t*)0xE000ED22)
#define NVIC_PENDSV_PRI  0xFF //; PendSV优先级(最低). 
#define NVIC_PENDSVSET  0x10000000 //; PendSV触发值



void os_cpu_req_svc(){
	NVIC_SYSPRI14 = NVIC_PENDSV_PRI;
	NVIC_INT_CTRL = NVIC_PENDSVSET;
}
