#include <pthread.h>
#include <stdlib.h>
#include "queue.h"

static job_t *head = NULL;
static job_t *tail = NULL;

pthread_mutex_t queue_mutex;
pthread_cond_t queue_cond;

void queue_init()
{
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond, NULL);
}

void enqueue_job(job_t *job)
{
    pthread_mutex_lock(&queue_mutex);

    job->next = NULL;
    if (!tail) {
        head = tail = job;
    } else {
        tail->next = job;
        tail = job;
    }

    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
}

job_t *dequeue_job()
{
    pthread_mutex_lock(&queue_mutex);

    while (!head)
        pthread_cond_wait(&queue_cond, &queue_mutex);

    job_t *job = head;
    head = head->next;
    if (!head)
        tail = NULL;

    pthread_mutex_unlock(&queue_mutex);
    return job;
}

