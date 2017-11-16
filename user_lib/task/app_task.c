#include "portmacro.h"
#include "app_task.h"

#include "app_msg.h"
#include "queue_mgr.h"

/**< User typedef structure definition -------------------------------------- */

/*
** Task control block handler.
*/
typedef struct TASK_LINK_LIST {
    uint32_t                        task_nbr_cnt;       /**< Task number counter */
    struct TASK_NODE *              next;               /**< Task link list */
}task_link_list_t;
typedef task_link_list_t *          p_task_link_list_t;       

/**< System task link list header ------------------------------------------- */
static p_task_link_list_t           p_task_link_list;           /**< Task control block link list handler  */

/**< User private function definition --------------------------------------- */
uint32_t task_insert( p_task_link_list_t linkp, task_cb_t *insert_node ) {
    register task_cb_t *current;
    register task_cb_t *new_node;
    
    if ( NULL == linkp ) {
        return FALSE;
    }
    
    /*
    ** Find the inserted node by node id until it less than the current node. 
    */
    while ( NULL != ( current = linkp->next ) && current->priority < insert_node->priority ) {
        linkp->next = current->next;
    }
    
    /*
    ** Allocate memory to new node and save the new value. Return FALSE if malloc
    ** error otherwise return TRUE.
    */
    new_node = ( task_cb_t * )xmalloc( sizeof( task_cb_t ) );
    if ( NULL == new_node ) {
        return FALSE;
    }
    new_node->priority = insert_node->priority;
    new_node->callback = insert_node->callback;
    
    /*
    ** Insert a new node in the list and return TRUE.
    */
    new_node->next = current;
    linkp->next = new_node;
    linkp->task_nbr_cnt++;
    return TRUE;
}

uint32_t task_delete( p_task_link_list_t linkp, task_cb_t *del_node ) {
    task_cb_t *current;
    
    if ( NULL == linkp ) {
        return FALSE;
    }
    
    /*
    ** Find the deleted node by function address and free the node memory.
    */
    while ( NULL != ( current = linkp->next ) && current->callback == del_node->callback ) {
        linkp->next = current->next;
    }
    
    if (NULL != current) {
        UNMOUNT( linkp->next, current );
        xfree( current );
        linkp->task_nbr_cnt--;
        return TRUE;
    }
    return FALSE;
}

void task_traverse( void ) {
    p_task_cb_t current;
    p_task_link_list_t linkp = p_task_link_list;
    msg_node_t *p_msg;
    
    while ( ( NULL != ( current = linkp->next ) ) ) {
        
        /*
        ** Perform related operations if task message list is not empty.
        */
        if ( ( NULL != ( p_msg = current->p_msg_handler->next ) ) ) {
            
            /*
            ** Excute current function.
            */
            current->callback( current, p_msg );
        }
        linkp->next = current->next;
    }
    
    //
    
}

void task_init( void ) {
    p_task_link_list = ( p_task_link_list_t )xmalloc( sizeof( task_link_list_t ) );
    if ( NULL != p_task_link_list ) {
        p_task_link_list->task_nbr_cnt = 0;
        p_task_link_list->next = NULL;
    }
}

void *task_create( const char *             func_name,      \
                   task_func_callback_t     callback,       \
                   uint32_t                 priority,       \
                   p_msg_sll_t              msg_sll ) {
                       
    task_cb_t *  task_node = NULL;
     
    if ( NULL == callback ) {
        return NULL;
    }
    
    task_node = ( task_cb_t * )xmalloc( sizeof( task_cb_t ) );
    if ( NULL == task_node ) {
        return NULL;
    }
    
    task_node->p_msg_handler    = ( p_msg_sll_t )msg_sll;
    task_node->next             = NULL;
    task_node->priority         = priority;
    task_node->callback         = callback;
    
    if ( !task_insert( p_task_link_list, task_node ) ) {
        LOG( "%s task create failed.\r\n", func_name );
        return NULL;
    }
    return ( void * )task_node;
}

