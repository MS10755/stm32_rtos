#include "os_task.h"

os_tcb_t * running_tcb = 0x00;
os_tcb_t * switch_tcb =0x00;
bool os_need_yiled = false;

volatile List_t osReadyList[OS_TASK_PRIORITY_MAX + 1] = {0};
volatile List_t osDelayList = {0};
volatile List_t osDelayByWriteQueueList = {0};
volatile List_t osDelayByReadQueueList = {0};

void os_task_errorExit(){
	// shouldn't run to here.
	os_suspend();
	printf("os_task_errorExit\r\n");
	while(1){
	
	};
}

#define OS_TCB_GET_LIST_NODE(tcb) ((list_node_t*)tcb->private_data)


void os_selectNextPrioityHighestTask(void);

void os_task_create(os_tcb_t * tcb,os_stack_t * stack,uint32_t stack_size,task_t task,os_taskPriority_t priority){
	os_suspend();
	tcb->p_stack = (os_stack_t*)((uint32_t)(stack+ stack_size-1) & 0xFFFFFFF8);//8字节对齐地址
	*(--tcb->p_stack) = 0x01000000;// xPSR
	*(--tcb->p_stack) = (uint32_t)task;//PC
	*(--tcb->p_stack) = (uint32_t)os_task_errorExit;//LR 
	*(--tcb->p_stack) = 0x12121212;//R12
	*(--tcb->p_stack) = 0x03030303;//R3
	*(--tcb->p_stack) = 0x02020202;//R2
	*(--tcb->p_stack) = 0x01010101;//R1
	*(--tcb->p_stack) = 0x00000000;//R0
	
	*(--tcb->p_stack) = 0x11111111;//R11
	*(--tcb->p_stack) = 0x10101010;//R10
	*(--tcb->p_stack) = 0x09090909;//R9
	*(--tcb->p_stack) = 0x08080808;//R8
	*(--tcb->p_stack) = 0x07070707;//R7
	*(--tcb->p_stack) = 0x06060606;//R6
	*(--tcb->p_stack) = 0x05050505;//R5
	*(--tcb->p_stack) = 0x04040404;//R4
	
	/* Init taskNode */
	tcb->taskNode = (ListNode_t*)os_calloc(1,sizeof(ListNode_t));
	LIST_NODE_SET_DATA(tcb->taskNode,tcb);
	// set priorty
	tcb->taskPriority = priority;
	LIST_NODE_SET_VALUE(tcb->taskNode,tcb->taskPriority);
	
	/* add to corresponding readyList[priority]  */
	List_t * readyListPriority = (List_t*)&(osReadyList[tcb->taskPriority]);
	List_InserOrderDec(readyListPriority,tcb->taskNode);
	tcb->state = OS_TASK_READY;
	os_resume();
}

void os_task_delete(os_tcb_t * tcb){
	os_suspend();
	if(tcb==NULL){
		tcb = running_tcb;
	}
	List_Remove(tcb->taskNode);
	os_free(tcb->taskNode);
	if(tcb == running_tcb){
		os_selectNextPrioityHighestTask();
	}
	os_resume();
}

void os_task_delay(os_tick_t ticksDelay){
	if(ticksDelay == 0) return;
	os_suspend();
	running_tcb->state = OS_TASK_BLOCKED;
	/* remove taskNode from readyList */
	List_Remove(running_tcb->taskNode);
	os_tick_t readyTicks = ticksDelay + os_tick_count;
	/* set nodeValue for sort */
	LIST_NODE_SET_VALUE(running_tcb->taskNode,readyTicks);// set delay ticks
	
	/* insert taskNode to delayList */
	List_InserOrderInc((List_t*)&osDelayList,running_tcb->taskNode);

	os_selectNextPrioityHighestTask();
	os_resume();
}

