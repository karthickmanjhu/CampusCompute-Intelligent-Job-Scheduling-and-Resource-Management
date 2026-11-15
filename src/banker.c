#include <stdio.h>
#include "banker.h"

void calculateNeed(
    int need[MAX_PROCESSES][MAX_RESOURCES],
    int maximum[MAX_PROCESSES][MAX_RESOURCES],
    int allocation[MAX_PROCESSES][MAX_RESOURCES],
    int processes,
    int resources
)
{
    for (int i = 0; i < processes; i++)
    {
        for (int j = 0; j < resources; j++)
        {
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
}

int isSafe(
    int processes,
    int resources,
    int available[MAX_RESOURCES],
    int allocation[MAX_PROCESSES][MAX_RESOURCES],
    int need[MAX_PROCESSES][MAX_RESOURCES]
)
{
    int work[MAX_RESOURCES];
    int finish[MAX_PROCESSES] = {0};
    int safeSequence[MAX_PROCESSES];

    // Copy available resources into work
    for (int i = 0; i < resources; i++)
    {
        work[i] = available[i];
    }

    int count = 0;

    while (count < processes)
    {
        int found = 0;

        for (int p = 0; p < processes; p++)
        {
            if (finish[p] == 0)
            {
                int canExecute = 1;

                for (int r = 0; r < resources; r++)
                {
                    if (need[p][r] > work[r])
                    {
                        canExecute = 0;
                        break;
                    }
                }

                if (canExecute)
                {
                    for (int r = 0; r < resources; r++)
                    {
                        work[r] += allocation[p][r];
                    }

                    safeSequence[count] = p;
                    count++;

                    finish[p] = 1;
                    found = 1;
                }
            }
        }

        if (!found)
        {
            printf("\nSystem is in an UNSAFE state!\n");
            return 0;
        }
    }

    printf("\nSystem is in a SAFE state!\n");
    printf("Safe Sequence: ");

    for (int i = 0; i < processes; i++)
    {
        printf("P%d", safeSequence[i]);

        if (i != processes - 1)
            printf(" -> ");
    }

    printf("\n");

    return 1;
}

void runBanker(void)
{
    int processes = 5;
    int resources = 3;

    int available[MAX_RESOURCES] = {3, 3, 2};

    int allocation[MAX_PROCESSES][MAX_RESOURCES] = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };

    int maximum[MAX_PROCESSES][MAX_RESOURCES] = {
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };

    int need[MAX_PROCESSES][MAX_RESOURCES];

    printf("\n========== BANKER'S ALGORITHM ==========\n");

    printf("\nAvailable Resources:\n");

    for (int i = 0; i < resources; i++)
    {
        printf("%d ", available[i]);
    }

    printf("\n\nCalculating Need Matrix...\n");

    calculateNeed(need, maximum, allocation, processes, resources);

    printf("\nNeed Matrix:\n");

    for (int i = 0; i < processes; i++)
    {
        printf("P%d: ", i);

        for (int j = 0; j < resources; j++)
        {
            printf("%d ", need[i][j]);
        }

        printf("\n");
    }

    isSafe(processes, resources, available, allocation, need);

    printf("\n========================================\n");
}

