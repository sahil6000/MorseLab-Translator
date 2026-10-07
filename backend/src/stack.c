#include <stdlib.h>

#include "stack.h"

Stack *stack_create(void)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));

    if (stack == NULL)
    {
        return NULL;
    }

    stack->top = NULL;
    stack->size = 0;

    return stack;
}

int stack_push(Stack *stack, char data)
{
    if (stack == NULL)
    {
        return 0;
    }

    StackNode *node = (StackNode *)malloc(sizeof(StackNode));

    if (node == NULL)
    {
        return 0;
    }

    node->data = data;
    node->next = stack->top;

    stack->top = node;
    stack->size++;

    return 1;
}

int stack_pop(Stack *stack, char *data)
{
    if (stack == NULL || stack->top == NULL || data == NULL)
    {
        return 0;
    }

    StackNode *node = stack->top;

    *data = node->data;
    stack->top = node->next;

    free(node);

    stack->size--;

    return 1;
}

int stack_peek(const Stack *stack, char *data)
{
    if (stack == NULL || stack->top == NULL || data == NULL)
    {
        return 0;
    }

    *data = stack->top->data;

    return 1;
}

int stack_is_empty(const Stack *stack)
{
    if (stack == NULL)
    {
        return 1;
    }

    return stack->top == NULL;
}

void stack_free(Stack *stack)
{
    if (stack == NULL)
    {
        return;
    }

    StackNode *current = stack->top;

    while (current != NULL)
    {
        StackNode *next = current->next;
        free(current);
        current = next;
    }

    free(stack);
}