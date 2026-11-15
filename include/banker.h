#ifndef BANKER_H
#define BANKER_H

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

void calculateNeed(
    int need[MAX_PROCESSES][MAX_RESOURCES],
    int maximum[MAX_PROCESSES][MAX_RESOURCES],
    int allocation[MAX_PROCESSES][MAX_RESOURCES],
    int processes,
    int resources
);

int isSafe(
    int processes,
    int resources,
    int available[MAX_RESOURCES],
    int allocation[MAX_PROCESSES][MAX_RESOURCES],
    int need[MAX_PROCESSES][MAX_RESOURCES]
);

void runBanker(void);

#endif