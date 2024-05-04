#include "os_queue.h"
#include "os.h"

extern os_tcb_t * running_tcb;
extern volatile List_t osReadyList[OS_TASK_PRIORITY_MAX + 1];
extern volatile List_t osDelayByWriteQueueList;
extern volatile List_t osDelayByReadQueueList;

#define _OS_QUEUE_ISFULL(q,p) queue_isFull(q->queue,p)
#define _OS_QUEUE_ISEMPTY(q,p) (queue_get_len(q->queue,p) == 0)

extern void os_selectNextPrioityHighestTask(void);

static queueSize_t OS_QUEUE_ISFULL(os_queue_t *q){
    if(q->queue->type == QUEUE_TYPE_MUTEX){
         return _OS_QUEUE_ISFULL(q,running_tcb);
    }else{
        return _OS_QUEUE_ISFULL(q,NULL);
    }
}

static queueSize_t OS_QUEUE_ISEMPTY(os_queue_t *q){
    if(q->queue->type == QUEUE_TYPE_MUTEX){
         return _OS_QUEUE_ISEMPTY(q,running_tcb);
    }else{
        return _OS_QUEUE_ISEMPTY(q,NULL);
    }
}

static queueRet_t o_queue_write(Queue_t * queue,void * item){
    if(queue->type == QUEUE_TYPE_MUTEX){
        return queue_write(queue,running_tcb);
    }else{
        return queue_write(queue,item);
    }
}
static queueRet_t o_queue_read(Queue_t * queue,void * item){
    if(queue->type == QUEUE_TYPE_MUTEX){
        return queue_read(queue,running_tcb);
    }else{
        return queue_read(queue,item);
    }
}

void osQueueResumeTaskTimeOut(List_t * osDelayByxxxxQueueList){
	os_suspend();
	LIST_ITERATOR_BEGIN(osDelayByxxxxQueueList);
	while(LIST_ITERATOR_HAS(osDelayByxxxxQueueList)){
		ListNode_t * itNode =  LIST_ITERATOR_GET(osDelayByxxxxQueueList);
		os_tick_t readyTicks = LIST_NODE_GET_VALUE(itNode);
		#warning "TODO: if readyTicks Exactly equal to 0xFFFFFFFF will not be resume by sysTicks "
		if(readyTicks == OS_TICK_MAX){
			LIST_ITERATOR_NEXT(osDelayByxxxxQueueList);
			continue;
		}
		if(readyTicks <= os_tick_count){
				os_tcb_t * itTCB = LIST_NODE_GET_DATA(itNode);
				List_Remove(itNode);
        LIST_NODE_SET_VALUE(itNode, itTCB->taskPriority);
				itTCB->_internal_flags |= INTERNAL_FLAG_UNBLOCKEDBYTICK;
        List_InserOrderDec((List_t*)&osReadyList[itTCB->taskPriority],itNode);
				if(itTCB->taskPriority > running_tcb->taskPriority){
					os_selectNextPrioityHighestTask();
				}
		}else{
			break;
		}
		LIST_ITERATOR_NEXT(osDelayByxxxxQueueList);
	}
	os_resume();
}


void os_checkQueueList(){
	osQueueResumeTaskTimeOut((List_t*)&osDelayByWriteQueueList);
	osQueueResumeTaskTimeOut((List_t*)&osDelayByReadQueueList);
}

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
                }else if(itTCB->taskPriority == highestTaskTCB->taskPriority){
									/* Unlock the earliest blocked task with the same priority */
									if(itTCB->blockTick < highestTaskTCB->blockTick){
										highestTaskNode = itNode;
									}
								}
            }
            }
            LIST_ITERATOR_NEXT(osDelayByxxxxQueueList);
        }
        if(highestTaskNode){
            os_tcb_t * highestTaskTCB = LIST_NODE_GET_DATA(highestTaskNode);
            List_Remove(highestTaskNode);
            LIST_NODE_SET_VALUE(highestTaskNode, highestTaskTCB->taskPriority);
						if(osDelayByxxxxQueueList == &osDelayByReadQueueList){
							highestTaskTCB->_internal_flags |= INTERNAL_FLAG_UNBLOCKEDBYWRITEQUEUE;
						}else{
							highestTaskTCB->_internal_flags |= INTERNAL_FLAG_UNBLOCKEDBYREADQUEUE;
						}
            List_InserOrderDec((List_t*)&osReadyList[highestTaskTCB->taskPriority],highestTaskNode);
            if(highestTaskTCB->taskPriority >= running_tcb->taskPriority){
                os_selectNextPrioityHighestTask();
            }
        }
    }    
}
os_queue_t * os_queue_create(queueSize_t size,queueSize_t itemSize,Queue_type_t type){
    os_queue_t * os_queue;
		os_suspend();
    Queue_t * queue = queue_create(size,itemSize,type);
    if(queue == NULL) return NULL;
		os_queue = (os_queue_t *)os_malloc(sizeof(os_queue_t));
    os_queue->queue = queue;
    List_Initialise(&os_queue->ListtaskForRead);
    List_Initialise(&os_queue->ListtaskForWrite);
    os_resume();
    return os_queue;
}


