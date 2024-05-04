/**
 * @file Juan_eventButton.c
 * @author Juan (1075502955@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Juan_eventButton.h"

static const char * event_strings[JEB(EVENT_MAX)]={
#ifdef USE_JEB_EVENT_POLL
    ENUM_TO_STR(JUAN_EVENTBUTTON_EVENT_POLL),
#endif
    ENUM_TO_STR(JUAN_EVENTBUTTON_EVENT_PRESSED),
    ENUM_TO_STR(JUAN_EVENTBUTTON_EVENT_PRESSING),
    ENUM_TO_STR(JUAN_EVENTBUTTON_EVENT_RELEASED),
    ENUM_TO_STR(JUAN_EVENTBUTTON_EVENT_SHORT_CLICKED),
    ENUM_TO_STR(JUAN_EVENTBUTTON_EVENT_LONG_PRESSING),
    ENUM_TO_STR(JUAN_EVENTBUTTON_EVENT_LONG_CLICKED),
    ENUM_TO_STR(JUAN_EVENTBUTTON_EVENT_DOUBLE_CLICKED),
    ENUM_TO_STR(JUAN_EVENTBUTTON_EVENT_TRIPLE_CLICKED),
    ENUM_TO_STR(JUAN_EVENTBUTTON_EVENT_COMB_MATCH)
};

const char * const JEB(event_str)(JEB(event_t) event){
    JEB_assert(event < JEB(EVENT_MAX));
    return event_strings[event];
}

#define PARM_BUF_SIZE MAX_STRUCT_SIZE(JEB_obj_t,JEB_comb_t)

static void jeb_comm_cb(void * parm,uint8_t parm_size,JEB_callback_t cb,JEB(event_t) event){
    JEB_assert(parm);
    JEB_assert(parm_size <= PARM_BUF_SIZE);
    JEB_assert(parm_size);
    JEB_assert(cb);
#ifdef JEB_NO_PARM_CP
    if(cb) cb(parm, event);
#else
    uint8_t* parm_buf = (uint8_t*)malloc(PARM_BUF_SIZE);
    if(parm_buf == NULL){
        JEB_printf("[warning] malloc parm_buf failed!!!\r\n");
        if(cb) cb(parm, event);
    }else{
        memmove(parm_buf,parm,parm_size);
        if(cb) cb(parm_buf, event);
    }
    free(parm_buf);
    parm_buf = NULL;
#endif
}
/**
 * @brief  : initialization an event button obj
 * 
 * @param obj : event button obj
 * @param pressed_level : button level after pressing
 * @param short_long_separate_ms : Long press and short press to distinguish time
 * @param double_clicked_interval : Double/trible click interval time
 * @param cb : button event callback function
 */

void JEB(init)(JEB_obj_t * obj,JEB_level_t pressed_level,JEB_ms_t short_long_separate_ms,JEB_ms_t double_clicked_interval,JEB_callback_t cb){
    JEB_assert(obj);
    JEB_assert(short_long_separate_ms > 0);
    obj->obj = obj;
    obj->_last = JEB(STATUS_RELEASED);
    obj->_pressed_level = pressed_level;
    obj->_cb = cb;
    obj->_double_clicked_interval = double_clicked_interval;
    obj->_short_long_separate_ms = short_long_separate_ms;
    obj->_last_event = JEB(EVENT_SHORT_CLICKED);
}

static bool check_pressed(JEB_obj_t * obj){
    return !((JEB(LEVEL_HIGH) == obj->_pressed_level) ^ obj->_level);
}


static JEB_ms_t ms_passed(JEB_ms_t before_ms,JEB_ms_t now){
    return now >= before_ms ?  now - before_ms: now + JEB_MS_MAX - before_ms;
}

/**
 * @brief button event polling
 * 
 * @param obj : event button obj
 * @param now : current time in microsecond
 */

