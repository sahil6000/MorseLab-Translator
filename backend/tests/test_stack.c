#include <stdio.h>

#include "stack.h"

int main(void)
{
    Stack *stack = stack_create();

    if (stack == NULL)
    {
        printf("Failed to create stack.\n");
        return 1;
    }

    printf("Pushing: A B C D\n");

    stack_push(stack, 'A');
    stack_push(stack, 'B');
    stack_push(stack, 'C');
    stack_push(stack, 'D');

    char top;

    if (stack_peek(stack, &top))
    {
        printf("Top element: %c\n", top);
    }

    printf("\nPopping elements:\n");

    char data;

    while (!stack_is_empty(stack))
    {
        if (stack_pop(stack, &data))
        {
            printf("Popped: %c\n", data);
        }
    }

    printf("\nStack empty: %s\n",
           stack_is_empty(stack) ? "YES" : "NO");

    stack_free(stack);

    return 0;
}