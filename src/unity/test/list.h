#ifndef __LISTS__H__
#define __LISTS__H__
#include <stdint.h>
#include <assert.h>

typedef struct ListNode_s
{
    uint32_t value;
    struct ListNode_s * prev;
    struct ListNode_s * next;
    void * data;
    void * owner;   //Point to the owning list
}ListNode_t;

typedef struct List_s{
    ListNode_t * head;
    ListNode_t * tail;
    ListNode_t * iterator;
    uint32_t numberOfItems;
}List_t;

void List_Initialise(List_t * list);
void List_InsertEnd(List_t * list,ListNode_t * node);
void List_Remove(ListNode_t * node);
void List_InserOrderInc(List_t * list,ListNode_t * node);
void List_InserOrderDec(List_t * list,ListNode_t * node);


#define LIST_ITERATOR_BEGIN(l)  ((l)->iterator = (l)->head)
#define LIST_ITERATOR_GET(l)  ((l)->iterator)
#define LIST_ITERATOR_NEXT(l)  ((l)->iterator,(l)->iterator = (l)->iterator->next)
#define LIST_ITERATOR_HAS(l)  ((l)->iterator ? 1 : 0)
#define LIST_ITERATOR_HAS_NEXT(l)  ((l)->iterator->next ? 1 : 0)
#define LIST_GET_HEAD(l)  ((l)->head)
#define LIST_GET_END(l)  ((l)->tail)
#define LIST_GET_LEN(l)  ((l)->numberOfItems)
#define LIST_NODE_SET_VALUE(n,v)  ((n)->value = v)
#define LIST_NODE_GET_VALUE(n,v)  ((n)->value)
#define LIST_NODE_SET_DATA(n,d)  ((n)->data = d)
#define LIST_NODE_GET_DATA(n,d)  ((n)->data)
#define LIST_NODE_GET_OWNER(n,d)  ((n)->owner)

#endif // !__LIST_H__
