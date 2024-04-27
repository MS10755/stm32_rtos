#ifndef __LIST_H__
#define __LIST_H__
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct list_dis;
typedef struct list_node{
	struct list_node * prev_node;
	struct list_node * next_node;
    struct list_dis * dis;
	void * pData;
	uint32_t orderNumber;
}list_node_t;

typedef struct list_dis{
    list_node_t * head;
    list_node_t * tail;
    uint16_t node_num;
}list_dis_t;

#ifdef __cplusplus
extern "C" {
#endif
list_dis_t * list_init(list_dis_t *dis,list_node_t * first_node);
list_node_t * list_set_pdata(list_node_t *node,void * pdata);
void * list_get_pdata(list_node_t *node);
list_node_t * list_insert_before(list_node_t * pos,list_node_t *node);
list_node_t * list_insert_after(list_node_t * pos,list_node_t *node);
list_node_t * list_insert_after_to_dis(list_node_t * node,list_dis_t *dis);
list_dis_t * list_insert_dis_before(list_node_t * pos,list_dis_t *dis);
list_dis_t * list_insert_dis_after(list_node_t * pos,list_dis_t *dis);
list_dis_t * list_insert_dis_order_inc(list_node_t * node,list_dis_t *dis);
list_node_t * list_delete(list_node_t * node);
list_node_t * list_delete_free(list_node_t * node,void *(*free_fun)(void * node));
uint16_t list_length_between(list_node_t * from,list_node_t * to);
uint16_t list_length(list_node_t * node);
uint16_t list_length_before(list_node_t * node);
uint16_t list_length_after(list_node_t * node);
list_node_t * list_contains(list_dis_t * dis,list_node_t * node);
#ifdef __cplusplus
}
#endif

#endif
