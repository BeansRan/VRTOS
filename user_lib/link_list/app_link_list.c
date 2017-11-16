#include "portmacro.h"
#include "app_link_list.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

node_t *node_create (void *data, size_t size) {
    node_t *node = NULL;
         
    if (!data) {
        node = (node_t *)xmalloc (NODE_SIZE);
        if (node) {
            return NULL;
        }
        memcpy (node->data, data, size);
        return node;
    }
    
    return NULL;
}

list_t *list_create (void) {
    list_t *new_list = NULL;
    
    new_list = (list_t *)xmalloc (NODE_SIZE);
    if (new_list) {
        new_list->head = NULL;
        new_list->node_size = 0;
        new_list->tail = new_list->head;
    }
    
    return new_list;
}

uint32_t list_insert (list_t *list, node_t *node) {
    uint32_t node_addr = 0;
    
    if (!list || !node) {
        return 0;
    }
    
    
    return node_addr;
}

// LIST_H
