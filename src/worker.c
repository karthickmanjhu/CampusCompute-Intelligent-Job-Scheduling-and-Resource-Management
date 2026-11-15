#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "worker.h"

void runWorker(void)
{
    pid_t pid;
    int status;

    printf("\n========== PROCESS MANAGEMENT ==========\n");

    // Scenario 1: Normal child execution
    printf("\n--- Normal Execution ---\n");

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        // Child process
        printf("Child process started. PID: %d\n", getpid());
        printf("Child process completed successfully.\n");
        exit(0);
    } else {
        // Parent process
        printf("Parent waiting for child PID: %d\n", pid);

        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return;
        }

        if (WIFEXITED(status)) {
            printf(
                "Child exited with status: %d\n",
                WEXITSTATUS(status)
            );
        }
    }

    // Scenario 2: Simulated child failure
    printf("\n--- Simulated Failure ---\n");

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        // Child deliberately exits with failure status.
        printf("Child process simulating failure.\n");
        exit(1);
    } else {
        printf("Parent waiting for child PID: %d\n", pid);

        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return;
        }

        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);

            if (exitCode != 0) {
                printf(
                    "Child failure detected! Exit code: %d\n",
                    exitCode
                );
            } else {
                printf("Child completed successfully.\n");
            }
        } else if (WIFSIGNALED(status)) {
            printf(
                "Child terminated by signal: %d\n",
                WTERMSIG(status)
            );
        }
    }

    printf("\nProcess management demonstration completed.\n");
}
