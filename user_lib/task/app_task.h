#ifndef APP_TASK_INCLUDE 
#define APP_TASK_INCLUDE 

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "app_timer.h"
#include "app_msg.h"

typedef void *                                  p_task_handler_t;
typedef uint32_t ( *task_func_callback_t )      ( p_task_handler_t task_handler, p_msg_node_t msg );

/*
** Task control block
*/
typedef struct TASK_NODE {
    struct TASK_NODE *              next;
    uint32_t                        priority;
    task_func_callback_t            callback;
    p_msg_sll_t                     p_msg_handler;
    p_timer_handler_t               task_timer_handler;
    char                            func_name[32];
}task_cb_t;

typedef task_cb_t *                 p_task_cb_t;

void task_init( void );

void * task_create ( const char *           func_name, 
                     task_func_callback_t   task_handler, 
                     uint32_t               priority, 
                     p_msg_sll_t            msg_sll );

void task_traverse ( void );

//#undef APP_TASK_INCLUDE
#endif // APP_TASK_H__

/** @} */

