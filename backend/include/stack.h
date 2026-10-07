#ifndef STACK_H
#define STACK_H

typedef struct StackNode
{
    char data;
    struct StackNode *next;
} StackNode;

typedef struct Stack
{
    StackNode *top;
    int size;
} Stack;

Stack *stack_create(void);

int stack_push(
    Stack *stack,
    char data
);

int stack_pop(
    Stack *stack,
    char *data
);

int stack_peek(
    const Stack *stack,
    char *data
);

int stack_is_empty(
    const Stack *stack
);

void stack_free(
    Stack *stack
);

#endif