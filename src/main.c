#include <stdio.h>
#include <stdlib.h>

#include "banker.h"
#include "ipc.h"
#include "worker.h"
#include "resource_limits.h"
#include "scheduler.h"
#include "logger.h"

void displayMenu(void)
{
    printf("\n");
    printf("================================================\n");
    printf("       CAMPUSCOMPUTE - INTELLIGENT RESOURCE MANAGER\n");
    printf("================================================\n");
    printf("  1. Banker's Algorithm\n");
    printf("  2. Inter-Process Communication (IPC)\n");
    printf("  3. Process Management\n");
    printf("  4. Resource Limits\n");
    printf("  5. CPU Scheduling\n");
    printf("  6. Producer-Consumer Logger\n");
    printf("  7. Exit\n");
    printf("================================================\n");
    printf("Enter your choice: ");
}

int main(void)
{
    int choice;

    while (1)
    {
        displayMenu();

        if (scanf("%d", &choice) != 1)
        {
            printf("\nInvalid input! Please enter a number.\n");

            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;

            continue;
        }

        switch (choice)
        {
            case 1:
                runBanker();
                break;

            case 2:
                runIPC();
                break;

            case 3:
                runWorker();
                break;

            case 4:
                runResourceLimits();
                break;

            case 5:
                runScheduler();
                break;

            case 6:
                runLogger();
                break;

            case 7:
                printf("\nThank you for using CampusCompute!\n");
                return 0;

            default:
                printf("\nInvalid choice! Please select between 1 and 7.\n");
        }

        printf("\nPress Enter to return to the main menu...");

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;

        getchar();
    }

    return 0;
}
