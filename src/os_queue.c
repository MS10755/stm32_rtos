#include "os_queue.h"
#include "os.h"

extern os_tcb_t * running_tcb;
extern volatile List_t osReadyList[OS_TASK_PRIORITY_MAX + 1];
extern volatile List_t osDelayByWriteQueueList;
extern volatile List_t osDelayByReadQueueList;

#define OS_QUEUE_ISFULL(q) queue_isFull(q->queue)
#define OS_QUEUE_ISEMPTY(q) (queue_get_len(q->queue) == 0)

extern void os_selectNextPrioityHighestTask(void);

void osQueueResumeTask(os_queue_t * os_queue,List_t * osDelayByxxxxQueueList,queueRet_t ret){
    if(ret == QUEUE_OK){
        /* resume a highest task from osDelayByxxxxQueueList */
        ListNode_t * highestTaskNode = NULL;
        LIST_ITERATOR_BEGIN(osDelayByxxxxQueueList);
        while(LIST_ITERATOR_HAS(osDelayByxxxxQueueList)){
            ListNode_t * itNode =  LIST_ITERATOR_GET(osDelayByxxxxQueueList);
            os_tcb_t * itTCB = LIST_NODE_GET_DATA(itNode);
            if(itTCB->waitQueue == os_queue){
            if(highestTaskNode == NULL){
                highestTaskNode = itNode;
            }else{
                os_tcb_t * highestTaskTCB = LIST_NODE_GET_DATA(highestTaskNode);
                if(itTCB->taskPriority > highestTaskTCB->taskPriority){
                    highestTaskNode = itNode;
                }
            }
            }
            LIST_ITERATOR_NEXT(osDelayByxxxxQueueList);
        }
        if(highestTaskNode){
            os_tcb_t * highestTaskTCB = LIST_NODE_GET_DATA(highestTaskNode);
            List_Remove(highestTaskNode);
            LIST_NODE_SET_VALUE(highestTaskNode, highestTaskTCB->taskPriority);
            List_InserOrderDec((List_t*)&osReadyList[highestTaskTCB->taskPriority],highestTaskNode);
            if(highestTaskTCB->taskPriority > running_tcb->taskPriority){
                os_selectNextPrioityHighestTask();
            }
        }
    }    
}
os_queue_t * os_queue_create(os_queue_t * os_queue,queueSize_t size,queueSize_t itemSize,Queue_type_t type){
    os_suspend();
    Queue_t * queue = queue_create(size,itemSize,type);
    if(queue == NULL) return NULL;
    os_queue->queue = queue;
    List_Initialise(&os_queue->ListtaskForRead);
    List_Initialise(&os_queue->ListtaskForWrite);
    os_resume();
    return os_queue;
}


os_ret_t os_queue_write(os_queue_t * os_queue,void * item,os_tick_t wait){
    os_ret_t ret = OS_RET_FALSE;
    os_suspend();
    if(OS_QUEUE_ISFULL(os_queue)){
        if(wait > 0){
            /* we need to blocked this task */
            os_tcb_t * taskTCB = (os_tcb_t *)LIST_NODE_GET_DATA(running_tcb->taskNode);
            taskTCB->waitQueue = os_queue;
            List_Remove(running_tcb->taskNode);
            os_tick_t readyTicks = wait + os_tick_count;
            LIST_NODE_SET_VALUE(running_tcb->taskNode, readyTicks);
            List_InserOrderInc((List_t*)&osDelayByWriteQueueList,running_tcb->taskNode);
            os_selectNextPrioityHighestTask();
            os_resume();
            // try again
            ret = queue_write(os_queue->queue,item);
            osQueueResumeTask(os_queue,(List_t*)&osDelayByReadQueueList,ret);
        }else{
            /* dont wait for writeable just return false */
            os_resume();
            return OS_RET_FALSE;
        }

    }else{
       ret = queue_write(os_queue->queue,item);
       osQueueResumeTask(os_queue,(List_t*)&osDelayByReadQueueList,ret);
    }

    os_resume();
    return ret;
}



os_ret_t os_queue_read(os_queue_t * os_queue,void * item,os_tick_t wait){
    os_ret_t ret = OS_RET_FALSE;
    os_suspend();
    if(OS_QUEUE_ISEMPTY(os_queue)){
        if(wait > 0){
            /* we need to blocked this task */
            os_tcb_t * taskTCB = (os_tcb_t *)LIST_NODE_GET_DATA(running_tcb->taskNode);
            taskTCB->waitQueue = os_queue;
            List_Remove(running_tcb->taskNode);
            os_tick_t readyTicks = wait + os_tick_count;
            LIST_NODE_SET_VALUE(running_tcb->taskNode, readyTicks);
            List_InserOrderInc((List_t*)&osDelayByReadQueueList,running_tcb->taskNode);
            os_selectNextPrioityHighestTask();
            os_resume();
            // try again
            ret = queue_read(os_queue->queue,item);
            osQueueResumeTask(os_queue,(List_t*)&osDelayByWriteQueueList,ret);
        }else{
            /* dont wait for writeable just return false */
            os_resume();
            return OS_RET_FALSE;
        }

    }else{
       ret = queue_read(os_queue->queue,item);
       osQueueResumeTask(os_queue,(List_t*)&osDelayByWriteQueueList,ret);
    }
    os_resume();
    return ret;
}

