#include "os.h"
#include "os_task.h"
#include "os_queue.h"
#include "bsp.h"


#define TASK_STACK_SIZE 128

os_queue_t queue1;
os_tcb_t task1_tcb;
os_tcb_t task2_tcb;
os_stack_t task1_stack[TASK_STACK_SIZE];
os_stack_t task2_stack[TASK_STACK_SIZE];

extern os_tcb_t * running_tcb;
extern os_tcb_t * switch_tcb;

void change_task(){
	if(OS_STATE_RUNNING != os_state()) return;
	if(running_tcb == &task1_tcb){
		switch_tcb = &task2_tcb;
	}else{
		switch_tcb = &task1_tcb;
	}
	os_yiled();
}

void task1(){
	while(1){
		printf("task1 running...\r\n");
		os_task_delay(1000);
		printf("task1 running...\r\n");
		os_task_delay(1000);
	}
}

void task2(){
	while(1){
		printf("[%d]task2 running...\r\n",os_tick_count);
//		printf("cpu usage:%.2f \r\n",(float)os_ge_useage()/OS_TIMESLICE_TICKS);
		os_task_delay(1000);
		os_task_delay(1000);
	}
}

extern void save_cpu_context(void);
extern void uart_init(void);
extern void LLA_SYS_Time_Init(void);

typedef struct{
	void * fun;
	const char * name;
}sys_lib_t;

#define SYSLIB_EXPORT(name,sectionName) \
const sys_lib_t sys_lib_##name __attribute__((section(sectionName)))={ \
	(void*)&name,	\
	#name	\
};

#define OS_TASK_CREATE(taskName,stackSize,priority)	\
os_tcb_t taskName##_tcb;	\
os_stack_t taskName##_stack[stackSize];	\
void taskName(void); \
void taskName##_create(){ \
	os_task_create(&taskName##_tcb,taskName##_stack,stackSize,taskName,priority); \
} \
SYSLIB_EXPORT(taskName##_create,"osTaskCreate") \
void taskName(void)


OS_TASK_CREATE(led0Task,128,5){
	while(1){
		os_queue_read(&queue1,NULL,OS_TICK_MAX);
		led_toggle(0);// red led blink
		os_task_delay(2000);
	}
}

OS_TASK_CREATE(led1Task,128,5){
	while(1){
		os_queue_write(&queue1,NULL,OS_TICK_MAX);
//	led_toggle(1);
		printf("give queue1\r\n");
	 os_task_delay(1000);
	}
}
OS_TASK_CREATE(task6,128,4){
	while(1){
//		os_queue_write(&queue1,NULL,OS_TICK_MAX);
//		os_task_delay(500);
	}
}

typedef void (*lib_fun_t)(void);
int main(){
	uart_init();
	LLA_SYS_Time_Init();
	led_init();
	extern int osTaskCreate$$Base;
	extern int osTaskCreate$$Limit;
	os_init();
	sys_lib_t * task_create_fun = NULL;
	task_create_fun = (sys_lib_t *)&osTaskCreate$$Base;
	for(;task_create_fun != (sys_lib_t *)&osTaskCreate$$Limit;task_create_fun++){
		lib_fun_t fun = (lib_fun_t)task_create_fun->fun;
		fun();
	}
	os_queue_create(&queue1,1,0,QUEUE_TYPE_COUNT);
	os_task_create(&task1_tcb,task1_stack,TASK_STACK_SIZE,task1,5);
	os_task_create(&task2_tcb,task2_stack,TASK_STACK_SIZE,task2,5);
	switch_tcb = &task1_tcb;
	running_tcb = &task1_tcb;
	os_start();
	while(1);
}
