#ifndef VTOS_INCLUDE
#define VTOS_INCLUDE 

#include "portmacro.h"

#define LOW_POWER_ENABLE                    (0U)

/**@brief User typedef declaration ------------------------------------------ */
typedef void *                              p_msg_handler_t;            /**< Message handler typedef */
typedef char *                              p_msg_data_t;               /**< Data typedef of message handler */

/*
** Message control block
*/
typedef struct MSG_NODE {
    struct MSG_NODE *                       next;                       /**<  */
    uint32_t                                type;                       /**<  */
    uint32_t                                length;                     /**<  */
    p_msg_data_t                            data;                       /**<  */
}msg_node_t;

typedef void *                             p_task_handler_t;
typedef uint32_t ( *task_func_callback_t )( p_task_handler_t task_handler, p_msg_handler_t msg_handler );

/*
** Task control block
*/
typedef enum TASK_STATE {
    TASK_STATE_PEND = 0xe1,
    TASK_STATE_RUN,
}task_state_t;

typedef struct TASK_NODE {
    struct TASK_NODE *                      next;                       /**<  */
    uint32_t                                state;                      /**<  */
    uint32_t                                priority;                   /**<  */
    task_func_callback_t                    callback;                   /**<  */
    msg_node_t *                            p_msg_handler;              /**<  */    
    char *                                  func_name;                  /**<  */    
}task_cb_t;

/*
** Timer control block
*/
#define TIMER_MAX_TICK_COUNTER              0x00FFFFFFUL                /**< Timer tick source counter max value */
#define SYSTEM_IDLE_TICKS_BEFORE_SLEEP      5U                          /**< The minimum interval during which the system performs sleep */
#define TIMER_TIME_SLICE                    1U                          /**< Timer time slice */

typedef uint32_t (*p_tick_source_t) (void);

/**@brief Application time-out handler type. */
/**@brief Type of function for passing events from the timer module to the scheduler. */
typedef uint32_t ( *p_timer_timeout_handler_t )( void * timeout_handler, void * p_context );

/**@brief Timer handler typedef */
typedef void *                              p_timer_handler_t;          /**<  */

/**@brief Timer modes. */
typedef enum
{
    APP_TIMER_MODE_SINGLE_SHOT,                                         /**< The timer will expire only once. */
    APP_TIMER_MODE_REPEATED                                             /**< The timer will restart each time it expires. */
} app_timer_mode_t;

/**@brief Timer node type. The nodes will be used form a linked list of running timers. */
typedef struct TIMER_NODE {
    struct TIMER_NODE *                     next;                       /**< Id of next timer in list of running timers. */
    app_timer_mode_t                        mode;                       /**< Timer mode. */
    uint32_t                                ticks_to_expire;            /**< Number of ticks from previous timer interrupt to timer expiry. */
    uint32_t                                ticks_at_start;             /**< Current tick source counter value when the timer was started. */
    uint32_t                                ticks_first_delay;          /**< Number of ticks in the first timer interval. */
    uint32_t                                ticks_periodic_interval;    /**< Timer period (for repeating timers). */
    task_cb_t *                             p_task_handler;             /**<  */
    msg_node_t *                            p_msg_node;                 /**< General purpose pointer. Will be passed to the timeout handler when the timer expires. */
} app_timer_t;

typedef app_timer_t *                       p_app_timer_t;              /**< Pointer to app_timer_t */

/*
** Timer structure link list header.
*/
typedef struct TIMER_SLL {
    app_timer_t *                           next;                       /**< Pointer to timer node */
    int32_t                                 timer_nbr_max;              /**< Valid timer number */
    uint32_t                                dynamic_sleep_ticks;        /**< System into sleep mode ticks */
    uint32_t                                time_slice;                 /**< Timer counter uint */
    p_tick_source_t                         tick_source;                /**< Timer counter source */
    bool                                    is_runing;                  /**< Timer handler run state */
}app_timer_list_t;

/**@brief User function declaration ----------------------------------------- */
/*
** Message function
*/
#define MSG_SIZE                sizeof( msg_node_t )
p_msg_handler_t app_msg_create( uint32_t msg_type, p_msg_data_t p_msg_data, uint32_t length );
p_msg_handler_t app_msg_copy( msg_node_t * msg_node );
uint32_t        app_task_msg_delete( p_msg_handler_t *msg_sll, msg_node_t * msg_node );
uint32_t        app_msg_post( p_task_handler_t task_handler, p_msg_handler_t msg_node );

/*
** Post a message after specified delay ticks by set a timer.
** param    ticks_periodic_interval
**          If a message needs to be sent periodically, this value should be set.
** param    ticks_first_delay
**          The delay before first sending.
** param    mode
**          Timer mode. APP_TIMER_MODE_SINGLE_SHOT or APP_TIMER_MODE_REPEATED
** param    task_handler.
**          App task handler. Generated by function task_create(...)
** param    msg_node
**          Message sent to the specified task.
** retval   TRUE or FALSE.
*/
uint32_t        app_msg_post_timer( uint32_t            ticks_periodic_interval,
                                    uint32_t            post_delay_ticks,
                                    app_timer_mode_t    mode,
                                    p_task_handler_t    task_handler,
                                    p_msg_handler_t msg_node);

msg_node_t *    app_msg_pend( p_msg_handler_t msg_sll );

/*
** Timer function
*/
void app_timer_init(void);
void app_timer_start( void );
void app_timer_stop( void );

/*
** Notice
** ticks_periodic_interval and ticks_first_delay must be a multiple of TIMER_TIME_SLICE, 
** otherwise timer would not be created and return NULL.
**
** param    ticks_periodic_interval
**          If a message needs to be sent periodically, this value should be set.
** param    ticks_first_delay
**          The delay before first sending.
** param    mode
**          Timer mode. APP_TIMER_MODE_SINGLE_SHOT or APP_TIMER_MODE_REPEATED
** param    task_handler.
**          App task handler. Generated by function task_create(...)
** param    msg_node
**          Message sent to the specified task.
**
** retval   NULL or Pointer to timer handler.
*/
p_timer_handler_t app_timer_create( uint32_t            ticks_periodic_interval,   \
                                    uint32_t            ticks_first_delay,         \
                                    app_timer_mode_t    mode,                      \
                                    p_task_handler_t    p_task_handler,            \
                                    p_msg_handler_t     p_msg_handler);

/*
** Delete a timer node.
**
** param    timer_node.
**          Need to delete the timer node.
**
** retval   TRUE or FALSE.
*/
uint32_t app_timer_delete( app_timer_t *timer_node );

p_timer_handler_t app_timer_find( p_timer_handler_t timer );
/*
** Calculate the processing of all timer times as well as the timeout callback function.
**
** param    None.
**
** retval   None
*/
void app_timer_timeouts_check( void );
                                    
/*
** Task function
*/
void app_task_init( void );

/*
** Create a task control block handler.
** 
** param    func_name
**          Function name string.
** param    callback
**          Task callback function.
** param    priority
**          Task priority. 1 to 15.
** param    msg_handler
**          Pointer to task message handler.
** retval   Pointer to task control block handler or NULL.
*/
p_task_handler_t    task_create( char *                   func_name,      \
                                 task_func_callback_t     callback,       \
                                 uint32_t                 priority,       \
                                 p_msg_handler_t          msg_handler );

uint32_t            task_insert( task_cb_t **task_handler, task_cb_t *insert_node );
void                task_dispatch ( void );

#endif // VTOS_H END

