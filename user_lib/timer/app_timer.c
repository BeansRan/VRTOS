#include "portmacro.h"
#include "app_timer.h"
#include "app_task.h"
#include "app_msg.h"
#include "queue_mgr.h"

typedef uint32_t (*p_tick_source_t) (void);

/**@brief Timer node type. The nodes will be used form a linked list of running timers. */
typedef struct TIMER_NODE {
    struct TIMER_NODE *             next;                       /**< Id of next timer in list of running timers. */
    app_timer_mode_t                mode;                       /**< Timer mode. */
    uint32_t                        ticks_to_expire;            /**< Number of ticks from previous timer interrupt to timer expiry. */
    uint32_t                        ticks_at_start;             /**< Current tick source counter value when the timer was started. */
    uint32_t                        ticks_first_delay;          /**< Number of ticks in the first timer interval. */
    uint32_t                        ticks_periodic_interval;    /**< Timer period (for repeating timers). */
    p_timer_timeout_handler_t       p_timeout_handler;          /**< Pointer to function to be executed when the timer expires. */
    p_task_handler_t                p_task_handler;             /**<  */
    p_msg_sll_t                     p_msg_sll;                  /**< General purpose pointer. Will be passed to the timeout handler when the timer expires. */
} app_timer_t;

typedef app_timer_t *               p_app_timer_t;              /**< Pointer to app_timer_t */

/*
** Timer structure link list header.
*/
typedef struct TIMER_SLL {
    uint32_t                        timer_nbr_max;              /**< Valid timer number */
    uint32_t                        dynamic_sleep_ticks;        /**< System into sleep mode ticks */
    uint32_t                        time_slice;                 /**< Timer counter uint */
    p_tick_source_t                 tick_source;                /**< Timer counter source */
    bool                            is_runing;                  /**< Timer handler run state */
    app_timer_t *                   next;                       /**< Pointer to timer node */
}timer_link_list_t;

typedef timer_link_list_t *         p_timer_link_list_t;

/**< User file paramters declaration -----------------------------------------*/
static p_timer_link_list_t          p_timer_link_list;

/**< Private function definition -------------------------------------------- */

/*
** Notice: Called this function before the task is created.
*/
void app_timer_init(void) {
    p_timer_link_list = ( timer_link_list_t * )xmalloc( sizeof( timer_link_list_t ) );
    
    if ( NULL != p_timer_link_list ) {
        p_timer_link_list->next           = NULL;
        p_timer_link_list->tick_source    = HAL_GetTick;
        p_timer_link_list->time_slice     = TIMER_TIME_SLICE;
        p_timer_link_list->is_runing      = false;
    }
}

void app_timer_start( void ) {
    p_timer_link_list->is_runing = true;
}

void app_timer_stop( void ) {
    p_timer_link_list->is_runing = false;
}

/*
** Notice
** ticks_periodic_interval and ticks_first_delay must be a multiple of TIMER_TIME_SLICE, 
** otherwise timer would not be created and return NULL.
*/
p_timer_handler_t app_timer_create( uint32_t                       ticks_periodic_interval, 
                                      uint32_t                     ticks_first_delay,
                                      app_timer_mode_t             mode,
                                      p_timer_timeout_handler_t    timeout_handler,
                                      void *                       p_msg_sll) {
    p_timer_link_list_t pheader = p_timer_link_list;
    if ( ( 0 == ticks_periodic_interval ) || \
         ( 0 != ( ticks_periodic_interval % TIMER_TIME_SLICE ) ) || \
         ( 0 != ( ticks_first_delay % TIMER_TIME_SLICE ) ) ) {
        return NULL;
    }
    
    p_app_timer_t htimer = ( p_app_timer_t )xmalloc( sizeof( app_timer_t ) );
    if ( NULL != htimer ) {
        htimer->next                    = NULL;
        htimer->mode                    = mode;
        htimer->p_msg_sll               = p_msg_sll;
        htimer->p_timeout_handler       = timeout_handler;
        htimer->ticks_at_start          = p_timer_link_list->tick_source();
        htimer->ticks_to_expire         = 0;
        htimer->ticks_first_delay       = ticks_first_delay;
        htimer->ticks_periodic_interval = ticks_periodic_interval;
    }
    pheader->timer_nbr_max++;
    MOUNT( pheader->next, htimer );
    
    return ( p_timer_handler_t )htimer;
}

/*
** Delete a timer node.
*/
uint32_t app_timer_delete( p_app_timer_t timer_node ) {
    p_timer_link_list_t pheader = p_timer_link_list;
    p_app_timer_t current;
    
    while ( ( NULL != ( current = pheader->next ) ) && (current == timer_node ) ) {
        pheader->next = current->next;
    }
    
    if ( NULL != current ) {
        UNMOUNT( pheader->next, timer_node );
        xfree( timer_node );
        pheader->timer_nbr_max--;
        
        return TRUE;
    }
    return FALSE;
}

/*
** Set timer paramters of task.
*/
p_timer_handler_t app_task_timer_set( uint32_t                      ticks_periodic_interval,
                                      uint32_t                      ticks_first_delay,
                                      app_timer_mode_t              mode,
                                      p_timer_timeout_handler_t     timeout_handler,
                                      p_task_handler_t              app_task,
                                      p_msg_handler_t               msg_handler ) {
    return NULL;
}

/*
** Function for computing the difference between two timer counter values.
** return   Number of ticks elapsed from ticks_old to ticks_now.
*/
uint32_t app_timer_get_diff_ticks( uint32_t lastest_ticks, uint32_t old_ticks ) {
    uint32_t ticks_diff = 0;
    
    ticks_diff = ( lastest_ticks - old_ticks ) & TIMER_MAX_TICK_COUNTER;
    
    return ticks_diff;
}

void app_timer_timeouts_check( void ) {
    p_app_timer_t current;
    timer_link_list_t *pheader = p_timer_link_list;
    
    while ( ( NULL != ( current = pheader->next ) ) ) {
        
        // Timer first delay check.
        if ( 0 == current->ticks_first_delay ) {
            current->ticks_at_start = pheader->tick_source();
            current->ticks_to_expire += pheader->time_slice;
            
            // If app timer ticks expired and excute timeout handler.
            if ( current->ticks_to_expire >= current->ticks_periodic_interval ) {
                current->ticks_to_expire = 0;
                if ( NULL != current->p_timeout_handler ) {
                    current->p_timeout_handler( current->p_task_handler, current->p_msg_sll );
                }
            }
        } else {
            current->ticks_first_delay -= pheader->time_slice;
        }
        
        // Calculate the ticks to run the next task.
        // Could perform low-power sleep if the time to meet the requirements.
        if ( 0 == pheader->dynamic_sleep_ticks ) {
            pheader->dynamic_sleep_ticks = app_timer_get_diff_ticks( current->next->ticks_periodic_interval,
                                                                   current->next->ticks_to_expire);
        }
        pheader->next = current->next;
        
        // If app timer is not repeat mode than delete it.
        if ( APP_TIMER_MODE_SINGLE_SHOT == current->mode ) {    //
            app_timer_delete( current );
        }
    }
    
    #if LOW_POWER_ENABLE
    // System ready to into sleep.
    if ( pheader->dynamic_sleep_ticks >= SYSTEM_IDLE_TICKS_BEFORE_SLEEP ) {
        // add your code ...
    }
    #endif
}






