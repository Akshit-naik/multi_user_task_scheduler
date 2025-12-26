#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "job.h"
#include "queue.h"
#include "protocol.h"

static int job_counter = 1;

void *worker_thread(void *arg);

void *client_handler(void *arg)
{
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[512];

    printf("Client connected\n");
    fflush(stdout);

    while (1) {
        int n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("Client disconnected\n");
            fflush(stdout);
            break;
        }

        buffer[n] = '\0';   // 🔥 CRITICAL

        printf("Received: [%s]\n", buffer);
        fflush(stdout);

        if (strncmp(buffer, "RUN ", 4) == 0) {
            job_t *job = calloc(1, sizeof(job_t));
            job->job_id = job_counter++;
            strncpy(job->command, buffer + 4, sizeof(job->command) - 1);
            job->command[strcspn(job->command, "\n")] = 0;
            job->status = JOB_PENDING;
            job->client_fd = client_fd;

            enqueue_job(job);

            dprintf(client_fd, "Job submitted. ID=%d\n", job->job_id);
        } else {
            dprintf(client_fd, "Invalid command\n");
        }
    }

    close(client_fd);
    return NULL;
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, MAX_CLIENTS);

    queue_init();

    pthread_t worker;
    pthread_create(&worker, NULL, worker_thread, NULL);

    printf("Task Scheduler Server running on port %d\n", SERVER_PORT);

int client_fd = accept(server_fd, NULL, NULL);
printf("Accepted new client\n");
fflush(stdout);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        printf("Client connected\n");
	int *p = malloc(sizeof(int));
        *p = client_fd;

        pthread_t tid;
        pthread_create(&tid, NULL, client_handler, p);
        pthread_detach(tid);
    }
}

