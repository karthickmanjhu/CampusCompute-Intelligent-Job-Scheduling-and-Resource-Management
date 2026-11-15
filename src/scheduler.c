#include <stdio.h>
#include <string.h>

#include "scheduler.h"

#define MAX_JOBS 10
#define MAX_SEGMENTS 100005

typedef struct {
    int id;
    int arrival;
    int burst;
    int priority;
    int remaining;
    int completion;
    int turnaround;
    int waiting;
} Job;

typedef struct {
    int pid;
    int start;
    int end;
} Segment;

int readNumber(const char *message, int min, int max)
{
    int value;

    while (1) {
        printf("%s", message);

        if (scanf("%d", &value) != 1) {
            printf("Invalid input. Enter an integer.\n");

            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;

            continue;
        }

        if (value < min || value > max) {
            printf("Enter a value between %d and %d.\n", min, max);
            continue;
        }

        return value;
    }
}

void addSegment(
    Segment segments[],
    int *count,
    int pid,
    int start,
    int end
)
{
    if (*count < MAX_SEGMENTS) {
        segments[*count].pid = pid;
        segments[*count].start = start;
        segments[*count].end = end;
        (*count)++;
    }
}

void printResults(
    const char *algorithm,
    Job jobs[],
    int n,
    Segment segments[],
    int segmentCount
)
{
    float totalWaiting = 0;
    float totalTurnaround = 0;

    printf("\n========== %s ==========\n", algorithm);

    printf("\nGantt Chart:\n");

    for (int i = 0; i < segmentCount; i++) {
        if (segments[i].pid == -1) {
            printf("| IDLE (%d-%d) ",
                   segments[i].start, segments[i].end);
        } else {
            printf("| P%d (%d-%d) ",
                   segments[i].pid,
                   segments[i].start,
                   segments[i].end);
        }
    }

    printf("|\n");

    printf("\nProcess\tAT\tBT\tPriority\tCT\tTAT\tWT\n");

    for (int i = 0; i < n; i++) {
        jobs[i].turnaround =
            jobs[i].completion - jobs[i].arrival;

        jobs[i].waiting =
            jobs[i].turnaround - jobs[i].burst;

        totalWaiting += jobs[i].waiting;
        totalTurnaround += jobs[i].turnaround;

        printf(
            "P%d\t%d\t%d\t%d\t\t%d\t%d\t%d\n",
            jobs[i].id,
            jobs[i].arrival,
            jobs[i].burst,
            jobs[i].priority,
            jobs[i].completion,
            jobs[i].turnaround,
            jobs[i].waiting
        );
    }

    printf("\nAverage Waiting Time: %.2f",
           totalWaiting / n);

    printf("\nAverage Turnaround Time: %.2f\n",
           totalTurnaround / n);
}

// FCFS Scheduling
void fcfs(Job original[], int n)
{
    Job jobs[MAX_JOBS];
    Segment segments[MAX_SEGMENTS];
    int segmentCount = 0;
    int time = 0;

    memcpy(jobs, original, sizeof(Job) * n);

    // Sort by arrival time, then input order.
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (jobs[j].arrival < jobs[i].arrival) {
                Job temp = jobs[i];
                jobs[i] = jobs[j];
                jobs[j] = temp;
            }
        }
    }

    for (int i = 0; i < n; i++) {

        if (time < jobs[i].arrival) {
            addSegment(
                segments, &segmentCount,
                -1, time, jobs[i].arrival
            );

            time = jobs[i].arrival;
        }

        int start = time;
        time += jobs[i].burst;

        jobs[i].completion = time;

        addSegment(
            segments, &segmentCount,
            jobs[i].id, start, time
        );
    }

    printResults("FCFS SCHEDULING",
                 jobs, n, segments, segmentCount);
}

