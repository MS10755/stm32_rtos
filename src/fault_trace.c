#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define FAULT_TRACE_USE_OS 1
#define FAULT_TRACE_OS_TYPE_MY_OS
#define CALL_MAX 32
extern char STACK$$Base;
extern char STACK$$Limit;
extern char Image$$ER_IROM1$$Base;
extern char Image$$ER_IROM1$$Limit;
uint32_t stack_start = (uint32_t)&STACK$$Base;
uint32_t stack_end = (uint32_t)&STACK$$Limit;
uint32_t rom_start = (uint32_t)&Image$$ER_IROM1$$Base;
uint32_t rom_end = (uint32_t)&Image$$ER_IROM1$$Limit;
const char* proj_name = NULL;
bool sp_overFlow = false;
uint8_t stack_depth = 0;
uint32_t stack_call[CALL_MAX];

bool check_sp(uint32_t sp){
	if(sp < stack_start || sp > stack_end){
		sp_overFlow = true;
	}
	return sp_overFlow;
}
#define POP(sp,n) (sp+=n*4)
#define SP_VAL(sp) (*(uint32_t*)sp)
#define in_rom(addr) (addr >= rom_start && addr <=rom_end ? 1:0)

static bool disassembly_ins_is_bl_blx(uint32_t addr) {
    uint16_t ins1 = *((uint16_t *)addr);
    uint16_t ins2 = *((uint16_t *)(addr + 2));

#define BL_INS_MASK         0xF800
#define BL_INS_HIGH         0xF800
#define BL_INS_LOW          0xF000
#define BLX_INX_MASK        0xFF00
#define BLX_INX             0x4700

    if ((ins2 & BL_INS_MASK) == BL_INS_HIGH && (ins1 & BL_INS_MASK) == BL_INS_LOW) {
        return true;
    } else if ((ins2 & BLX_INX_MASK) == BLX_INX) {
        return true;
    } else {
        return false;
    }
}
#ifdef FAULT_TRACE_USE_OS
void get_stack_info(void){
#ifdef FAULT_TRACE_OS_TYPE_MY_OS
#include "os.h"
#include "os_task.h"
extern os_tcb_t * running_tcb;
	stack_start = (uint32_t)running_tcb->stack;
	stack_end = (uint32_t)&running_tcb->stack[running_tcb->stack_size];
#endif

}
#endif

void fault_init(const char * proj){
	proj_name = proj;
}

void fault_trace(uint32_t sp){
	get_stack_info();
	if(check_sp(sp) == true){
		printf("sp over flow!!!\r\n");
	}else{
		 POP(sp,6);// sp->PC
		 stack_call[stack_depth++] = SP_VAL(sp);
		 POP(sp,2);
		for( ; sp<=stack_end ; POP(sp,1)){
			uint32_t val = SP_VAL(sp);
			if(val%2==0 || !in_rom(val)){
				continue;
			}
			uint32_t pc = (val-1)-4;
			if(disassembly_ins_is_bl_blx(pc) == false){
				continue;
			}
			/*
				PC:BL	funcion
				LR:val
			*/
			stack_call[stack_depth++] = pc;
		}
		printf("HardFault_Handler:\r\naddr2line -e %s.axf -afpiC ",proj_name);
		for(int i=0;i<stack_depth;i++){
			printf(" %08X",stack_call[i]);
		}
	}
	while(1);
}

__asm void HardFault_Handler() 
{
	IMPORT fault_trace
#if FAULT_TRACE_USE_OS
	MRS R0,PSP
#else
	MRS R0,MSP
#endif
	BL fault_trace
}
