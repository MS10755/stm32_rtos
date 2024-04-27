#ifndef __OS_QUEUE_H__
#define __OS_QUEUE_H__
#include "queue.h"
#include "list.h"
#include "os_task.h"

typedef struct os_queue
{
    Queue_t * queue;
    List_t  ListtaskForWrite;
    List_t  ListtaskForRead;
}os_queue_t;

os_queue_t * os_queue_create(os_queue_t * os_queue,queueSize_t size,queueSize_t itemSize,Queue_type_t type);
os_ret_t os_queue_write(os_queue_t * os_queue,void * item,os_tick_t wait);
os_ret_t os_queue_read(os_queue_t * os_queue,void * item,os_tick_t wait);
#endif

