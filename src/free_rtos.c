
__asm void xPortPendSVHandler( void )
{
//    extern uxCriticalNesting;
    extern pxCurrentTCB;
//    extern vTaskSwitchContext;

/* *INDENT-OFF* */
//    PRESERVE8

    mrs r0, psp
    isb

    ldr r3, =pxCurrentTCB /* Get the location of the current TCB. */
    ldr r2, [ r3 ]

    stmdb r0 !, { r4 - r11 } /* Save the remaining registers. */
    str r0, [ r2 ] /* Save the new top of stack into the first member of the TCB. */

    stmdb sp !, { r3, r14 }
    mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
    msr basepri, r0
    dsb
    isb
    bl vTaskSwitchContext
    mov r0, #0
    msr basepri, r0
    ldmia sp !, { r3, r14 }

    ldr r1, [ r3 ]
    ldr r0, [ r1 ] /* The first item in pxCurrentTCB is the task top of stack. */
    ldmia r0 !, { r4 - r11 } /* Pop the registers and the critical nesting count. */
    msr psp, r0
    isb
    bx r14
    nop
/* *INDENT-ON* */
}



BaseType_t xTaskIncrementTick( void )
{
TCB_t * pxTCB;
TickType_t xItemValue;
BaseType_t xSwitchRequired = pdFALSE;

	/* 每个滴答周期调用，增加滴答值，并检查是否有任务能解除阻塞 */

	if( uxSchedulerSuspended == ( UBaseType_t ) pdFALSE )
	{
		/* Minor optimisation.  The tick count cannot change in this
		block. */
		const TickType_t xConstTickCount = xTickCount + ( TickType_t ) 1;

		/* Increment the RTOS tick, switching the delayed and overflowed
		delayed lists if it wraps to 0. */
		xTickCount = xConstTickCount;

		if( xConstTickCount == ( TickType_t ) 0U ) /*lint !e774 'if' does not always evaluate to false as it is looking for an overflow. */
		{
			taskSWITCH_DELAYED_LISTS();
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}

		/* See if this tick has made a timeout expire.  Tasks are stored in
		the	queue in the order of their wake time - meaning once one task
		has been found whose block time has not expired there is no need to
		look any further down the list. */
		if( xConstTickCount >= xNextTaskUnblockTime )
		{
			for( ;; )
			{
				if( listLIST_IS_EMPTY( pxDelayedTaskList ))
				{
					/* The delayed list is empty.  Set xNextTaskUnblockTime
					to the maximum possible value so it is extremely
					unlikely that the
					if( xTickCount >= xNextTaskUnblockTime ) test will pass
					next time through. */
					xNextTaskUnblockTime = portMAX_DELAY; /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
					break;
				}
				else
				{
					/* 获取延迟队列，延迟值 */
					pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );
					xItemValue = listGET_LIST_ITEM_VALUE( &( pxTCB->xStateListItem ) );

					if( xConstTickCount < xItemValue )
					{
						/* It is not time to unblock this item yet, but the
						item value is the time at which the task at the head
						of the blocked list must be removed from the Blocked
						state -	so record the item value in
						xNextTaskUnblockTime. */
						xNextTaskUnblockTime = xItemValue;
						break;
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}

					/* 从延迟队列中移除任务 */
					( void ) uxListRemove( &( pxTCB->xStateListItem ) );

					/* Is the task waiting on an event also?  If so remove
					it from the event list. */
					if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
					{
						( void ) uxListRemove( &( pxTCB->xEventListItem ) );
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}

					/*  将任务加入到就绪队列 */
					prvAddTaskToReadyList( pxTCB );
				}           
			}
		}

	}
	else
	{
		++uxPendedTicks;
	}

	return xSwitchRequired;
}

void xPortSysTickHandler( void )
{
	/* The SysTick runs at the lowest interrupt priority, so when this interrupt
	executes all interrupts must be unmasked.  There is therefore no need to
	save and then restore the interrupt mask value as its value is already
	known. */
	portDISABLE_INTERRUPTS();
	{
		/* Increment the RTOS tick. */
		if( xTaskIncrementTick() == pdTRUE )
		{
			/* 请求PendSV中断，进行上下文切换 */
			portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
		}
	}
	portENABLE_INTERRUPTS();
}


void vTaskSwitchContext( void )
{
	if( uxSchedulerSuspended == ( UBaseType_t ) pdTRUE )
	{
		/* 调度器被挂起，现在不允许上下文切换 */
		xYieldPending = pdTRUE;
	}
	else
	{
		xYieldPending = pdFALSE;

		/* Select a new task to run using either the generic C or port
		optimised asm code. */
		UBaseType_t uxTopPriority;																		
		/* Find the highest priority list that contains ready tasks. */								
		portGET_HIGHEST_PRIORITY( uxTopPriority, uxTopReadyPriority );								
		listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );		
	}
}