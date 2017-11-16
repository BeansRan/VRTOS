#include "main.h"

#include "vrtos.h"
#include "app_led.h"

task_cb_t *led_task_handler;
msg_node_t *led_msg_handler;

void app_led_op_callback(  ) {
    
}

uint32_t app_led_task( p_task_handler_t task_handler, p_msg_handler_t msg_handler ) {
    msg_node_t *pmsg = ( msg_node_t * )msg_handler;
    p_msg_handler_t new_msg = NULL;
    uint32_t op_type = 0;
    task_cb_t *p_task = ( task_cb_t * )task_handler;
    
    if ( TASK_STATE_RUN == p_task->state ) {
        p_task->state = TASK_STATE_PEND;
        if ( NULL != pmsg ) {
            op_type = pmsg->type;
            app_task_msg_delete( ( p_msg_handler_t * )&(p_task->p_msg_handler), pmsg );
            
            switch( op_type ) {
                case LED_OP_TYPE_TURN_ON:
                    HAL_GPIO_WritePin( LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET );
                    new_msg = app_msg_create( LED_OP_TYPE_TURN_ON, NULL, 0 );
                    app_msg_post_timer( 1000, 0, APP_TIMER_MODE_SINGLE_SHOT, led_task_handler, new_msg );
                    LOG("led turn on.\r\n");
                    break;
                case LED_OP_TYPE_TURN_OFF:
                    HAL_GPIO_WritePin( LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET );
                    new_msg = app_msg_create( LED_OP_TYPE_TURN_OFF, NULL, 0 );
                    app_msg_post_timer( 3000, 0, APP_TIMER_MODE_SINGLE_SHOT, led_task_handler, new_msg );
                    LOG("led turn off.\r\n");
                    break;
                default:
                    break;
            }
        }
    }
    return TRUE;
}

void led_init( void ) {
    led_task_handler = task_create( "led_task", app_led_task, 1, led_msg_handler );
}


