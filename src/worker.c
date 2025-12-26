#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include "queue.h"

void *worker_thread(void *arg)
{
    while (1) {
        job_t *job = dequeue_job();
        job->status = JOB_RUNNING;

        dprintf(job->client_fd, "Job %d started\n", job->job_id);

        pid_t pid = fork();
        if (pid == 0) {
            // Redirect stdout and stderr to client socket
            dup2(job->client_fd, STDOUT_FILENO);
            dup2(job->client_fd, STDERR_FILENO);

            execlp("/bin/sh", "sh", "-c", job->command, NULL);
            _exit(1);
        } else {
            int status;
            waitpid(pid, &status, 0);

            job->status = (WIFEXITED(status)) ?
                          JOB_COMPLETED : JOB_FAILED;

            dprintf(job->client_fd,
                    "\nJob %d finished: %s\n",
                    job->job_id,
                    job->status == JOB_COMPLETED ?
                    "COMPLETED" : "FAILED");
        }
    }
}

