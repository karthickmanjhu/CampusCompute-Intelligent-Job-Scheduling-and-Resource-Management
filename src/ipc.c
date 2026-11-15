#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>

#include "ipc.h"

#define SHM_NAME "/campuscompute_shm"
#define SEM_NAME "/campuscompute_sem"

#define SHM_SIZE 1024

void runIPC(void)
{
    int shm_fd;
    char *shared_data;
    sem_t *semaphore;

    printf("\n========== IPC DEMONSTRATION ==========\n");

    // Remove leftover resources from an earlier run.
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);

    // Create shared memory.
    shm_fd = shm_open(
        SHM_NAME,
        O_CREAT | O_EXCL | O_RDWR,
        0666
    );

    if (shm_fd == -1) {
        perror("shm_open failed");
        return;
    }

    // Set shared memory size.
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate failed");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return;
    }

    // Map shared memory into this process.
    shared_data = mmap(
        NULL,
        SHM_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_fd,
        0
    );

    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return;
    }

    // Create named semaphore.
    semaphore = sem_open(
        SEM_NAME,
        O_CREAT | O_EXCL,
        0666,
        1
    );

    if (semaphore == SEM_FAILED) {
        perror("sem_open failed");
        munmap(shared_data, SHM_SIZE);
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return;
    }

    // Write data while holding the semaphore.
    if (sem_wait(semaphore) == -1) {
        perror("sem_wait failed");
    } else {
        snprintf(
            shared_data,
            SHM_SIZE,
            "Hello from CampusCompute! Shared memory is working."
        );

        printf("Data written to shared memory:\n%s\n", shared_data);

        if (sem_post(semaphore) == -1) {
            perror("sem_post failed");
        }
    }

    // Release resources.
    sem_close(semaphore);
    sem_unlink(SEM_NAME);

    munmap(shared_data, SHM_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);

    printf("IPC resources cleaned up successfully.\n");
}
