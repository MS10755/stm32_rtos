#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <stdint.h>
#include <stdbool.h>

typedef uint16_t queueSize_t ;
typedef int8_t queueRet_t;
#define QUEUE_MUTEX_MAX_RECUR 0xFFFF
enum{
    QUEUE_OK = 0,
    QUEUE_FAILED = -100,
};
typedef enum{
	QUEUE_TYPE_MUTEX,
    QUEUE_TYPE_COUNT,
    QUEUE_TYPE_NORMAL,
}Queue_type_t;

typedef struct Queue
{
    Queue_type_t type;
    queueSize_t capacity;
    queueSize_t itemSizeBytes;
    queueSize_t len;//useful for counter/mutex queue.
    bool isFull;
    void * self;
    void * owner;// for Mutex
    void * p_read;
    void * p_write;
    void * storage;
}Queue_t;

Queue_t * queue_create(queueSize_t size,queueSize_t itemSize,Queue_type_t type);
queueSize_t queue_get_len(Queue_t * queue,void *parms);
bool queue_isFull(Queue_t * queue,void *parms);
queueRet_t queue_write(Queue_t * queue,void * item);
queueRet_t queue_read(Queue_t * queue,void * item);
#define QUEUE_SET_OWNER(q,o) ((q)->owner = o)
#endif
