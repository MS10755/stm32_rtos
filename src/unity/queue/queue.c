#include <queue.h>
#include <stdlib.h>
#include <string.h>
#define os_malloc(size)   malloc(size)
#define os_calloc(n,size)   calloc(n,size)
#define os_memove(dest,src,size) memmove(dest,src,size)


/**
 * @brief  : create an queue
 * 
 * @param size : total number of items
 * @param itemSize : item size in bytes
 * @param type : counter queue or storage queue
 * @return Queue_t* : queue ptr
 */
Queue_t * queue_create(queueSize_t size,queueSize_t itemSize,Queue_type_t type){
    uint32_t totalSize;
    if(type == QUEUE_TYPE_NORMAL){
        totalSize = sizeof(Queue_t) + size * itemSize;
    }else{
        totalSize = sizeof(Queue_t);
    }
    void * p_queue = os_malloc(totalSize);
    if(p_queue == NULL) return NULL;

    Queue_t * newQueue = ( Queue_t *)p_queue;
    newQueue->type = type;
    newQueue->capacity = size;
    newQueue->len = 0;
    newQueue->itemSizeBytes = itemSize;
    newQueue->other = NULL;
    newQueue->isFull = false;
    if(type == QUEUE_TYPE_NORMAL){
        newQueue->storage = p_queue + sizeof(Queue_t);
        newQueue->p_write = newQueue->storage;
        newQueue->p_read =  newQueue->storage;
    }else{
        newQueue->storage = NULL;
        newQueue->p_write = NULL;
        newQueue->p_read =  NULL;
    }

    newQueue->self = newQueue;
    return newQueue;
}

static queueSize_t queue_convert_index(Queue_t * queue,void * p){
    return (uint32_t)(p - queue->storage)/queue->itemSizeBytes;
}

static queueSize_t queue_get_read_index(Queue_t * queue){
    return queue_convert_index(queue,queue->p_read);
}

static queueSize_t queue_get_write_index(Queue_t * queue){
    return queue_convert_index(queue,queue->p_write);
}

static queueRet_t queue_move_write_index(Queue_t * queue){
    if( queue->isFull == true){
        return QUEUE_FAILED;
    }

    queueSize_t readIndex = queue_get_read_index(queue);
    queueSize_t writeIndex = queue_get_write_index(queue);

    queueSize_t nextIndex;
    nextIndex = writeIndex + 1;
    if(nextIndex == queue->capacity){
        nextIndex = 0;
    }
    if(nextIndex  == readIndex){
        queue->isFull = true;
    }

    queue->p_write = queue->storage + queue->itemSizeBytes * nextIndex;
    return QUEUE_OK;
}


static queueRet_t queue_move_read_index(Queue_t * queue){

    queueSize_t readIndex = queue_get_read_index(queue);
    queueSize_t writeIndex = queue_get_write_index(queue);
    queueSize_t nextIndex;
    nextIndex = readIndex + 1;
    if(nextIndex == queue->capacity){
        nextIndex = 0;
    }

    queue->isFull = false;
    queue->p_read = queue->storage + queue->itemSizeBytes * nextIndex;
    return QUEUE_OK;
}


/**
 * @brief  : write one item data to queue
 * 
 * @param queue : queue will be writen
 * @param item : item data will be write
 * @return queueRet_t : operation result
 */
queueRet_t queue_write(Queue_t * queue,void * item){
    queueRet_t ret = QUEUE_FAILED;
    if(queue_isFull(queue)) return QUEUE_FAILED;
    if(queue->type == QUEUE_TYPE_COUNT){
        if(queue->len < queue->capacity){
            queue->len++;

            ret = QUEUE_OK;
        }else{
            // cannot write queue,deal with it;
            while (1)
            {
                /* code */
            }
        }

    }else if(queue->type == QUEUE_TYPE_NORMAL){
        os_memove(queue->p_write,item,queue->itemSizeBytes);
        ret = queue_move_write_index(queue);
    }

    return ret;
}

/**
 * @brief  : read an item data from queue
 * 
 * @param queue : queue will be read
 * @param item : item straoge space
 * @return queueRet_t : operation result
 */
queueRet_t queue_read(Queue_t * queue,void * item){
    queueRet_t ret = QUEUE_FAILED;
    if(queue_get_len(queue) == 0) return QUEUE_FAILED;

    if(queue->type == QUEUE_TYPE_COUNT){
        if(queue->len > 0){
            queue->len--;

            ret = QUEUE_OK;
        }else{
            // cannot write queue,deal with it;
            while (1)
            {
                /* code */
            }
        }

    }else if(queue->type == QUEUE_TYPE_NORMAL){
        os_memove(item,queue->p_read,queue->itemSizeBytes);
        ret = queue_move_read_index(queue);
    }

    return ret;
}

/**
 * @brief  : get number of queue holding
 * 
 * @param queue : 
 * @return queueSize_t : 
 */
queueSize_t queue_get_len(Queue_t * queue){
    if(queue->type == QUEUE_TYPE_COUNT){
        return queue->len;
    }else if(queue->type == QUEUE_TYPE_NORMAL){
        queueSize_t writeIndex = queue_get_write_index(queue);
        queueSize_t readIndex = queue_get_read_index(queue);
        if(queue->isFull == true){
            return queue->capacity;
        }
        if(readIndex == writeIndex){
            return 0;
        }else if(writeIndex > readIndex){
            return writeIndex - readIndex;
        }else if(writeIndex < readIndex){
            return queue->capacity + writeIndex - readIndex;
        }
    }
    
}

/**
 * @brief  : check if the queue full or not
 * 
 * @param queue : 
 * @return true : 
 * @return false : 
 */
bool queue_isFull(Queue_t * queue){
    if(queue_get_len(queue) == queue->capacity){
        return true;
    }else{
        return false;
    }

}