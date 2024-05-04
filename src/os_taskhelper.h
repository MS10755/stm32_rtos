#ifndef __OS_TASKHELPER_H__

#define __OS_TASKHELPER_H__
#include "sys_lib.h"

#define OS_TASK_CREATE(taskName,stackSize,priority)	\
os_tcb_t taskName##_tcb;	\
os_stack_t taskName##_stack[stackSize];	\
void taskName(void); \
void taskName##_create(){ \
	os_task_create(&taskName##_tcb,taskName##_stack,stackSize,taskName,#taskName,priority); \
} \
SYSLIB_EXPORT(taskName##_create,"osTaskCreate") \
void taskName(void)


#endif