void os_checkDelayList(){
	os_suspend();
	bool need_yiled = false;
	LIST_ITERATOR_BEGIN(&osDelayList);
	while(LIST_ITERATOR_HAS(&osDelayList)){
		/* get one taskNode to check it if arrival delayTicks */
		ListNode_t * taskNode = LIST_ITERATOR_GET(&osDelayList);
		os_tick_t readyTicks = LIST_NODE_GET_VALUE(taskNode);

		/* it arrival delayTicks */
		if(readyTicks <= os_tick_count){
		
			/* remove it from delayList */
			List_Remove(taskNode);

			/* set nodeValue for sort */
			os_tcb_t * taskTcb = (os_tcb_t *)LIST_NODE_GET_DATA(taskNode);
			LIST_NODE_SET_VALUE(taskNode,taskTcb->taskPriority);
	
			taskTcb->state = OS_TASK_READY;
			
			List_InserOrderDec((List_t*)&osReadyList[taskTcb->taskPriority],taskNode);
			
			/* will be scheduled when unblocked taskPriority > runningTaskPriority */
			if( taskTcb->taskPriority > running_tcb->taskPriority ){
				need_yiled = true;
			}
		}else{
			/* no need check remain task */
			break;
		}
		/* move to next taskNode */
		LIST_ITERATOR_NEXT(&osDelayList);
	}
	if(need_yiled) os_selectNextPrioityHighestTask();
	os_resume();
}


void os_switchTaskTCB(os_tcb_t * tcbSwitch){
	if(tcbSwitch != running_tcb){
		switch_tcb = tcbSwitch;
		os_need_yiled = true;
	}
}

void os_selectNextPrioityHighestTask(){
	os_suspend();
	List_t * readyListHighestPriority = NULL;
	for(os_taskPriority_t priorityHigest = OS_TASK_PRIORITY_MAX; priorityHigest>=0 ;priorityHigest--){
		if(LIST_GET_LEN(&osReadyList[priorityHigest]) > 0){
			readyListHighestPriority = (List_t *)&osReadyList[priorityHigest];
			break;
		}
	}

	if(readyListHighestPriority != NULL){

		/* curent global iter --> currentRunningTask */
		/*  so we iter to next task */
		if(LIST_ITERATOR_GLOBAL_HAS(readyListHighestPriority)){
			if(LIST_ITERATOR_GLOBAL_HAS_NEXT(readyListHighestPriority)){
				LIST_ITERATOR_GLOBAL_NEXT(readyListHighestPriority);
			}else{
				LIST_ITERATOR_GLOBAL_BEGIN(readyListHighestPriority);
			}
		}else{
			LIST_ITERATOR_GLOBAL_BEGIN(readyListHighestPriority);
		}

		/* get task tcb */
		ListNode_t * taskNodeSwitch = NULL;
		taskNodeSwitch = LIST_ITERATOR_GLOBAL_GET(readyListHighestPriority);
		os_tcb_t * taskTcbSwitch = (os_tcb_t *)LIST_NODE_GET_DATA(taskNodeSwitch);

		/* switch task tcb */
		os_switchTaskTCB(taskTcbSwitch);
		os_yiled();
		os_resume();
	}
}

void os_task_suspend(os_tcb_t * tcb){
	os_suspend();
	if(tcb->state != OS_TASK_SUSPEND){
		tcb->state = OS_TASK_SUSPEND;
		tcb->suspendFrom = LIST_NODE_GET_OWNER(tcb->taskNode);
		List_Remove(tcb->taskNode);
	}
	os_resume();
}

void os_task_resume(os_tcb_t * tcb){
	os_suspend();
	if(tcb->state == OS_TASK_SUSPEND){
		if(tcb->suspendFrom == &osDelayList){
			List_InserOrderInc((List_t*)&osDelayList,tcb->taskNode);
			tcb->state =OS_TASK_BLOCKED;
		}else{
			List_InserOrderDec(tcb->suspendFrom,tcb->taskNode);
			tcb->state = OS_TASK_READY;
		}
	}
	os_resume();
}

