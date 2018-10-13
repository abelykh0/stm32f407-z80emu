#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdint.h>

#define QUEUE_ELEMENTS 6

#ifdef __cplusplus
extern "C" {
#endif

void QueueInit();
int QueuePut(int32_t element);
int QueueGet(int32_t *element);

#ifdef __cplusplus
}
#endif

#endif