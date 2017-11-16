#include "portmacro.h"
#include "queue_mgr.h"
#include "app_msg.h"
#include "app_timer.h"

uint32_t app_msg_callback( void * msg_sll, void * msg_node ) {
    if ( NULL == msg_sll || NULL == msg_node ) {
        return FALSE;
    }
    
    MOUNT( msg_sll, msg_node );
    return TRUE;
}

p_msg_node_t app_msg_create( uint32_t msg_type, p_msg_data_t p_msg_data, uint32_t length ) {
    p_msg_node_t msg = ( p_msg_node_t )xmalloc( MSG_SIZE );
    if ( NULL != msg ) {
        msg->length = length;
        msg->type = msg_type;
        msg->data = ( p_msg_data_t )xmalloc( length );   // 
        if ( NULL != msg->data ) {
            memcpy( msg->data, p_msg_data, length );
        } 
    }
    return msg;
}

/*
** Delete a message node
*/
uint32_t app_msg_delete( p_msg_sll_t *msg_sll, p_msg_node_t msg_node ) {   //
    if ( NULL == msg_sll || NULL == msg_node ) {
        return FALSE;
    }
    
    // Remove frome message link list.
    UNMOUNT( *msg_sll, msg_node );
    
    // Free message memory
    if ( NULL != msg_node->data ) {
        xfree( &(msg_node->data) );
        xfree( msg_node );
        (*msg_sll)->msg_nbr_cnt--;
        return TRUE;
    }
    return FALSE;
}

uint32_t app_msg_flush( p_msg_sll_t msg_sll ) {   //
    p_msg_node_t current;
    
    if ( NULL == msg_sll ) {
        return FALSE;
    }
    
    while ( NULL != ( current = msg_sll->next ) ) {
        msg_sll->next = current->next;
        
        // Remove from message link list.
        UNMOUNT( msg_sll, current );
        
        // Free message memory.
        if ( NULL != current->data ) {
            xfree( current->data );
            xfree( current );
            msg_sll->msg_nbr_cnt--;
        } else {
            // Meet fault
            break;
        }
    }
    return msg_sll->msg_nbr_cnt;
}

/*
** Post a message to task.
** If post_delay_ticks = 0 (N) means post the meassage immdiately otherwise post 
** after N ticks by create a timer.
*/
uint32_t app_msg_post( p_msg_sll_t msg_sll, p_msg_node_t msg_node, uint32_t post_delay_ticks ) {
    if ( NULL == msg_node ) {
        return FALSE;
    }
    
    if ( 0 == post_delay_ticks ) {
        // Post message immdiately.
        MOUNT( msg_sll, msg_node );
    } else {
        // Post message after delay ticks.
        if ( NULL == app_timer_create( post_delay_ticks,            \
                                       0,                           \
                                       APP_TIMER_MODE_SINGLE_SHOT,  \
                                       app_msg_callback,            \
                                       msg_node ) ) {
            return FALSE;
        }
    }
    return TRUE;
}

/*
** Waiting message.
** If there is no new message return NULL otherwise return a pointer of message.
*/
void * app_msg_pend( p_msg_handler_t msg_sll ) {
    p_msg_sll_t p = ( p_msg_sll_t )msg_sll;
    
    if ( NULL == p ) {
        return NULL;
    } else {
        p->msg_nbr_cnt--;
        return p->next;
    }
}

// End APP_MSG_H
