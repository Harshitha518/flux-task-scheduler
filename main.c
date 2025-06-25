#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "task.h"


int main(int argc, char *argv[]) {
    // Load tasks from memory
    load_tasks();
   
    // Check if 2 arguments are passed in
    if (argc < 2) {
        // Return usage error
        print_usage();
        return 1;
    }


    // ./flux list
    if (strcmp(argv[1], "list") == 0) {
        if (argc != 2) {
            printf("Usage: ./flux list\n");
            return 1;
        }

        load_tasks();

        display_task();
        return 0;
    }


    // ./flux start
    else if (strcmp(argv[1], "start") == 0) {
        if (argc != 2) {
            printf("Usage: ./flux start\n");
            return 1;
        }
        

       pid_t pid = fork();

        if (pid < 0) {
            perror("Failed to start scheduler");
            return 1;
        } else if (pid == 0) {
            load_tasks();
            int indicator = scheduler();
            exit(indicator);
        } else {
         
            printf("Scheduler started in background (PID %d).\n", pid);
            return 0;
        }

    }

    // ./flux status
    else if (strcmp(argv[1], "status") == 0) {
        if (argc != 2) {
            printf("Usage: ./flux status\n");
            return 1;
        }

        if (file_exists("scheduler.running")) {
            printf("Scheduler is currently running in the background.\n");
        } else {
            printf("Scheduler is not running.\n");
        }
        return 0;
    }

    else if (strcmp(argv[1], "stop") == 0) {
        if (argc != 2) {
            printf("Usage: ./flux stop\n");
            return 1;
        }

        if (remove("scheduler.running") == 0) {
            printf("Stopping scheduler...\n");
        } else {
            printf("Scheduler is not running.\n");
        }
        return 0;
    }

    // ./flux add
    else if (strcmp(argv[1], "add") == 0) {
        if (argc != 4) {
            printf("Usage: ./flux add \"<command>\" <interval_in_seconds>\n");
            return 1;
        }

        char *command = argv[2];
        int interval = atoi(argv[3]);

        if (!command_exists(command)) {
            printf("Command '%s' not found on system. Task could not be added.\n", command);
            return 1;
        }


        if (interval <= 0) {
            printf("Interval must be a positive number.\n");
            return 1;
        }

        int indicator = add_task(command, interval);

        if (indicator == -1) {
            printf("Task could not be added. Too many tasks. Delete existing tasks to continue.\n");
            return 1;
        } else {
            save_tasks();
            printf("\n\nTask of '%s' with ID of %d has been added.\n\n", command, indicator);
            printf("Recently added task will occur every %d seconds. Run scheduler to begin.\n\n\n", interval);
        }

    }

    // ./flux delete
    else if (strcmp(argv[1], "delete") == 0) {
        if (argc != 3) {
            printf("Usage: ./flux delete <task_id>\n");
            return 1;
        }
        
        load_tasks(); 
        int task_id = atoi(argv[2]);

        int indicator = delete_task(task_id);

        if (indicator == 0) {
            save_tasks();
            printf("Task with ID %d has been deleted successfully.\n", task_id);
        } else {
            printf("Could not delete task. No task found with ID of %d.\n", task_id);
        }

    }

    // ./flux pause
    else if (strcmp(argv[1], "pause") == 0) {
        if (argc != 3) {
            printf("Usage: ./flux pause <task_id>\n");
            return 1;
        }

        load_tasks(); 
        int task_id = atoi(argv[2]);

        int indicator = pause_task(task_id);

        if (indicator == 0) {
            save_tasks();
            printf("Task with ID %d has been paused successfully.\n", task_id);
        } else if (indicator == -1) {
            printf("Task with ID of %d is already paused.\n", task_id);
        } else {
            printf("Could not pause task. No task found with ID of %d.\n", task_id);
        }
    }

    // ./flux resume
    else if (strcmp(argv[1], "resume") == 0) {
        if (argc != 3) {
            printf("Usage: ./flux resume <task_id>\n");
            return 1;
        }

        load_tasks(); 
        int task_id = atoi(argv[2]);

        int indicator = resume_task(task_id);

        if (indicator == 0) {
            save_tasks();
            printf("Task with ID %d has resumed.\n", task_id);
        } else if (indicator == -1) {
            printf("Task with ID of %d is already active.\n", task_id);
        } else {
            printf("Could not resume task. No task found with ID of %d.\n", task_id);
        }
    }

    else if (strcmp(argv[1], "history") == 0) {
        // .flux history
        if (argc == 2) {
            display_history();
        } 
        // .flux history <id>
        else if (argc == 3) {
            int task_id = atoi(argv[2]);

            if (task_id <= 0) {
                printf("Invalid task ID provided.\n");
                return 1;
            }

            display_history_filtered(task_id);
        } else {
            printf("Invalid arguments for history command.\n");
            return 1;
        }
        return 0;
    }

    // ./flux archive
    else if (strcmp(argv[1], "archive") == 0) {
        archive_logs();
    }

    // ./flux help
    else if (strcmp(argv[1], "help") == 0) {
        print_usage();
        return 0;
    }

    // Check if arguments passed match none of the above
    else {
        printf("Unknown command: %s\n", argv[1]);
        print_usage();
        return 1;
    }

}
