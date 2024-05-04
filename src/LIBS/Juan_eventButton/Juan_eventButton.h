/**
 * @file Juan_eventButton.h :
 * @author Juan (1075502955@qq.com)
 * @brief  :
 * @version 0.1
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __JUAN_EVENTBUTTON_H__
#define __JUAN_EVENTBUTTON_H__
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>

// #define JEB_DEBUG
// #define JEB_NO_PARM_CP

#define JEB_COMB_MAX 3

#ifdef JEB_printf
#else
#define JEB_printf(...) printf(__VA_ARGS__)
#endif

#ifdef JEB_DEBUG
#define JEB_debug(...)  JEB_printf("%s/%s/%d",__FILE__,__FUNCTION__,__LINE__),JEB_printf(__VA_ARGS__),JEB_printf("\r\n")
#else 
#define JEB_debug(...)
#endif


#ifdef assert
#define JEB_assert(x) assert(x)
#else
#define JEB_assert(x) 
#endif


#define ENUM_TO_STR(x) #x

#define JEB(x) JUAN_EVENTBUTTON_##x

typedef uint32_t JEB(ms_t);
#define JEB_MS_MAX UINT32_MAX

// #define USE_INDEPENDENT_SETTINGS

#ifndef USE_INDEPENDENT_SETTINGS 
 #define JEB_SHAKE_MS_MAX 30
#endif

#define MAX_STRUCT_SIZE(s1, s2) ((sizeof(s1) > sizeof(s2)) ? sizeof(s1) : sizeof(s2))

typedef enum{
#ifdef USE_JEB_EVENT_POLL
    JEB(EVENT_POLL),
#endif
    JEB(EVENT_PRESSED),
    JEB(EVENT_PRESSING),
    JEB(EVENT_RELEASED),
    JEB(EVENT_SHORT_CLICKED),
    JEB(EVENT_LONG_PRESSING),
    JEB(EVENT_LONG_CLICKED),
    JEB(EVENT_DOUBLE_CLICKED),
    JEB(EVENT_TRIPLE_CLICKED),
    JEB(EVENT_COMB_MATCH),
    JEB(EVENT_MAX)
}JEB(event_t);

typedef enum{
    JEB(STATUS_PRESSED_BEGIN),
    JEB(STATUS_PRESSED),
    JEB(STATUS_PRESSING),
    JEB(STATUS_RELEASED_BEGIN),
    JEB(STATUS_RELEASED),
}JEB(status_t);

typedef enum{
    JEB(LEVEL_LOW),
    JEB(LEVEL_HIGH),
}JEB(level_t);

typedef void (*JEB(callback_t))(void * obj,JEB(event_t) event);
typedef struct
{
    void * obj;
#ifdef USE_INDEPENDENT_SETTINGS
    JEB(ms_t) shake_ms;
#endif
    JEB(ms_t) _short_long_separate_ms;
    JEB(ms_t) _storage[3];
    JEB(ms_t) _last_released_ms;
    JEB(ms_t) _double_clicked_interval;
    uint8_t _clicked_count;
    JEB(status_t) _last;
    JEB(event_t) _last_event;
    JEB(level_t) _pressed_level;
    JEB(level_t) _level;
    JEB(callback_t) _cb;
}JEB(obj_t);

typedef struct 
{
    void * obj;
    JEB(obj_t) *obj_list[JEB_COMB_MAX];
    uint8_t obj_num;
    uint8_t check_index;
    JEB(callback_t) _cb;
    bool _match;
}JEB(comb_t);

typedef JEB(comb_t) JEB_comb_t;
typedef JEB(obj_t) JEB_obj_t;
typedef JEB(ms_t) JEB_ms_t;
typedef JEB(status_t) JEB_status_t;
typedef JEB(level_t) JEB_level_t;


typedef JEB(callback_t) JEB_callback_t;

#ifdef __cplusplus
extern "C" {
#endif
void JEB(init)(JEB_obj_t * obj,JEB_level_t pressed_level,JEB_ms_t short_long_separate_ms,JEB_ms_t double_clicked_interval,JEB_callback_t cb);
void JEB(poll)(JEB_obj_t * obj,JEB_ms_t now);
void JEB(notify)(JEB_obj_t * obj,JEB_level_t level);
const char * const JEB(event_str)(JEB(event_t) event);

void JEB(comb_init_raw)(JEB_comb_t *comb,JEB_callback_t cb,...);
void JEB(comb_poll)(JEB_comb_t *comb);

#ifdef __cplusplus
}
#endif

#define JEB_comb_init(comb,cb,...) JEB(comb_init_raw)(comb,cb,__VA_ARGS__,NULL)
#endif // !__JUAN_EVENTBUTTON_H__