void JEB(poll)(JEB_obj_t * obj,JEB_ms_t now){
    JEB_assert(obj);
#ifdef USE_JEB_EVENT_POLL
    jeb_comm_cb(obj,sizeof(JEB_obj_t),obj->_cb,JEB(EVENT_POLL));
    
#endif
    bool is_pressed = check_pressed(obj);
    // 1.1.pressed_begin
    if(obj->_last == JEB(STATUS_RELEASED) && is_pressed){
        obj->_storage[0] = now;
        obj->_last = JEB(STATUS_PRESSED_BEGIN);
        JEB_debug("[%d]%s",now,ENUM_TO_STR(STATUS_PRESSED_BEGIN));
    }
    // 1.2.pressed shake check
    if(obj->_last == JEB(STATUS_PRESSED_BEGIN) && is_pressed && ms_passed(obj->_storage[0],now) >=
        #ifndef USE_INDEPENDENT_SETTINGS
            JEB_SHAKE_MS_MAX
        #else
            obj->shake_ms
        #endif // USE_INDEPENDENT_SETTINGS 
    ){
        obj->_storage[0] = now;
        obj->_storage[2] = now;
        obj->_last = JEB(STATUS_PRESSED);
        JEB_debug("[%d]%s",now,ENUM_TO_STR(STATUS_PRESSED));
        jeb_comm_cb(obj,sizeof(JEB_obj_t),obj->_cb,JEB(EVENT_PRESSED));
    }
    // 1.3.pressed shake reset
    if(obj->_last == JEB(STATUS_PRESSED_BEGIN) && is_pressed == false && ms_passed(obj->_storage[0],now) >=
        #ifndef USE_INDEPENDENT_SETTINGS
            2*JEB_SHAKE_MS_MAX
        #else
            2*obj->shake_ms
        #endif // USE_INDEPENDENT_SETTINGS 
    ){
        obj->_last = JEB(STATUS_RELEASED);
        JEB_debug("[warning][%d] auto reset %s",now,ENUM_TO_STR(STATUS_RELEASED));
    }

    // 2.pressing
    if(obj->_last == JEB(STATUS_PRESSED) && is_pressed){
        JEB_debug("[%d]%s",now,ENUM_TO_STR(STATUS_PRESSING));
        jeb_comm_cb(obj,sizeof(JEB_obj_t),obj->_cb,JEB(EVENT_PRESSING));
        if( ms_passed(obj->_storage[2],now) > obj->_short_long_separate_ms){
            jeb_comm_cb(obj,sizeof(JEB_obj_t),obj->_cb,JEB(EVENT_LONG_PRESSING));
            obj->_storage[2] = now;
        }
    }

    // 3.released
    if(obj->_last == JEB(STATUS_PRESSED) && is_pressed == false){
        obj->_storage[1] = now;
        obj->_last = JEB(STATUS_RELEASED_BEGIN);
        JEB_debug("[%d]%s",now,ENUM_TO_STR(STATUS_RELEASED_BEGIN));
    }
    
    if(obj->_last == JEB(STATUS_RELEASED_BEGIN) && is_pressed == false && ms_passed(obj->_storage[1],now) >= 
    #ifndef USE_INDEPENDENT_SETTINGS
        JEB_SHAKE_MS_MAX
    #else
        obj->shake_ms
    #endif // USE_INDEPENDENT_SETTINGS 
    ){
        obj->_last = JEB(STATUS_RELEASED);
        jeb_comm_cb(obj,sizeof(JEB_obj_t),obj->_cb,JEB(EVENT_RELEASED));
        JEB_debug("[%d]%s",now,ENUM_TO_STR(STATUS_RELEASED));
        if(ms_passed( obj->_storage[0],now) < obj->_short_long_separate_ms){
        /* short pressed */
            if(ms_passed( obj->_last_released_ms,now) < obj->_double_clicked_interval){
                if(obj->_last_event == JEB(EVENT_SHORT_CLICKED)){
                    jeb_comm_cb(obj,sizeof(JEB_obj_t),obj->_cb,JEB(EVENT_DOUBLE_CLICKED));
                    obj->_last_event = JEB(EVENT_DOUBLE_CLICKED);
                }else if(obj->_last_event == JEB(EVENT_DOUBLE_CLICKED)){
                    jeb_comm_cb(obj,sizeof(JEB_obj_t),obj->_cb,JEB(EVENT_TRIPLE_CLICKED));
                    obj->_last_event = JEB(EVENT_TRIPLE_CLICKED);
                }else{
                    jeb_comm_cb(obj,sizeof(JEB_obj_t),obj->_cb,JEB(EVENT_SHORT_CLICKED));
                    obj->_last_event = JEB(EVENT_SHORT_CLICKED);
                }
            }else{
                //sinale clocked
                jeb_comm_cb(obj,sizeof(JEB_obj_t),obj->_cb,JEB(EVENT_SHORT_CLICKED));
                obj->_last_event = JEB(EVENT_SHORT_CLICKED);
            }
        }else{
            /* long pressed */
            obj->_last_event = JEB(EVENT_LONG_CLICKED);
            jeb_comm_cb(obj,sizeof(JEB_obj_t),obj->_cb,JEB(EVENT_LONG_CLICKED));
        }
        obj->_last_released_ms = now;
    }

    if(obj->_last == JEB(STATUS_RELEASED_BEGIN) && is_pressed && ms_passed(obj->_storage[1],now) >= 
    #ifndef USE_INDEPENDENT_SETTINGS
        2*JEB_SHAKE_MS_MAX
    #else
        2*obj->shake_ms
    #endif // USE_INDEPENDENT_SETTINGS 
    ){
        obj->_last = JEB(STATUS_PRESSED);
        JEB_debug("[warning][%d] auto reset %s",now,ENUM_TO_STR(STATUS_PRESSED));
    }
}

