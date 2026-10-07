#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

int main(void)
{
    Queue *queue = queue_create();

    if (queue == NULL)
    {
        printf("Failed to create queue.\n");
        return 1;
    }

    printf("Enqueuing operations:\n");

    queue_enqueue(queue, 1, "TRANSLATE_TEXT_TO_MORSE");
    queue_enqueue(queue, 2, "TRANSLATE_MORSE_TO_TEXT");
    queue_enqueue(queue, 3, "SAVE_TRANSLATION");

    int peek_id;
    const char *peek_operation;

    if (queue_peek(queue, &peek_id, &peek_operation))
    {
        printf(
            "Front operation: ID %d | %s\n",
            peek_id,
            peek_operation
        );
    }

    printf("\nProcessing operations:\n");

    int operation_id;
    char *operation;

    while (!queue_is_empty(queue))
    {
        if (queue_dequeue(queue, &operation_id, &operation))
        {
            printf(
                "Processed: ID %d | %s\n",
                operation_id,
                operation
            );

            free(operation);
        }
    }

    printf(
        "\nQueue empty: %s\n",
        queue_is_empty(queue) ? "YES" : "NO"
    );

    queue_free(queue);

    return 0;
}