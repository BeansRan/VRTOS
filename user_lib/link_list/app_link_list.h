#ifndef LIST_INCLUDE
#define LIST_INCLUDE

#include <stdint.h>

typedef void* elem_type_t;

typedef struct NODE {
    struct NODE         *next;
    elem_type_t         data;
}node_t;


typedef struct LIST {
    node_t *head;
    uint32_t node_size;
    node_t *tail;
}list_t;

#define NODE_SIZE               sizeof(node_t)

#undef LIST_INCLUDE
#endif // LIST_H

