#include "os.h"
#include "list.h"
#include "os_cpu.h"
#include "os_task.h"
#include "stm32f10x.h"                  // Device header
#include "sys_lib.h"

extern void init_cpu(void);

static os_state_t _os_state = OS_STATE_NOTSTART;
static uint32_t os_suspend_cnt = 0;
extern volatile List_t osReadyList[OS_TASK_PRIORITY_MAX];
extern volatile List_t osDelayList;

void os_init(){
	for(int i=0;i<= OS_TASK_PRIORITY_MAX;i++){
		List_Initialise((List_t*)&(osReadyList[i]));
	}
	List_Initialise((List_t*)&osDelayList);
}
#define IDLE_TASK_STACK_SIZE 128
os_tcb_t idle_taskTCB;
os_stack_t idle_stack[IDLE_TASK_STACK_SIZE];

void idle_task(){
	while(1){
	__ASM("WFI \n");
	}
}

static void create_user_task(){
	extern int osTaskCreate$$Base;
	extern int osTaskCreate$$Limit;
	sys_lib_t * task_create_fun = NULL;
	task_create_fun = (sys_lib_t *)&osTaskCreate$$Base;
	for(;task_create_fun != (sys_lib_t *)&osTaskCreate$$Limit;task_create_fun++){
		lib_fun_t fun = (lib_fun_t)task_create_fun->fun;
		fun();
	}
}

void os_selectNextPrioityHighestTask(void);
void os_start(){
	init_cpu();
	_os_state = OS_STATE_RUNNING;
	os_task_create(&idle_taskTCB,idle_stack,IDLE_TASK_STACK_SIZE,idle_task,"idle",0);
    create_user_task();
	os_selectNextPrioityHighestTask();
}

os_state_t os_state(){
	return _os_state;
}

void os_suspend(){
	__disable_irq();
	_os_state = OS_STATE_SUSPEND;
	os_suspend_cnt++;
}


void os_resume(){
	if(os_suspend_cnt > 0){
		os_suspend_cnt--;
	}
	if(os_suspend_cnt == 0){
		_os_state = OS_STATE_RUNNING;
		__enable_irq();
	}
}

void os_yiled(){
	os_cpu_req_svc();
}

os_tick_t os_ge_useage(void){
	return OS_TIMESLICE_TICKS - idle_taskTCB.cpuUseTick;
}


void pendSVC_enter(os_tcb_t * LastTask){
//	LastTask->scheduleExitTick = os_tick_count;
//	LastTask->cpuUseTick = LastTask->scheduleExitTick - LastTask->scheduleStartTick;
}


void pendSVC_exit(os_tcb_t * NextTask){
//	NextTask->scheduleStartTick = os_tick_count;
}

