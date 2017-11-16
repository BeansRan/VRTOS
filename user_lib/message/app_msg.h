#ifndef APP_MSG_INCLUDE
#define APP_MSG_INCLUDE 

#include <stdio.h>
#include <stdint.h>

typedef void *                  p_msg_handler_t;
typedef char *                  p_msg_data_t;

#define MSG_SIZE                sizeof( msg_node_t )

typedef struct MSG_NODE {
    struct MSG_NODE *           next;
    uint32_t                    type;
    uint32_t                    length;
    p_msg_data_t                data;
}msg_node_t;

typedef msg_node_t *            p_msg_node_t;

typedef struct MSG_SLL {
    uint32_t                    msg_nbr_cnt;
    p_msg_node_t                next;
}msg_sll_t;

typedef msg_sll_t *             p_msg_sll_t;

p_msg_node_t    app_msg_create( uint32_t msg_type, p_msg_data_t p_msg_data, uint32_t length );
uint32_t        app_msg_delete( p_msg_sll_t *msg_sll, p_msg_node_t msg_node );
uint32_t        app_msg_post( p_msg_sll_t msg_sll, p_msg_node_t msg_node, uint32_t post_delay_ticks );
void *          app_msg_pend( p_msg_handler_t msg_sll );

//#undef APP_MSG_INCLUDE
#endif