os_ret_t os_queue_write(os_queue_t * os_queue,void * item,os_tick_t wait){
		queueRet_t queueRet = QUEUE_FAILED;
    os_suspend();
    if(OS_QUEUE_ISFULL(os_queue)){
        if(wait > 0){
            /* we need to blocked this task */
            os_tcb_t * taskTCB = (os_tcb_t *)LIST_NODE_GET_DATA(running_tcb->taskNode);
            taskTCB->waitQueue = os_queue;
            List_Remove(running_tcb->taskNode);
            os_tick_t readyTicks = wait + os_tick_count;
						taskTCB->blockTick = os_tick_count;
						if(wait == OS_TICK_MAX){
							readyTicks = OS_TICK_MAX;
						}
            LIST_NODE_SET_VALUE(running_tcb->taskNode, readyTicks);
            List_InserOrderInc((List_t*)&osDelayByWriteQueueList,running_tcb->taskNode);
            os_selectNextPrioityHighestTask();
            os_resume();
            // try again
						if(running_tcb->_internal_flags & INTERNAL_FLAG_UNBLOCKEDBYREADQUEUE){
							running_tcb->_internal_flags &=~INTERNAL_FLAG_UNBLOCKEDBYREADQUEUE;
							os_suspend();
							queueRet = o_queue_write(os_queue->queue,item);
                            if(os_queue->queue->type != QUEUE_TYPE_MUTEX){
                                osQueueResumeTask(os_queue,(List_t*)&osDelayByReadQueueList,queueRet);
                            }
						}
        }else{
            /* dont wait for writeable just return false */
            os_resume();
            return OS_RET_FALSE;
        }

    }else{
       queueRet = o_queue_write(os_queue->queue,item);
        if(os_queue->queue->type != QUEUE_TYPE_MUTEX){
            osQueueResumeTask(os_queue,(List_t*)&osDelayByReadQueueList,queueRet);
        }
       
    }

    os_resume();
		return queueRet == QUEUE_OK ? OS_RET_TRUE : OS_RET_FALSE ;
}



os_ret_t os_queue_read(os_queue_t * os_queue,void * item,os_tick_t wait){
		queueRet_t queueRet = QUEUE_FAILED;
    os_suspend();
    if(OS_QUEUE_ISEMPTY(os_queue)){
        if(wait > 0){
            /* we need to blocked this task */
            os_tcb_t * taskTCB = (os_tcb_t *)LIST_NODE_GET_DATA(running_tcb->taskNode);
            taskTCB->waitQueue = os_queue;
            List_Remove(running_tcb->taskNode);
            os_tick_t readyTicks = wait + os_tick_count;
						taskTCB->blockTick = os_tick_count;
						if(wait == OS_TICK_MAX){
							readyTicks = OS_TICK_MAX;
						}
            LIST_NODE_SET_VALUE(running_tcb->taskNode, readyTicks);
            List_InserOrderInc((List_t*)&osDelayByReadQueueList,running_tcb->taskNode);
            os_selectNextPrioityHighestTask();
            os_resume();
            // try again
						if(running_tcb->_internal_flags & INTERNAL_FLAG_UNBLOCKEDBYWRITEQUEUE){
							running_tcb->_internal_flags &=~INTERNAL_FLAG_UNBLOCKEDBYWRITEQUEUE;
							os_suspend();
							queueRet = o_queue_read(os_queue->queue,item);
                            if(os_queue->queue->type != QUEUE_TYPE_MUTEX || os_queue->queue->owner == NULL){
                                osQueueResumeTask(os_queue,(List_t*)&osDelayByWriteQueueList,queueRet);
                            }
						}
        }else{
            /* dont wait for writeable just return false */
            os_resume();
            return OS_RET_FALSE;
        }

    }else{
       queueRet = o_queue_read(os_queue->queue,item);
       if(os_queue->queue->type != QUEUE_TYPE_MUTEX || os_queue->queue->owner == NULL){
        osQueueResumeTask(os_queue,(List_t*)&osDelayByWriteQueueList,queueRet);
       }
       
    }
    os_resume();
    return queueRet == QUEUE_OK ? OS_RET_TRUE : OS_RET_FALSE ;
}

