#ifndef _IDX_STACK_H_
#define _IDX_STACK_H_

#define MAX_STACK_SIZE 16

struct _OBJECT_NODE;
typedef struct _OBJECT_NODE *object_node;
typedef struct {
    double idx;
    object_node obj;
} idx_stack_element;

#define AIR_ELEMENT (idx_stack_element) {.obj=NULL, .idx=1.0}

typedef struct {
    idx_stack_element data[MAX_STACK_SIZE];
    int top;
} idx_stack;

static inline void idx_stack_init(idx_stack *stk)
{
    stk->top = 0;
}
static inline void idx_stack_push(idx_stack *stk, idx_stack_element element)
{
    if(stk->top < MAX_STACK_SIZE)
        stk->data[stk->top++] = element;
}

static inline int idx_stack_empty(idx_stack *stk)
{
    return !stk->top;
}

static inline idx_stack_element idx_stack_pop(idx_stack *stk)
{
    if(!idx_stack_empty(stk))
        return stk->data[--stk->top];
    else
        return AIR_ELEMENT;
}

static inline idx_stack_element idx_stack_top(idx_stack *stk)
{
    if(!idx_stack_empty(stk))
        return stk->data[stk->top-1];
    else
        return AIR_ELEMENT;
}

#endif