#ifndef APP_LED_INCLUDE
#define APP_LED_INCLUDE

#include <stdio.h>
#include <stdint.h>

#include "vrtos.h"

#define LED_OP_TYPE_TURN_ON            0X01
#define LED_OP_TYPE_TURN_OFF           0X02

extern task_cb_t * led_task_handler;
//extern msg_node_t *  led_msg_handler;

void        led_init( void );
uint32_t    app_led_task( p_task_handler_t task_handler, p_msg_handler_t msg_handler );

#undef  APP_LED_INCLUDE
#endif
