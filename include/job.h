#ifndef JOB_H
#define JOB_H

#include <pthread.h>

typedef enum {
    JOB_PENDING,
    JOB_RUNNING,
    JOB_COMPLETED,
    JOB_FAILED
} job_status_t;

typedef struct job {
    int job_id;
    char command[256];
    job_status_t status;
    int client_fd;
    struct job *next;
} job_t;

#endif

