#include <stdlib.h>
#include <stdio.h>

#include "primitives.h"
#include "objects.h"

#define GEN_DELETE_FUNC(name)   \
void delete_##name##_list(name##_node *list) \
{ \
    while(*list) { \
        name##_node nextNode= (*list)->next; \
        free(*list); \
        *list = nextNode; \
    } \
}

void append_object(const object *X, object_node *list)
{
    object_node newNode = malloc(sizeof(object_node_body)+X->vt->private_data_size);
    X->vt->clone(X, &newNode->element);
    newNode->next = NULL;
    if(!*list)
        *list = newNode;
    else
    {
        object_node p;
        /* locate the last node */
        for(p=*list; p->next; p=p->next);
        p->next = newNode;
    }
}

GEN_DELETE_FUNC(object);

void append_light(const light *X, light_node *list)
{
    light_node newNode = malloc(sizeof(light_node_body));
    newNode->element = *X;    
    newNode->next = NULL;
    if(!*list)
        *list = newNode;
    else
    {
        light_node p;
        /* locate the last node */
        for(p=*list; p->next; p=p->next);
        p->next = newNode;
    }
}

GEN_DELETE_FUNC(light);




// *INDENT-ON*
