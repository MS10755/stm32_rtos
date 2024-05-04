#ifndef __OS_TASK_H__
#define __OS_TASK_H__
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "os.h"

#define OS_TASK_PRIORITY_MAX	5
typedef void (*task_t)(void);


void os_task_create(os_tcb_t * tcb,os_stack_t * stack,uint32_t stack_size,task_t task,const char *name,os_taskPriority_t priority);
void os_task_delete(os_tcb_t * tcb);
void os_task_delay(os_tick_t ticksDelay);
void os_task_suspend(os_tcb_t * tcb);
void os_task_resume(os_tcb_t * tcb);
#endif
