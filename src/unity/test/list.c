#include "list.h"

void List_Initialise(List_t * list){
    assert(list);
    list->head = NULL;
    list->tail = NULL;
    list->iterator = NULL;
    list->numberOfItems = 0;
}

/**
 * @brief  : insert a node before pos_node
 * 
 * @param pos : pos_node
 * @param node : new node
 */
static void list_InsertPosBefore(ListNode_t * pos,ListNode_t * node){
    if(pos == NULL || node == NULL) return;
    node->prev = pos->prev;
    node->next = pos;

    if(node->prev){
        node->prev->next =node;
    }
    pos->prev = node;
    node->owner = pos->owner;
}

/**
 * @brief  : insert a node after pos_node
 * 
 * @param pos : pos_node
 * @param node : new node
 */
static void list_InsertPosAfter(ListNode_t * pos,ListNode_t * node){
    if(pos == NULL || node == NULL) return;
    node->prev = pos;
    node->next = pos->next;

    if(pos->next){
        pos->next->prev = node;
    }

    pos->next = node;

    node->owner = pos->owner;
}

void List_InsertEnd(List_t * list,ListNode_t * node){
    assert(list);
    assert(node);

    if(list->numberOfItems == 0){
        list->head = node;
        list->tail = node;
    }else{
        if(list->tail){
            list_InsertPosAfter(list->tail,node);
            list->tail = node;
        }else{
            // couldn't run to here.
            while (1);
        }

    }
    
    node->owner = list;

    list->numberOfItems++;
}

void List_InserOrderDec(List_t * list,ListNode_t * node){
    assert(list);
    assert(node);
    //6
    //4
    //[4/3]
    //2
    //null
    if(list->numberOfItems == 0){
        List_InsertEnd(list,node);
        return;
    }

    ListNode_t * it = list->head;
    for(;it != NULL;it=it->next){
        if(it->value <= node->value){
            break;
        }
    }

    if(it){
        list_InsertPosBefore(it,node);
        if(it == list->head){
            list->head = node;
        }
    }else{
        list_InsertPosAfter(list->tail,node);
        list->tail = node;
    }
    node->owner = list;
    list->numberOfItems++;
}

void List_InserOrderInc(List_t * list,ListNode_t * node){
    assert(list);
    assert(node);
    //2
    //3
    //[4/3]
    //6
    //null
    if(list->numberOfItems == 0){
        List_InsertEnd(list,node);
        return;
    }

    ListNode_t * it = list->head;
    for(;it != NULL;it=it->next){
        if(it->value >= node->value){
            break;
        }
    }

    if(it){
        list_InsertPosBefore(it,node);
        if(it == list->head){
            list->head = node;
        }
    }else{
        list_InsertPosAfter(list->tail,node);
        list->tail = node;
    }
    node->owner = list;
    list->numberOfItems++;
}


void List_Remove(ListNode_t * node){
    assert(node);
    assert(node->owner);

    List_t * ownerList = node->owner;
    ListNode_t * prevNode = node->prev;
    ListNode_t * nextNode = node->next;
    
    if(prevNode){
        prevNode->next = nextNode;
    }

    if(nextNode){
        nextNode->prev = prevNode;
    }

    node->prev = NULL;
    node->next = NULL;
    node->owner = NULL;

    if(ownerList->head == node){
        ownerList->head = nextNode;
    }

    if(ownerList->tail == node){
        ownerList->tail = prevNode;
    }

    ownerList->numberOfItems--;
}