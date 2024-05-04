#include "stm32f1xx_hal.h"
#include "Juan_eventButton.h"

#define KEY2_Pin GPIO_PIN_2
#define KEY2_Port GPIOE

#define KEY1_Pin GPIO_PIN_3
#define KEY1_Port GPIOE

#define KEY0_Pin GPIO_PIN_4
#define KEY0_Port GPIOE

#define KEY_WKUP_Pin GPIO_PIN_0
#define KEY_WKUP_Port   GPIOA

JEB_obj_t key1,key2,key3,key4;
JEB_comb_t comb1;

static void key_callback(void * obj,JUAN_EVENTBUTTON_event_t event){
    JEB_obj_t*key = (JEB_obj_t*)obj;
    printf("key:%p  event:%s\r\n", key->obj,JUAN_EVENTBUTTON_event_str(event));
}
static void comb_callback(void * obj,JUAN_EVENTBUTTON_event_t event){
    JEB_comb_t * comb = (JEB_comb_t *)obj;
    printf("key:%p  event:%s\r\n",comb->obj,JUAN_EVENTBUTTON_event_str(event));
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
	MX_USART1_UART_Init();

    JUAN_EVENTBUTTON_init(&key1,JEB(LEVEL_LOW),3000,500,key_callback);
    JUAN_EVENTBUTTON_init(&key2,JEB(LEVEL_LOW),3000,500,key_callback);
    JUAN_EVENTBUTTON_init(&key3,JEB(LEVEL_LOW),3000,500,key_callback);
    JUAN_EVENTBUTTON_init(&key4,JEB(LEVEL_HIGH),3000,500,key_callback);
	JEB_comb_init(&comb1,comb_callback,&key1,&key2,&key3);
    while (1)
    {
	JUAN_EVENTBUTTON_notify(&key1,(JEB_level_t)HAL_GPIO_ReadPin(KEY0_Port,KEY0_Pin));
	JUAN_EVENTBUTTON_notify(&key2,(JEB_level_t)HAL_GPIO_ReadPin(KEY1_Port,KEY1_Pin));
	JUAN_EVENTBUTTON_notify(&key3,(JEB_level_t)HAL_GPIO_ReadPin(KEY2_Port,KEY2_Pin));
	JUAN_EVENTBUTTON_notify(&key4,(JEB_level_t)HAL_GPIO_ReadPin(KEY_WKUP_Port,KEY_WKUP_Pin));
		
        JUAN_EVENTBUTTON_poll(&key1,HAL_GetTick());
        JUAN_EVENTBUTTON_poll(&key2,HAL_GetTick());
        JUAN_EVENTBUTTON_poll(&key3,HAL_GetTick());
        JUAN_EVENTBUTTON_poll(&key4,HAL_GetTick());
		JUAN_EVENTBUTTON_comb_poll(&comb1);
    HAL_Delay(1);
    }
    
}