#ifndef QUEUE_H
#define QUEUE_H

typedef struct QueueNode
{
    int operation_id;
    char *operation;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
    QueueNode *front;
    QueueNode *rear;
    int size;
} Queue;

Queue *queue_create(void);

int queue_enqueue(
    Queue *queue,
    int operation_id,
    const char *operation
);

int queue_dequeue(
    Queue *queue,
    int *operation_id,
    char **operation
);

int queue_peek(
    const Queue *queue,
    int *operation_id,
    const char **operation
);

int queue_is_empty(
    const Queue *queue
);

void queue_free(
    Queue *queue
);

#endif