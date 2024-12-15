#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_TASK_LENGTH 100
#define MAX_TASKS 70
#define NUM_HIGH_CATEGORIES 7
#define NUM_LOW_CATEGORIES 3
#define URGENCY_CATEGORY 1 // Added task urgency

// Function to get a validated score between 1 and 5 or handle "CANCEL"
int get_valid_score(const char *category, int *cancelFlag) {
    int score;
    char input[10];

    while (1) {
        printf("%s (1-5, or type 'CANCEL' to cancel this task): ", category);
        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = '\0'; // Remove newline character

        if (strcmp(input, "CANCEL") == 0) {
            *cancelFlag = 1;
            return -1;
        }

        if (isdigit(input[0]) && (score = atoi(input)) >= 1 && score <= 5) {
            return score;
        } else {
            printf("Invalid input. Please enter a number between 1 and 5.\n");
        }
    }
}

int calculate_task_score(int *scores) {
    int total = 0;

    for (int i = 0; i < NUM_HIGH_CATEGORIES; i++) {
        total += 2 * scores[i];
    }

    for (int i = 0; i < NUM_LOW_CATEGORIES; i++) {
        total += scores[NUM_HIGH_CATEGORIES + i];
    }

    total += scores[NUM_HIGH_CATEGORIES + NUM_LOW_CATEGORIES];
    return total;
}

// Function to identify and resolve ties dynamically
void resolve_ties(int taskScores[], char taskNames[][MAX_TASK_LENGTH], int taskCount) {
    for (int i = 0; i < taskCount - 1; i++) {
        int tieCount = 0;

        // Dynamically allocate memory for tiedIndices
        int *tiedIndices = (int *)malloc(taskCount * sizeof(int));
        if (tiedIndices == NULL) {
            perror("Failed to allocate memory for tiedIndices");
            return;
        }

        // Find all tasks tied with the current task
        tiedIndices[tieCount++] = i;
        for (int j = i + 1; j < taskCount; j++) {
            if (taskScores[i] == taskScores[j]) {
                tiedIndices[tieCount++] = j;
            }
        }

        // If a tie exists, resolve it
        if (tieCount > 1) {
            printf("\nThe following tasks are tied (Score: %d):\n", taskScores[i]);
            for (int k = 0; k < tieCount; k++) {
                printf("%d. %s\n", k + 1, taskNames[tiedIndices[k]]);
            }

            printf("\nEnter the desired order (space-separated, 1-based indices):\n");
            int *order = (int *)malloc(tieCount * sizeof(int));
            if (order == NULL) {
                perror("Failed to allocate memory for order");
                free(tiedIndices);
                return;
            }

            for (int k = 0; k < tieCount; k++) {
                scanf("%d", &order[k]);
                if (order[k] < 1 || order[k] > tieCount) {
                    printf("Invalid input. Please restart the ordering process.\n");
                    free(order);
                    free(tiedIndices);
                    return;
                }
                order[k]--; // Convert to zero-based index
            }
            getchar(); // Consume newline

            // Reorder the tied tasks based on user input
            char (*tempNames)[MAX_TASK_LENGTH] = malloc(tieCount * MAX_TASK_LENGTH);
            int *tempScores = malloc(tieCount * sizeof(int));

            if (tempNames == NULL || tempScores == NULL) {
                perror("Failed to allocate memory for tie resolution");
                free(tiedIndices);
                return;
            }

            for (int k = 0; k < tieCount; k++) {
                strncpy(tempNames[k], taskNames[tiedIndices[order[k]]], MAX_TASK_LENGTH);
                tempScores[k] = taskScores[tiedIndices[order[k]]];
            }

            for (int k = 0; k < tieCount; k++) {
                strncpy(taskNames[tiedIndices[k]], tempNames[k], MAX_TASK_LENGTH);
                taskScores[tiedIndices[k]] = tempScores[k];
            }

            free(order); // Free memory for order
        }

        free(tiedIndices); // Free memory for tiedIndices
    }
}

void save_to_file(char taskNames[][MAX_TASK_LENGTH], int totalScores[], int taskCount) {
    FILE *file = fopen("Task_Rankings.txt", "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    fprintf(file, "Task Rankings:\n");
    for (int i = 0; i < taskCount; i++) {
        fprintf(file, "%d. %s (Total Score: %d)\n", i + 1, taskNames[i], totalScores[i]);
    }

    fclose(file);
    printf("\nResults saved to 'Task_Rankings.txt'.\n");
}

int main(void) {
    char task[MAX_TASK_LENGTH];
    int taskScores[MAX_TASKS][NUM_HIGH_CATEGORIES + NUM_LOW_CATEGORIES + URGENCY_CATEGORY];
    int totalScores[MAX_TASKS];
    char taskNames[MAX_TASKS][MAX_TASK_LENGTH];
    int taskCount = 0;

    const char *highCategories[NUM_HIGH_CATEGORIES] = {
        "Duration", "Difficulty", "Enjoyability", "College and university importance",
        "Positive financial impact", "Negative financial impact", "Current academic importance"
    };

    const char *lowCategories[NUM_LOW_CATEGORIES] = {
        "Hobby related", "Social benefit", "Personal health benefit"
    };

    printf("Welcome to A.T.I.C. (Automatic Task Importance Calculator)\n");
    printf("Type 'DONE' when finished, or 'CANCEL' to cancel a task.\n");

    while (1) {
        printf("\nEnter a task: ");
        fgets(task, MAX_TASK_LENGTH, stdin);

        task[strcspn(task, "\n")] = '\0'; // Remove newline

        if (strcmp(task, "DONE") == 0) {
            break;
        }

        if (taskCount >= MAX_TASKS) {
            printf("Task limit reached. Cannot add more tasks.\n");
            break;
        }

        strncpy(taskNames[taskCount], task, MAX_TASK_LENGTH);

        printf("Score the following for '%s':\n", task);

        int cancelFlag = 0;

        for (int i = 0; i < NUM_HIGH_CATEGORIES; i++) {
            taskScores[taskCount][i] = get_valid_score(highCategories[i], &cancelFlag);
            if (cancelFlag) goto next_task;
        }

        for (int i = 0; i < NUM_LOW_CATEGORIES; i++) {
            taskScores[taskCount][NUM_HIGH_CATEGORIES + i] = get_valid_score(lowCategories[i], &cancelFlag);
            if (cancelFlag) goto next_task;
        }

        taskScores[taskCount][NUM_HIGH_CATEGORIES + NUM_LOW_CATEGORIES] = get_valid_score("Task Urgency", &cancelFlag);
        if (cancelFlag) goto next_task;

        totalScores[taskCount] = calculate_task_score(taskScores[taskCount]);
        taskCount++;

    next_task:
        continue;
    }

    resolve_ties(totalScores, taskNames, taskCount);
    save_to_file(taskNames, totalScores, taskCount);

    return 0;
}
