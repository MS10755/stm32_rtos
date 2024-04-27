// Header:
// File Name: list.c
// Author:
// Date:
/*
	说明：当前文件是对可变列表（链表）的插入，删除，求长度的基本操作，不涉及动态内存，因此可以灵活支持静态和动态两种模式。
				节点的内存操作需要调用者自行实现。
*/
#include "list.h"

/*********************************************************************************
  *Function:  list_init
  * Description：  初始化一个节点
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
list_node_t * list_node_init(list_node_t *node){
**********************************************************************************/
list_dis_t * list_init(list_dis_t *dis,list_node_t * first_node){
    assert((dis && first_node));
    first_node->dis = dis;
    dis->node_num = 1;
    dis->head = first_node;
    dis->tail = first_node;
    return dis;
}

/*********************************************************************************
  *Function:  list_set_pdata
  * Description：  向节点写入数据
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
list_node_t * list_node_init(list_node_t *node){
**********************************************************************************/
list_node_t * list_set_pdata(list_node_t *node,void * pdata){
    assert((node));
		node->pData = pdata;
    return node;
}


/*********************************************************************************
  *Function:  list_get_pdata
  * Description：  获取节点存储的数据
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
list_node_t * list_node_init(list_node_t *node){
**********************************************************************************/
void * list_get_pdata(list_node_t *node){
    assert((node));
    return node->pData;
}

/*********************************************************************************
  *Function:  list_insert_before 
  * Description：  在当前节点前插入新节点
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  新节点
  *Others:  //其他说明
**********************************************************************************/
list_node_t * list_insert_before(list_node_t * pos,list_node_t *node){
    assert((pos && node && pos->dis));
    if(pos == NULL){
        return NULL;
    }

    list_node_t * pos_before = pos->prev_node;
    pos->prev_node = node;
    node->prev_node = pos_before;
    node->next_node = pos;

    if(pos_before){
        pos_before->next_node = node;
    }

    list_dis_t * dis = pos->dis;
    if(dis->head == pos){
        dis->head = node;
    }

    node->dis = dis;
    dis->node_num++;
    return node;
}

/*********************************************************************************
  *Function:  list_insert_after 
  * Description：  在当前节点之后插入新节点
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  新节点
  *Others:  //其他说明
**********************************************************************************/

list_node_t * list_insert_after(list_node_t * pos,list_node_t *node){
    assert((pos && node && pos->dis));
	if(pos == NULL){
		return NULL;
	}
	
    list_node_t * pos_after = pos->next_node;
    pos->next_node = node;
    node->next_node = pos_after;
	node->prev_node = pos;

	if(pos_after){
		pos_after->prev_node = node;
	}
	
    list_dis_t * dis = pos->dis;
    if(dis->tail == pos){
        dis->tail = node;
    }

    node->dis = dis;
    dis->node_num++;
	return node;
}

/*********************************************************************************
  *Function:  list_insert_dis_before 
  * Description：  将一条整链插到某个节点之后
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  新节点
  *Others:  //其他说明
**********************************************************************************/
list_dis_t * list_insert_dis_before(list_node_t * pos,list_dis_t *dis){
	assert((pos && dis));
	
  list_node_t * pos_before = pos->prev_node;
	
    pos->prev_node = dis->tail;
    dis->head->prev_node = pos_before;
		dis->tail->next_node = pos;

	if(pos_before){
		pos_before->next_node = dis->head;
	}
	
    list_dis_t * pos_dis = pos->dis;
    if(pos_dis->head == pos){
        pos_dis->head = dis->head;
    }

		list_node_t * p_node = dis->head;
		while(p_node!=dis->tail){
			p_node->dis = pos->dis;
			p_node = p_node->next_node;
		}
		if(p_node == dis->tail){
			p_node->dis = pos->dis;
		}
		
    pos_dis->node_num += dis->node_num;
	
return pos->dis;
}

