#ifndef __OS_H__
#define __OS_H__
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define OS_TASK_NAME_LEN_MAX 32

#define os_malloc(size) malloc(size)
#define os_calloc(size,n) calloc(size,n)
#define os_free(p) free(p)

#define OS_TIMESLICE_TICKS	10
enum{
	BLOCK_FLAG_DELAY = (1<<0),
	BLOCK_FLAG_EVENT = (1<<1),
	BLOCK_FLAG_QUEUE = (1<<2),
};
#define OS_TICK_MAX UINT32_MAX
typedef uint32_t os_stack_t;
typedef uint32_t os_tick_t;
typedef int16_t os_taskPriority_t;
typedef enum{
	OS_TASK_SUSPEND,
	OS_TASK_RUNING,
	OS_TASK_READY,
	OS_TASK_BLOCKED,
}os_task_state_t;

typedef enum{
	OS_RET_FALSE = 0,
	OS_RET_TRUE,
}os_ret_t;

enum{
	INTERNAL_FLAG_UNBLOCKEDBYTICK = 1<<0,
	INTERNAL_FLAG_UNBLOCKEDBYREADQUEUE = 1<<1,
	INTERNAL_FLAG_UNBLOCKEDBYWRITEQUEUE = 1<<2,
};

typedef struct{
	os_stack_t * p_stack;//must in first
	os_stack_t * stack;
	uint32_t stack_size;
	struct{
		uint8_t blocked_flag;
	};
	os_taskPriority_t taskPriority;
	os_tick_t scheduleStartTick;
	os_tick_t scheduleExitTick;
	os_tick_t cpuUseTick;
	os_tick_t blockTick;
	os_tick_t readyTick;
	os_task_state_t state;
	List_t * suspendFrom;
	ListNode_t * taskNode;
	uint8_t _internal_flags;
	void * waitQueue;
	void * private_data;
    char name[OS_TASK_NAME_LEN_MAX];
}os_tcb_t;


typedef enum{
	OS_STATE_NOTSTART = 0,
	OS_STATE_RUNNING,
	OS_STATE_SUSPEND,
}os_state_t;

extern volatile os_tick_t os_tick_count;

void os_init(void);
void os_start(void);
void os_yiled(void);
void os_suspend(void);
void os_resume(void);
os_tick_t os_ge_useage(void);
os_state_t os_state(void);

#endif
