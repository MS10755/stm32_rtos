#include "list.h"
#include <stdio.h>
/*
void List_Initialise(List_t * list);
void List_InsertEnd(List_t * list,ListNode_t * node);
void List_Remove(ListNode_t * node);
void List_InserOrderInc(List_t * list,ListNode_t * node);
void List_InserOrderDec(List_t * list,ListNode_t * node);
*/
List_t list1;
ListNode_t node1;
ListNode_t node2;
ListNode_t node3;
ListNode_t node4;
ListNode_t node5;

int main(int argv,char *argc[]){
    List_Initialise(&list1);

    // assert(list1.head == NULL);
    // assert(list1.tail == NULL);
    // assert(list1.iterator == NULL);
    // assert(list1.numberOfItems == 0);

    // // List_InsertEnd(&list1,&node1);

    // assert(list1.head == &node1);
    // assert(list1.tail == &node1);
    // assert(list1.iterator == NULL);
    // assert(list1.numberOfItems == 1);
    // assert(node1.next == NULL);
    // assert(node1.prev == NULL);
    // assert(node1.owner == &list1);

    node1.value = 923;
    node2.value = 100;//
    node3.value = 2320;//
    node4.value = 912;//
    node5.value = 200;//
    List_InserOrderInc(&list1,&node1);
    List_InserOrderInc(&list1,&node2);
    List_InserOrderInc(&list1,&node3);
    List_InserOrderInc(&list1,&node4);
    List_InserOrderInc(&list1,&node5);
    // List_InsertEnd(&list1,&node1);
    // List_InsertEnd(&list1,&node2);
    // List_InsertEnd(&list1,&node3);
    // List_InsertEnd(&list1,&node4);
    // List_InsertEnd(&list1,&node5);
    LIST_ITERATOR_BEGIN(&list1);

    while (LIST_ITERATOR_HAS(&list1))
    {   
        ListNode_t * node = LIST_ITERATOR_GET(&list1);
        printf("node value: %d\r\n",node->value);
        fflush(stdout);
        LIST_ITERATOR_NEXT(&list1);
    }
    

    List_Remove(&node2);
    List_Remove(&node3);

    List_Remove(&node1);
    List_Remove(&node5);
    List_Remove(&node4);
    #include <stdlib.h>
    calloc(1,2);
    return 0;
}