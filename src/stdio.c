#include "stm32f10x.h"                  // Device header
#include <stdio.h>
/**/
#if 1
//#define AC6_ENABLE
// 	  
#ifdef AC6_ENABLE
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	
__ASM(".global __use_no_semihosting");      
#else                                                       //AC5_ENABLE
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
//#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
};
#endif
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR & USART_FLAG_TXE)==0);
	USART1->DR = ch;
	return ch;
}
#endif 

/**/

int stderr_putchar(int ch){
	while((USART1->SR & USART_FLAG_TXE)==0);
	USART1->DR = ch;
	return ch;
}

int stdin_getchar(void){
	while((USART1->SR & USART_FLAG_TXE)==0);
	int ch = USART1->DR;
	return ch;
}

int stdout_putchar(int ch){
	while((USART1->SR & USART_FLAG_TXE)==0);
	USART1->DR = ch;
	return ch;
}

void ttywrch(int ch){
	while((USART1->SR & USART_FLAG_TXE)==0);
	USART1->DR = ch;
}
