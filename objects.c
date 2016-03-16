#include <stdlib.h>
#include <stdio.h>

#include "primitives.h"
#include "objects.h"

void append_object(const object *X, object_node *list)
{
    object_node newNode = malloc(sizeof(object_node_body)+X->vt->private_data_size);
    INVOKE_VIRTUAL_FUNC(*X, clone, &newNode->element);
    newNode->next = NULL;
    if(!*list)
        *list = newNode;
    else {
        object_node p;
        /* locate the last node */
        for(p=*list; p->next; p=p->next);
        p->next = newNode;
    }
}

void append_light(const light *X, light_node *list)
{
    light_node newNode = malloc(sizeof(light_node_body));
    newNode->element = *X;
    newNode->next = NULL;
    if(!*list)
        *list = newNode;
    else {
        light_node p;
        /* locate the last node */
        for(p=*list; p->next; p=p->next);
        p->next = newNode;
    }
}

void delete_light_list(light_node *list)
{
    while(*list) {
        light_node nextNode= (*list)->next;
        free(*list);
        *list = nextNode;
    }
}

void delete_object_list(object_node *list)
{
    while(*list) {
        object_node nextNode= (*list)->next;
        /* release private data */
        INVOKE_VIRTUAL_FUNC((*list)->element, release);
        free(*list);
        *list = nextNode;
    }
}

// *INDENT-ON*
