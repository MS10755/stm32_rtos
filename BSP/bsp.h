#ifndef __BSP_H__
#define __BSP_H__

enum{
	LED0,
	LED1,
};
#define LED0_PORT GPIOB
#define LED0_PIN	GPIO_Pin_5

#define LED1_PORT GPIOE
#define LED1_PIN	GPIO_Pin_5

void led_init(void);
void led_set(int ledNumber,int value);
void led_toggle(int ledNumber);
#endif

