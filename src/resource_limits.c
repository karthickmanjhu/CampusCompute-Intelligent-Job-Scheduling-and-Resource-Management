#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <errno.h>
#include <string.h>

#include "resource_limits.h"

void runResourceLimits(void)
{
    struct rlimit limit;

    printf("\n========== RESOURCE LIMIT DEMONSTRATION ==========\n");

    // Get the current file descriptor limit.
    if (getrlimit(RLIMIT_NOFILE, &limit) == -1) {
        perror("getrlimit failed");
        return;
    }

    printf("\nOriginal file descriptor limits:\n");
    printf("Soft limit: %ld\n", (long)limit.rlim_cur);
    printf("Hard limit: %ld\n", (long)limit.rlim_max);

    // Set a smaller soft limit.
    struct rlimit newLimit = limit;

    if (newLimit.rlim_cur > 50) {
        newLimit.rlim_cur = 50;
    }

    if (setrlimit(RLIMIT_NOFILE, &newLimit) == -1) {
        perror("setrlimit failed");
        return;
    }

    // Verify the updated limit.
    if (getrlimit(RLIMIT_NOFILE, &limit) == -1) {
        perror("getrlimit failed");
        return;
    }

    printf("\nUpdated file descriptor limits:\n");
    printf("Soft limit: %ld\n", (long)limit.rlim_cur);
    printf("Hard limit: %ld\n", (long)limit.rlim_max);

    printf("\nResource limit applied successfully.\n");
}
