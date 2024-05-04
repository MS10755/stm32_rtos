#include "bsp.h"
#include <stdio.h>
#include <stdbool.h>
#include "cm_backtrace.h"
#include "stm32f10x.h"                  // Device header


void fault_test_by_div0(void) {
    volatile int * SCB_CCR = (volatile int *) 0xE000ED14; // SCB->CCR
    int x, y, z;

    *SCB_CCR |= (1 << 4); /* bit4: DIV_0_TRP. */

    x = 10;
    y = 0;
    z = x / y;
    printf("z:%d\n", z);
}

void function_c(){
	fault_test_by_div0();
}

void function_b(){
	function_c();
}

void function_a(){
	function_b();
}
void uart_init(void);
int main(){
	uart_init();
	function_a();

	while(1);

}