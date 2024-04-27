#include "queue.h"
#include "assert.h"
#include <stdio.h>
 Queue_t *queue;
int main(int argv ,char argc[]){
    float a = 1.1;
    float b = 1.2;
    float c = 1.3;
    float d = 1.4;
    float e = 1.5;
    float f = 1.6;
    queue = queue_create(5,sizeof(float),QUEUE_TYPE_COUNT);

    assert(queue_isFull(queue) == false);
    queue_write(queue,&a);
    assert(queue_get_len(queue) == 1);
    assert(queue_isFull(queue) == false);
    queue_write(queue,&b);
    assert(queue_get_len(queue) == 2);
    assert(queue_isFull(queue) == false);
    queue_write(queue,&c);
    assert(queue_get_len(queue) == 3);
    assert(queue_isFull(queue) == false);
    queue_write(queue,&d);
    assert(queue_get_len(queue) == 4);
    assert(queue_isFull(queue) == false);
    queue_write(queue,&e);
    assert(queue_get_len(queue) == 5);
    assert(queue_isFull(queue) == true);

    assert(queue_write(queue,&e) == QUEUE_FAILED);
    assert(queue_get_len(queue) == 5);
    assert(queue_isFull(queue) == true);
    assert(queue_write(queue,&e) == QUEUE_FAILED);
    assert(queue_get_len(queue) == 5);
    assert(queue_isFull(queue) == true);
    float r;
    queue_read(queue,&r);
    assert(queue_get_len(queue) == 4);
    assert(queue_isFull(queue) == false);
    queue_read(queue,&r);
    assert(queue_get_len(queue) == 3);
    assert(queue_isFull(queue) == false);
    queue_read(queue,&r);
    assert(queue_get_len(queue) == 2);
    assert(queue_isFull(queue) == false);
    queue_read(queue,&r);
    assert(queue_get_len(queue) == 1);
    assert(queue_isFull(queue) == false);
    queue_read(queue,&r);
    assert(queue_get_len(queue) == 0);
    assert(queue_isFull(queue) == false);
    assert(queue_read(queue,&r) == QUEUE_FAILED);
    assert(queue_get_len(queue) == 0);
    assert(queue_isFull(queue) == false);
    assert(queue_read(queue,&r) == QUEUE_FAILED);
    assert(queue_get_len(queue) == 0);
    assert(queue_isFull(queue) == false);
    printf("pass all test!\r\n");
    while (1)
    {
        /* code */
    }
    
    return 0;
}