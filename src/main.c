#include "os.h"
#include "os_task.h"
#include "os_queue.h"
#include "bsp.h"
#include "Juan_eventButton.h"
#include "os_semaphore.h"
#include "os_taskhelper.h"

JEB_obj_t key0,key1,key2;

#define TASK_STACK_SIZE 128

os_semaphore_t *semaphr_led;
os_queue_t * queue_mutex;
os_tcb_t task1_tcb;
os_tcb_t task2_tcb;
os_stack_t task1_stack[TASK_STACK_SIZE];
os_stack_t task2_stack[TASK_STACK_SIZE];

extern os_tcb_t * running_tcb;


extern void uart_init(void);
extern void LLA_SYS_Time_Init(void);



OS_TASK_CREATE(led0Task,128,5){
	while(1){
		os_ret_t ret = os_semaphore_take(semaphr_led,OS_TICK_MAX);
		if(ret == OS_RET_FALSE){
//			printf("read queue1 led0 timeout\r\n");
		}else{
			led_toggle(0);// red led blink
		}
	}
}

OS_TASK_CREATE(led1Task,128,5){
	while(1){
		os_ret_t ret = os_semaphore_take(semaphr_led,OS_TICK_MAX);
		if(ret == OS_RET_FALSE){
//			printf("read queue1 led1 timeout\r\n");
		}else{
			led_toggle(1);// red led blink
		}
	}
}

void bsp_printf(){
    os_semaphore_mutex_lock(queue_mutex,OS_TICK_MAX);
    printf("[%d][%s]task_printf\r\n",os_tick_count,running_tcb->name);
    os_semaphore_mutex_unlock(queue_mutex,OS_TICK_MAX);
    os_task_delay(5);
}

OS_TASK_CREATE(bsp_task1,512,4){
	while(1){
        bsp_printf();
	}
}
OS_TASK_CREATE(bsp_task2,512,4){
	while(1){
        bsp_printf();
	}
}


extern void key_init(void);
extern int key_read(int key);

OS_TASK_CREATE(keyCheck,128,1){
	while(1){
        JUAN_EVENTBUTTON_notify(&key0,key_read(0));
        JUAN_EVENTBUTTON_notify(&key1,key_read(1));
        JUAN_EVENTBUTTON_notify(&key2,key_read(2));
		JUAN_EVENTBUTTON_poll(&key0,os_tick_count);
		JUAN_EVENTBUTTON_poll(&key1,os_tick_count);
		JUAN_EVENTBUTTON_poll(&key2,os_tick_count);
		os_task_delay(10);
	}
}

void function_a();
static void key_callback(void * obj,JUAN_EVENTBUTTON_event_t event){
    JEB_obj_t*key = obj;
		if(key->obj == &key0 && event == JUAN_EVENTBUTTON_EVENT_PRESSED){
			os_semaphore_give(semaphr_led,OS_TICK_MAX);
		}else if(key->obj == &key1 && event == JUAN_EVENTBUTTON_EVENT_PRESSED){
			function_a();
		}
}




void fault_test_by_div0(void) {
    volatile int * SCB_CCR = (volatile int *) 0xE000ED14; // SCB->CCR
    int x, y, z;

    *SCB_CCR |= (1 << 4); /* bit4: DIV_0_TRP. */

    x = 10;
    y = 0;
    z = x / y;
    printf("z:%d\n", z);
}

void function_c(){
	fault_test_by_div0();
}

void function_b(){
	function_c();
}

void function_a(){
	function_b();
}

extern void fault_init(const char * proj);
int main(){
	uart_init();
	fault_init("rtos");
	LLA_SYS_Time_Init();
	led_init();
	key_init();
	JUAN_EVENTBUTTON_init(&key0,JEB(LEVEL_LOW),3000,500,key_callback);
	JUAN_EVENTBUTTON_init(&key1,JEB(LEVEL_LOW),3000,500,key_callback);
	JUAN_EVENTBUTTON_init(&key2,JEB(LEVEL_LOW),3000,500,key_callback);
	
	os_init();

	semaphr_led = os_semaphore_count_create(1);
    queue_mutex = os_semaphore_mutex_create();

	os_start();
	while(1);
}