/*********************************************************************************
  *Function:  list_insert_dis_after 
  * Description：  将一条整链插到某个节点之后
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  新节点
  *Others:  //其他说明
**********************************************************************************/
list_dis_t * list_insert_dis_after(list_node_t * pos,list_dis_t *dis){
	assert((pos && dis));
	
  list_node_t * pos_after = pos->next_node;
	
    pos->next_node = dis->head;
    dis->tail->next_node = pos_after;
	dis->head->prev_node = pos;

	if(pos_after){
		pos_after->prev_node = dis->tail;
	}
	
    list_dis_t * pos_dis = pos->dis;
    if(pos_dis->tail == pos){
        pos_dis->tail = dis->tail;
    }

		list_node_t * p_node = dis->head;
		while(p_node!=dis->tail){
			p_node->dis = pos->dis;
			p_node = p_node->next_node;
		}
		if(p_node == dis->tail){
			p_node->dis = pos->dis;
		}

    pos_dis->node_num += dis->node_num;
	
return pos->dis;
}
/*********************************************************************************
  *Function:  list_delete
  * Description：  释放链表单个节点，只是从链表中移除，并不释放内存。
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  返回被删除节点的地址
  *Others:  //其他说明
**********************************************************************************/
list_node_t * list_delete(list_node_t * node){
    assert((node && node->dis));
    list_dis_t * dis = node->dis;
	list_node_t * prev_node = node->prev_node;
	list_node_t * next_node = node->next_node;
	
	if(prev_node){
		prev_node->next_node = next_node;
        if(dis->tail == node){
            dis->tail = prev_node;
        }
	}
	
	if(next_node){
		next_node->prev_node = prev_node;
        if(dis->head == node){
            dis->head = next_node;
        }
	}
	
	if(dis->head == node){
		dis->head = NULL;
	}
	
	if(dis->tail == node){
		dis->tail = NULL;
	}
    dis->node_num--;
    node->dis = NULL;
    node->prev_node = NULL;
    node->next_node = NULL;
	return node;
}

/*********************************************************************************
  *Function:  list_delete_free 
  * Description：  释放链表单个节点，并调用用户释放函数来释放内存
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  释放返回 NULL,未释放内存返回 node 地址
  *Others:  //其他说明
**********************************************************************************/
list_node_t * list_delete_free(list_node_t * node,void *(*free_fun)(void * node)){
	list_delete(node);
	if(free_fun){
		return free_fun(node);
	}
	return node;
}

/*********************************************************************************
  *Function:  list_length_between
  * Description：  获取同一条链表的两个节点之间的长度
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
**********************************************************************************/
uint16_t list_length_between(list_node_t * from,list_node_t * to){
    assert((from && to && (from->dis == to->dis)));
    list_dis_t * dis = from->dis;
    if((dis->head == from && dis->tail == to) || (dis->head == to && dis->tail == from)){
        return dis->node_num;
    }
    uint16_t len = 0;
    list_node_t * p_node = NULL;
   //from front to back.
    p_node = from;
    while(1){
        len++;
        if(p_node == to){
            return len;
        }else if(p_node == dis->tail && to!= dis->tail){
            len = 0;
            break;
        }
        p_node = p_node->next_node;
    }

    //from back to front.
    p_node = from;
    while(1){
        len++;
        if(p_node == to){
            return len;
        }else if(p_node == dis->head && to!= dis->head){
            len = 0;
            break;
        }
        p_node = p_node->prev_node;
    }
    return len;
}

/*********************************************************************************
  *Function:  list_length
  * Description：  获取当前节点所属链表的长度
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
**********************************************************************************/
uint16_t list_length(list_node_t * node){
    assert(node && node->dis);
    if(node == NULL){
        return 0;
    }
    return list_length_between(node->dis->head,node->dis->tail);
}


/*********************************************************************************
  *Function:  list_length
  * Description：  返回从链表头当当前节点的长度
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
**********************************************************************************/
uint16_t list_length_before(list_node_t * node){
    assert(node);
    if(node == NULL){
        return 0;
    }
    return list_length_between(node->dis->head,node);
}


/*********************************************************************************
  *Function:  list_length
  * Description：  返回从当前节点到尾节点的长度
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
**********************************************************************************/
uint16_t list_length_after(list_node_t * node){
	if(node == NULL){
		return 0;
	}
    return list_length_between(node,node->dis->tail);
}

/*********************************************************************************
  *Function:  list_contains
  * Description：  返回从当前节点到尾节点的长度
  *Calls:  //被本函数调用的函数清单
  *Called By:  //调用本函数的清单
  *Input:  //输入参数的说明，包括每个参数的作用、取值说明及参数间关系
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
**********************************************************************************/
list_node_t * list_contains(list_dis_t * dis,list_node_t * node){
    return dis == node->dis ? node: NULL;
}

list_dis_t * list_insert_dis_order_inc(list_node_t * node,list_dis_t *dis){
	assert(node);
	assert(dis);
	list_node_t * it = NULL;
	for(it = dis->head;(it != NULL) && (it->orderNumber <= node->orderNumber);it=it->next_node){
	}
	if(it){
		list_insert_after(it,node);
	}else{
		list_insert_after_to_dis(node,dis);
	}
	return dis;
}

list_node_t * list_insert_after_to_dis(list_node_t * node,list_dis_t *dis){
	if(dis->node_num == 0 && dis->head == NULL && dis->tail == NULL){
		list_init(dis,node);
	}else{
		list_insert_after(dis->tail,node);
	}
	return node;
}