/**
 * @brief  : Notification the event polling button level
 * 
 * @param obj : button obj
 * @param level : current button level
 */

void JEB(notify)(JEB_obj_t * obj,JEB_level_t level){
    JEB_assert(obj);
    obj->_level = level;
}

/**
 * @brief  : Initialize combination buttons
 * 
 * @param comb : combination data pointer
 * @param cb : combination callback function
 * @param vargs :  button obj list
 * @example :   JEB_comb_init(&comb1,key_cb,&key_1,&key_2);
 *      
 */

void JEB(comb_init_raw)(JEB_comb_t *comb,JEB_callback_t cb,...){
    JEB_assert(comb);
    JEB_assert(cb);
    va_list args;
    comb->obj = comb;
    comb->obj_num = 0;
    comb->_cb = cb;
    comb->_match = true;
    va_start(args,cb);
    JEB_obj_t * obj = NULL;
    while( (obj = va_arg(args,JEB_obj_t *)) != NULL){
        if(comb->obj_num < JEB_COMB_MAX){
            comb->obj_list[comb->obj_num++] = obj;
        }else{
            JEB_printf("[warning] The combination key num > JEB_COMB_MAX , obj{%p} will be ingored!!!\r\n",(void*)obj);
            break;
        }
    }
    va_end(args);
}

/**
 * @brief combination event polling
 * 
 * @param comb : combination data pointer
 */


void JEB(comb_poll)(JEB_comb_t *comb){
    JEB_assert(comb);
    JEB_assert(comb->obj_num);
    JEB_obj_t * obj = NULL;
    int8_t j;
    JEB_ms_t before_btn_pressed_ms;
    for(uint8_t i=0;i<comb->obj_num;i++){
        j = i-1;
        if(j >= 0) before_btn_pressed_ms = comb->obj_list[j]->_storage[0];
        obj = comb->obj_list[i];
        if(obj->_last != JEB(STATUS_PRESSED)){
            comb->_match = true;
            return;
        }
        // sequence match
        if(j >=0 && obj->_storage[0] <= before_btn_pressed_ms){
            comb->_match = true;
            return;
        }
    }
    if(comb->_cb && comb->_match) {
        comb->_match = false;
        jeb_comm_cb(comb,sizeof(JEB_comb_t),comb->_cb,JEB(EVENT_COMB_MATCH));
    };
}
