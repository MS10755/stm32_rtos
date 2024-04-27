	AREA |.text|,CODE
	EXTERN running_tcb
	EXTERN switch_tcb
	IMPORT pendSVC_exit
	IMPORT pendSVC_enter
		
;pendSVC_exit
;	BX LR

PendSV_Handler
	EXPORT PendSV_Handler
	CPSID i
	
	
	PUSH {R0,LR}
	LDR R0,=running_tcb
	LDR R0,[R0]	;R0 = running_tcb
	BL pendSVC_enter
	POP {R0,LR}

	MRS R0,PSP
	CBZ R0,restore_context

save_context
	;save current tcb
	;SUBS R0,R0,#0x20
	STMDB R0!,{R4 - R11}	;copy R4-R11 to p_stack
	LDR R1,=running_tcb
	LDR R1,[R1]	;R0 = running_tcb
	STR R0,[R1] ; R0 ->p_stack
	
restore_context
	;restore highPority task tcb
	LDR R0,=switch_tcb
	LDR R0,[R0]	;R0 = switch_tcb
	LDR R0,[R0] ; R0 ->p_stack
	LDM R0!,{R4 - R11} ;copy p_statck{R4-R11} to CPU
	MSR PSP,R0
	ISB	
	
	;//running_tcb = switch_tcb
	LDR R0,=switch_tcb;
	LDR R0,[R0]
	LDR R1,=running_tcb;
	STR R0,[R1]
	
	PUSH {R0,LR}
	BL pendSVC_exit
	POP {R0,LR}
	
	ORR LR,LR,#0x0D	;Thumb status,USE PSP,return thread mode
	
	CPSIE i
	BX LR	;//should not be here


init_cpu
	EXPORT init_cpu
	LDR R0,=0x00000000
	MSR PSP,R0
	BX LR
	NOP
	
	END
