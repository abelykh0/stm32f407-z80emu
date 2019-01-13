#include "queue.h"

#define QUEUE_SIZE (QUEUE_ELEMENTS + 1)

static int Queue[QUEUE_SIZE];
static uint8_t QueueIn, QueueOut;

void QueueInit()
{
    QueueIn = QueueOut = 0;
}

int QueuePut(int32_t element)
{
    if (QueueIn == ((QueueOut - 1 + QUEUE_SIZE) % QUEUE_SIZE))
    {
        return -1; /* Queue Full*/
    }

    Queue[QueueIn] = element;
    QueueIn = (QueueIn + 1) % QUEUE_SIZE;

    return 0; // No errors
}

int QueueGet(int32_t* element)
{
    if (QueueIn == QueueOut)
    {
        return -1; /* Queue Empty - nothing to get*/
    }

    *element = Queue[QueueOut];
    QueueOut = (QueueOut + 1) % QUEUE_SIZE;

    return 0; // No errors
}
