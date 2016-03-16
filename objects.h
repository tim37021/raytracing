#ifndef __RAY_OBJECTS_H
#define __RAY_OBJECTS_H

struct _OBJECT_NODE;
typedef struct _OBJECT_NODE *object_node;
typedef struct _OBJECT_NODE object_node_body;
struct _OBJECT_NODE {
    object_node next;
    object element;
};

void append_object(const object *X, object_node *list);
void delete_object_list(object_node *list);

struct _LIGHT_NODE;
typedef struct _LIGHT_NODE *light_node;
typedef struct _LIGHT_NODE light_node_body;
struct _LIGHT_NODE {
    light_node next;
    light element;
};

void append_light(const light *X, light_node *list);
void delete_light_list(light_node *list);


/* the following is gcc extension, allowing zero argument */
#define INVOKE_VIRTUAL_FUNC(obj, member, ...) (obj).vt->member(&(obj), ##__VA_ARGS__)

#endif
