#ifndef __OS_SEMAPHORE_H__
#define __OS_SEMAPHORE_H__
#include "os_queue.h"

#define os_semaphore_t os_queue_t

#define os_semaphore_mutex_create()	os_queue_create(0,0,QUEUE_TYPE_MUTEX)
#define os_semaphore_binary_create(n)	os_queue_create(2,0,QUEUE_TYPE_COUNT)
#define os_semaphore_count_create(n)	os_queue_create(n,0,QUEUE_TYPE_COUNT)
#define os_semaphore_give(s,wait)	os_queue_write(s,0,wait)
#define os_semaphore_take(s,wait)	os_queue_read(s,0,wait)
#define os_semaphore_mutex_lock(s,wait)	os_queue_write(s,0,wait)
#define os_semaphore_mutex_unlock(s,wait)	os_queue_read(s,0,wait)

#endif

