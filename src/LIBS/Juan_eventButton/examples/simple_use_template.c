#include "Juan_eventButton.h"

JEB_obj_t key1,key2,key3,key4;
JEB_comb_t comb1;

static void key_callback(void * obj,JUAN_EVENTBUTTON_event_t event){
    JEB_obj_t*key = obj;
    printf("key:%p  event:%s\r\n",key->this,JUAN_EVENTBUTTON_event_str(event));
}

static void comb_callback(void * obj,JUAN_EVENTBUTTON_event_t event){
    JEB_comb_t * comb = obj;
    printf("key:%p  event:%s\r\n",comb->this,JUAN_EVENTBUTTON_event_str(event));
}

/* get SysTime in Milliseconds */
static uint32_t get_ms(){
    //  For example: HAL_GetTick() , xTaskGetTickCount() , etc...

}

/* get gpio level */
static uint8_t gpio_read_input(uint8_t io){
    uint8_t io_level;

    return io_level;
}

int main(){\
    /* Initialize key1 ~ key4 */
    JUAN_EVENTBUTTON_init(&key1,JEB(LEVEL_HIGH),3000,500,key_callback);
    JUAN_EVENTBUTTON_init(&key2,JEB(LEVEL_HIGH),3000,500,key_callback);
    JUAN_EVENTBUTTON_init(&key3,JEB(LEVEL_HIGH),3000,500,key_callback);
    JUAN_EVENTBUTTON_init(&key4,JEB(LEVEL_HIGH),3000,500,key_callback);

    /* Initialize comb1 */
    JEB_comb_init(&comb1,comb_callback,&key1,&key2,&key3);

    for(;;){
        /* read key1 ~ key4 gpio level and notify event_button */
        JUAN_EVENTBUTTON_notify(&key1,gpio_read_input(1));
        JUAN_EVENTBUTTON_notify(&key2,gpio_read_input(2));
        JUAN_EVENTBUTTON_notify(&key3,gpio_read_input(3));
        JUAN_EVENTBUTTON_notify(&key4,gpio_read_input(4));

        /* key1 ~ key4 event polling */
        JUAN_EVENTBUTTON_poll(&key1,get_ms());
        JUAN_EVENTBUTTON_poll(&key2,get_ms());
        JUAN_EVENTBUTTON_poll(&key3,get_ms());
        JUAN_EVENTBUTTON_poll(&key4,get_ms());

        /* comb1 event polling */
        JUAN_EVENTBUTTON_comb_poll(&comb1);
    }
}