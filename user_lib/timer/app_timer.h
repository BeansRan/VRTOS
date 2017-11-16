#ifndef APP_TIMER_INCLUDE 
#define APP_TIMER_INCLUDE 

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//#include "app_msg.h"

#define TIMER_MAX_TICK_COUNTER          0x00FFFFFFUL    /**< Timer tick source counter max value */
#define SYSTEM_IDLE_TICKS_BEFORE_SLEEP  5U              /**< The minimum interval during which the system performs sleep */
#define TIMER_TIME_SLICE                1U              /**< Timer time slice */

/**@brief Application time-out handler type. */
/**@brief Type of function for passing events from the timer module to the scheduler. */
typedef uint32_t ( *p_timer_timeout_handler_t )( void * timeout_handler, void * p_context );

/**@brief Timer handler typedef */
typedef void *                          p_timer_handler_t;

/**@brief Timer modes. */
typedef enum
{
    APP_TIMER_MODE_SINGLE_SHOT,                         /**< The timer will expire only once. */
    APP_TIMER_MODE_REPEATED                             /**< The timer will restart each time it expires. */
} app_timer_mode_t;

void app_timer_init(void);
void app_timer_start( void );
void app_timer_stop( void );

p_timer_handler_t app_timer_create( uint32_t                     ticks_periodic_interval, 
                                      uint32_t                     ticks_first_delay,
                                      app_timer_mode_t             mode,
                                      p_timer_timeout_handler_t  timeout_handler,
                                      void *                       p_msg_sll);

void app_timer_timeouts_check( void );

//#undef APP_TIMER_INCLUDE
#endif // APP_TIMER_H__

/** @} */

