#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "logger.h"

#define BUFFER_SIZE 5
#define MESSAGE_SIZE 256
#define TOTAL_MESSAGES 12

// Bounded buffer
char buffer[BUFFER_SIZE][MESSAGE_SIZE];

int in = 0;
int out = 0;
int count = 0;

int producerDone = 0;

// Synchronization variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t notFull = PTHREAD_COND_INITIALIZER;
pthread_cond_t notEmpty = PTHREAD_COND_INITIALIZER;

// Producer thread
void *producer(void *arg)
{
    (void)arg;

    for (int i = 1; i <= TOTAL_MESSAGES; i++) {

        pthread_mutex_lock(&mutex);

        // Wait while the buffer is full.
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&notFull, &mutex);
        }

        snprintf(
            buffer[in],
            MESSAGE_SIZE,
            "Log message %d: CampusCompute event recorded",
            i
        );

        printf("[PRODUCER] Generated message %d\n", i);

        in = (in + 1) % BUFFER_SIZE;
        count++;

        // Notify consumer that data is available.
        pthread_cond_signal(&notEmpty);

        pthread_mutex_unlock(&mutex);
    }

    // Tell consumer that production is finished.
    pthread_mutex_lock(&mutex);

    producerDone = 1;

    pthread_cond_broadcast(&notEmpty);

    pthread_mutex_unlock(&mutex);

    return NULL;
}

// Consumer thread
void *consumer(void *arg)
{
    (void)arg;

    FILE *file = fopen("logs/campuscompute.log", "a");

    if (file == NULL) {
        perror("Unable to open log file");
    }

    while (1) {

        char message[MESSAGE_SIZE];

        pthread_mutex_lock(&mutex);

        // Wait while buffer is empty and producer is active.
        while (count == 0 && !producerDone) {
            pthread_cond_wait(&notEmpty, &mutex);
        }

        // Stop only after all messages are consumed.
        if (count == 0 && producerDone) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        snprintf(message, MESSAGE_SIZE, "%s", buffer[out]);

        out = (out + 1) % BUFFER_SIZE;
        count--;

        // Notify producer that space is available.
        pthread_cond_signal(&notFull);

        pthread_mutex_unlock(&mutex);

        printf("[CONSUMER] Processing: %s\n", message);

        if (file != NULL) {
            fprintf(file, "%s\n", message);
            fflush(file);
        }
    }

    if (file != NULL) {
        fclose(file);
    }

    return NULL;
}

// Main logger function
void runLogger(void)
{
    pthread_t producerThread;
    pthread_t consumerThread;

    printf("\n========== PRODUCER-CONSUMER LOGGER ==========\n");

    // Reset state for another execution.
    pthread_mutex_lock(&mutex);

    in = 0;
    out = 0;
    count = 0;
    producerDone = 0;

    pthread_mutex_unlock(&mutex);

    // Start consumer first.
    int result = pthread_create(
        &consumerThread,
        NULL,
        consumer,
        NULL
    );

    if (result != 0) {
        printf("Consumer thread creation failed: %s\n",
               strerror(result));
        return;
    }

    // Start producer.
    result = pthread_create(
        &producerThread,
        NULL,
        producer,
        NULL
    );

    if (result != 0) {

        printf("Producer thread creation failed: %s\n",
               strerror(result));

        pthread_mutex_lock(&mutex);

        producerDone = 1;

        pthread_cond_broadcast(&notEmpty);

        pthread_mutex_unlock(&mutex);

        pthread_join(consumerThread, NULL);

        return;
    }

    // Wait for both threads to finish.
    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    printf("\nAll log messages processed successfully.\n");
    printf("Check logs/campuscompute.log for saved messages.\n");
}
