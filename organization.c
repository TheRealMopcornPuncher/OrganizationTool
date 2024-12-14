#include <stdio.h>
#include <string.h>

#define MAX_TASK_LENGTH 100

int main(void)
{
    char task[MAX_TASK_LENGTH];

    printf("Welcome to A.T.I.C. (Automatic Task Importance Calculator)\n");
    printf("Please answer the following questions to receive final task rankings.\n");
    printf("Type 'DONE' when you are finished.\n");

    while (1)
    {
        printf("Enter a task: ");
        fgets(task, MAX_TASK_LENGTH, stdin);

        // Remove the newline character at the end, if any
        task[strcspn(task, "\n")] = '\0';

        // Check if the user typed "DONE"
        if (strcmp(task, "DONE") == 0)
        {
            break;
        }

        printf("Task entered: %s\n", task);
        // Add further processing for the task here
    }

    printf("All tasks entered. Exiting program.\n");
    return 0;
}
