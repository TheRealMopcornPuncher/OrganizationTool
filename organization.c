#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_TASK_LENGTH 100
#define MAX_TASKS 10
#define NUM_HIGH_CATEGORIES 7
#define NUM_LOW_CATEGORIES 3

// Function to get a validated score between 1 and 5 or handle "CANCEL"
int get_valid_score(const char *category, int *cancelFlag) {
    int score;
    char input[10];

    while (1) {
        printf("%s (1-5, or type 'CANCEL' to cancel this task): ", category);
        fgets(input, sizeof(input), stdin);

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        // Check if the input is "CANCEL"
        if (strcmp(input, "CANCEL") == 0) {
            *cancelFlag = 1; // Set cancel flag
            return -1;       // Return invalid score
        }

        // Check if input is a valid number between 1 and 5
        if (isdigit(input[0]) && (score = atoi(input)) >= 1 && score <= 5) {
            return score;
        } else {
            printf("Invalid input. Please enter a number between 1 and 5.\n");
        }
    }
}

int main(void) {
    char task[MAX_TASK_LENGTH];
    int taskScores[MAX_TASKS][NUM_HIGH_CATEGORIES + NUM_LOW_CATEGORIES]; // Stores scores for each task
    char taskNames[MAX_TASKS][MAX_TASK_LENGTH]; // Stores task names
    int taskCount = 0;

    // High-priority categories
    const char *highCategories[NUM_HIGH_CATEGORIES] = {
        "Duration", "Difficulty", "Enjoyability", "College and university importance",
        "Positive financial impact", "Negative financial impact", "Current academic importance"
    };

    // Low-priority categories
    const char *lowCategories[NUM_LOW_CATEGORIES] = {
        "Hobby related", "Social benefit", "Personal health benefit"
    };

    printf("Welcome to A.T.I.C. (Automatic Task Importance Calculator)\n");
    printf("Please answer the following questions to receive final task rankings.\n");
    printf("Type 'DONE' when you are finished, type 'CANCEL' to cancel the scoring for a task.\n");

    while (1) {
        printf("\nEnter a task: ");
        fgets(task, MAX_TASK_LENGTH, stdin);

        // Remove the newline character
        task[strcspn(task, "\n")] = '\0';

        // Check if the user typed "DONE"
        if (strcmp(task, "DONE") == 0) {
            break;
        }

        if (taskCount >= MAX_TASKS) {
            printf("Task limit reached. Cannot add more tasks.\n");
            break;
        }

        // Store the task name temporarily
        strncpy(taskNames[taskCount], task, MAX_TASK_LENGTH);

        printf("Score the following for '%s' (1-5, or type 'CANCEL' to cancel):\n", task);

        int cancelFlag = 0;

        // Get scores for high-priority categories
        for (int i = 0; i < NUM_HIGH_CATEGORIES; i++) {
            int score = get_valid_score(highCategories[i], &cancelFlag);
            if (cancelFlag) {
                printf("Scoring for '%s' canceled.\n", task);
                cancelFlag = 0;
                goto next_task; // Skip storing and continue with the next task
            }
            taskScores[taskCount][i] = 2 * score;
        }

        // Get scores for low-priority categories
        for (int i = 0; i < NUM_LOW_CATEGORIES; i++) {
            int score = get_valid_score(lowCategories[i], &cancelFlag);
            if (cancelFlag) {
                printf("Scoring for '%s' canceled.\n", task);
                cancelFlag = 0;
                goto next_task; // Skip storing and continue with the next task
            }
            taskScores[taskCount][NUM_HIGH_CATEGORIES + i] = score;
        }

        taskCount++; // Task completed successfully

    next_task:
        continue; // Move to the next task
    }

    // Display all tasks and their scores
    printf("\nAll tasks entered. Here are the scores:\n");
    for (int i = 0; i < taskCount; i++) {
        printf("\nTask: %s\n", taskNames[i]);
        printf("Scores:\n");

        for (int j = 0; j < NUM_HIGH_CATEGORIES; j++) {
            printf("  %s: %d\n", highCategories[j], taskScores[i][j]);
        }

        for (int j = 0; j < NUM_LOW_CATEGORIES; j++) {
            printf("  %s: %d\n", lowCategories[j], taskScores[i][NUM_HIGH_CATEGORIES + j]);
        }
    }

    printf("\nExiting program.\n");
    return 0;
}
