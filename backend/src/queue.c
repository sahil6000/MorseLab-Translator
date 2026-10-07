#include <stdlib.h>
#include <string.h>

#include "queue.h"

static char *duplicate_string(const char *source)
{
    if (source == NULL)
    {
        return NULL;
    }

    size_t length = strlen(source);

    char *copy = (char *)malloc(length + 1);

    if (copy == NULL)
    {
        return NULL;
    }

    strcpy(copy, source);

    return copy;
}

Queue *queue_create(void)
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));

    if (queue == NULL)
    {
        return NULL;
    }

    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;

    return queue;
}

int queue_enqueue(
    Queue *queue,
    int operation_id,
    const char *operation)
{
    if (queue == NULL || operation == NULL)
    {
        return 0;
    }

    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));

    if (node == NULL)
    {
        return 0;
    }

    node->operation = duplicate_string(operation);

    if (node->operation == NULL)
    {
        free(node);
        return 0;
    }

    node->operation_id = operation_id;
    node->next = NULL;

    if (queue->rear == NULL)
    {
        queue->front = node;
        queue->rear = node;
    }
    else
    {
        queue->rear->next = node;
        queue->rear = node;
    }

    queue->size++;

    return 1;
}

int queue_dequeue(
    Queue *queue,
    int *operation_id,
    char **operation)
{
    if (queue == NULL ||
        queue->front == NULL ||
        operation_id == NULL ||
        operation == NULL)
    {
        return 0;
    }

    QueueNode *node = queue->front;

    *operation_id = node->operation_id;
    *operation = node->operation;

    queue->front = node->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(node);

    queue->size--;

    return 1;
}

int queue_peek(
    const Queue *queue,
    int *operation_id,
    const char **operation)
{
    if (queue == NULL ||
        queue->front == NULL ||
        operation_id == NULL ||
        operation == NULL)
    {
        return 0;
    }

    *operation_id = queue->front->operation_id;
    *operation = queue->front->operation;

    return 1;
}

int queue_is_empty(const Queue *queue)
{
    if (queue == NULL)
    {
        return 1;
    }

    return queue->front == NULL;
}

void queue_free(Queue *queue)
{
    if (queue == NULL)
    {
        return;
    }

    QueueNode *current = queue->front;

    while (current != NULL)
    {
        QueueNode *next = current->next;

        free(current->operation);
        free(current);

        current = next;
    }

    free(queue);
}