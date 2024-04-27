#include "stm32f10x.h"                  // Device header

#include "os.h"

#define LLA_SYS_TIME_CLK SystemCoreClock
#define SysTick_LoadValue (LLA_SYS_TIME_CLK / 1000U)
#define CYCLES_PER_MICROSECOND (LLA_SYS_TIME_CLK/1000000U)
volatile static os_tick_t System_ms = 0;
volatile static os_tick_t timeSliceTicks = 0;
volatile os_tick_t os_tick_count = 0;

void LLA_SYS_Time_Init(){
	SystemCoreClockUpdate();
  SysTick_Config(SysTick_LoadValue);
  NVIC_SetPriority(SysTick_IRQn, 0);
}


extern void change_task(void);
extern void os_checkDelayList(void);
extern void os_selectNextPrioityHighestTask(void);
void SysTick_Handler(void){
	os_tick_count++;
	System_ms++;
	if(os_state() == OS_STATE_RUNNING){
		if(os_tick_count >= timeSliceTicks){
		os_selectNextPrioityHighestTask();
		timeSliceTicks = os_tick_count + OS_TIMESLICE_TICKS;
	}
		os_checkDelayList();
	}
}