// Non-Preemptive Priority Scheduling
// Smaller priority number means higher priority.
void priorityScheduling(Job original[], int n)
{
    Job jobs[MAX_JOBS];
    Segment segments[MAX_SEGMENTS];

    int completed[MAX_JOBS] = {0};
    int segmentCount = 0;
    int time = 0;
    int finished = 0;

    memcpy(jobs, original, sizeof(Job) * n);

    while (finished < n) {

        int selected = -1;

        for (int i = 0; i < n; i++) {

            if (completed[i] || jobs[i].arrival > time)
                continue;

            if (selected == -1 ||
                jobs[i].priority < jobs[selected].priority ||
                (jobs[i].priority == jobs[selected].priority &&
                 jobs[i].arrival < jobs[selected].arrival)) {

                selected = i;
            }
        }

        // No process is ready.
        if (selected == -1) {

            int nextArrival = -1;

            for (int i = 0; i < n; i++) {
                if (!completed[i] &&
                    (nextArrival == -1 ||
                     jobs[i].arrival < nextArrival)) {
                    nextArrival = jobs[i].arrival;
                }
            }

            addSegment(
                segments, &segmentCount,
                -1, time, nextArrival
            );

            time = nextArrival;
            continue;
        }

        int start = time;

        time += jobs[selected].burst;

        jobs[selected].completion = time;
        completed[selected] = 1;
        finished++;

        addSegment(
            segments, &segmentCount,
            jobs[selected].id, start, time
        );
    }

    printResults("NON-PREEMPTIVE PRIORITY SCHEDULING",
                 jobs, n, segments, segmentCount);
}

// Round Robin Scheduling
void roundRobin(Job original[], int n, int quantum)
{
    Job jobs[MAX_JOBS];
    Segment segments[MAX_SEGMENTS];

    int queue[MAX_JOBS + 1];
    int head = 0, tail = 0, count = 0;

    int order[MAX_JOBS];
    int nextArrival = 0;

    int segmentCount = 0;
    int time = 0;
    int finished = 0;

    memcpy(jobs, original, sizeof(Job) * n);

    for (int i = 0; i < n; i++) {
        jobs[i].remaining = jobs[i].burst;
        order[i] = i;
    }

    // Sort indices by arrival time.
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (jobs[order[j]].arrival <
                jobs[order[i]].arrival) {

                int temp = order[i];
                order[i] = order[j];
                order[j] = temp;
            }
        }
    }

    // Initially enqueue all arrived processes.
    while (nextArrival < n &&
           jobs[order[nextArrival]].arrival <= time) {

        queue[tail] = order[nextArrival];
        tail = (tail + 1) % (MAX_JOBS + 1);
        count++;
        nextArrival++;
    }

    while (finished < n) {

        // CPU is idle until the next process arrives.
        if (count == 0) {

            if (nextArrival < n) {
                int arrival = jobs[order[nextArrival]].arrival;

                addSegment(
                    segments, &segmentCount,
                    -1, time, arrival
                );

                time = arrival;

                while (nextArrival < n &&
                       jobs[order[nextArrival]].arrival <= time) {

                    queue[tail] = order[nextArrival];
                    tail = (tail + 1) % (MAX_JOBS + 1);
                    count++;
                    nextArrival++;
                }
            }
        }

        // Remove process from ready queue.
        int index = queue[head];

        head = (head + 1) % (MAX_JOBS + 1);
        count--;

        int start = time;

        int executionTime =
            jobs[index].remaining < quantum
            ? jobs[index].remaining
            : quantum;

        time += executionTime;
        jobs[index].remaining -= executionTime;

        addSegment(
            segments, &segmentCount,
            jobs[index].id, start, time
        );

        // Add newly arrived processes first.
        while (nextArrival < n &&
               jobs[order[nextArrival]].arrival <= time) {

            queue[tail] = order[nextArrival];
            tail = (tail + 1) % (MAX_JOBS + 1);
            count++;
            nextArrival++;
        }

        if (jobs[index].remaining > 0) {

            // Process goes to the end of the queue.
            queue[tail] = index;
            tail = (tail + 1) % (MAX_JOBS + 1);
            count++;

        } else {
            jobs[index].completion = time;
            finished++;
        }
    }

    printResults("ROUND ROBIN SCHEDULING",
                 jobs, n, segments, segmentCount);
}

// Main scheduling function
void runScheduler(void)
{
    Job jobs[MAX_JOBS];

    printf("\n========== CPU SCHEDULING ==========\n");

    int n = readNumber("Enter number of processes (1-10): ",
                       1, MAX_JOBS);

    for (int i = 0; i < n; i++) {

        jobs[i].id = i + 1;

        printf("\nProcess P%d\n", i + 1);

        jobs[i].arrival =
            readNumber("Arrival Time: ", 0, 100000);

        jobs[i].burst =
            readNumber("Burst Time: ", 1, 10000);

        jobs[i].priority =
            readNumber("Priority (smaller = higher): ",
                       0, 100000);
    }

    int quantum =
        readNumber("\nEnter Round Robin Time Quantum: ",
                   1, 10000);

    fcfs(jobs, n);

    priorityScheduling(jobs, n);

    roundRobin(jobs, n, quantum);
}
