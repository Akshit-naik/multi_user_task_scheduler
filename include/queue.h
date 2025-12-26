#ifndef QUEUE_H
#define QUEUE_H

#include "job.h"

extern pthread_mutex_t queue_mutex;
extern pthread_cond_t queue_cond;

void queue_init();
void enqueue_job(job_t *job);
job_t *dequeue_job();

#endif

