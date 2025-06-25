#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>


#define MAX_TASKS 100
#define MAX_COMMAND_LEN 256
#define DELIMITER "█"
#define TASK_FILE "tasks.txt"
#define LOG_FILE "task_logs.txt"
#define ARCHIVE_DIR "archive"

static Task tasks[MAX_TASKS];  
static int task_count = 0;    
static int next_id = 1;


// Add a new task, return task id on success, -1 if full
int add_task(const char *command, int interval_seconds) {
   // Check if there's space left to add task
   if (task_count >= MAX_TASKS) {
        // Returns -1 on failure (no space left)
        return -1;
    }

    // Pointer to next free slot in task array
    Task *t = &tasks[task_count];
    // Assign unique id to task
    t->id = next_id++;  

    // Copy command string
    strncpy(t->command, command, MAX_COMMAND_LEN - 1);
    t->command[MAX_COMMAND_LEN - 1] = '\0';

    // Set interval
    t->interval_seconds = interval_seconds;
    // Initialize last_run (task hasn't run yet)
    t->last_run = 0;
    // Mark task as active
    t->active = true;

    // Increase count of stored tasks
    task_count++;

    // Return new task's id
    return t->id;
}


// Display all tasks added
void display_task() {
    if (task_count != 0) {
        printf("\nAll tasks:\n");
        // Loop through all stored tasks & display formatted info
        for(int i = 0; i < task_count; i++) {
            printf("\n=============================================================\n");
            printf("Task ID:   %d\n", tasks[i].id);
            printf("-------------------------------------------------------------\n");
            printf("Command:   %s\n", tasks[i].command);
            printf("-------------------------------------------------------------\n");
            printf("Interval:  Every %d seconds\n", tasks[i].interval_seconds);
            printf("-------------------------------------------------------------\n");
            if (tasks[i].last_run == 0) {
                printf("Last run:  Never\n");
            } else {
                printf("Last run:  %s", ctime(&(tasks[i].last_run)));
            }
            printf("-------------------------------------------------------------\n");
            if (tasks[i].active){
                printf("Status:    Enabled (will run when scheduler runs)\n");
            } else {
                printf("Status:   Paused\n");
            }
            printf("=============================================================\n\n");
        }
    } else {
        printf("\nNo tasks currently available. Run '/flux add \"<command>\" <interval_in_seconds>' to add task.\n\n");
    }
}


// Continuously loop in the background and run tasks
int scheduler() {
    // Load tasks beforehand
    load_tasks();

    // No tasks to run
    if (task_count == 0) {
        return 1;
    }

    // Create a "running" flag file to indicate scheduler is running
    FILE *flag = fopen("scheduler.running", "w");
    if (flag != NULL) {
        fclose(flag);
    }


    // Loop forever
    while (true) {
        // Check if shceduler isn't running 
        if (!file_exists("scheduler.running")) {
            printf("\nScheduler has been stopped\n");
            break;
        }

        // Load the most recent changes from file
        load_tasks(); 

        // Current time & if tasks has changed
        time_t current_time = time(NULL);
        bool changed = false;

        for (int i = 0; i < task_count; i++) {
            // If task is paused, skip
            if (!tasks[i].active) {
                continue;
            }

            // Else if its time to run next task or it hasn't run before, run the command
            if (tasks[i].last_run == 0 || 
                (current_time - tasks[i].last_run) >= tasks[i].interval_seconds) {

                system(tasks[i].command);

                // Log the command
                FILE *txt = fopen(LOG_FILE, "a");

                // Format timestamp
                char timebuf[64];
                strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&current_time));


                // Log timestamp, task id, & the command that was run
                fprintf(txt, "[%s] Ran task #%d: %s\n", timebuf, tasks[i].id, tasks[i].command);
                // Close the file
                fclose(txt);


                tasks[i].last_run = current_time;
                changed = true;
            }
        }

        if (changed) {
            for (int i = 0; i < task_count; i++) {
                if (tasks[i].last_run == current_time) {
                    update_last_run(tasks[i].id, tasks[i].last_run);
                }
            }
        }


        // Sleep to be more effecient
        sleep(1);
}

    return 0;
}


// Delete task of specific id
int delete_task(int given_id) {
    // Loop through tasks array
    for (int i = 0; i < task_count; i++) {
        // Check if selected task is the one user wants to delete
        if (tasks[i].id == given_id) {
            for (int j = i; j < task_count - 1; j++) {
                // Delete task & shift other tasks left to keep array compact
                tasks[j] = tasks[j + 1];
            }

            // Update task_count to reflect deletion of 1 task
                task_count--;

            // Return 0 if task is found & successfully deleted
            return 0;
        }
    }

    // Return 1 if no task with given ID was found
    return 1;
}


// Pause tasks when desired
int pause_task(int given_id) {
for (int i = 0; i < task_count; i++) {
        // Check if selected task is the one user wants to pause
        if (tasks[i].id == given_id) {
            if (tasks[i].active == false) {
                // Return -1 to indicate task found but is already paused
                return -1;
            }
            // Set active state of task to false
            tasks[i].active = false;

            // Return 0 if task is found & successfully paused
            return 0;
        }
    }

    // Return 1 if no task with given ID was found
    return 1;
}

// Resume tasks when desired
int resume_task(int given_id) {
for (int i = 0; i < task_count; i++) {
        // Check if selected task is the one user wants to resume
        if (tasks[i].id == given_id) {
            if (tasks[i].active == true) {
                // Return -1 to indicate task found but already active
                return -1;
            }
            // Set active state of task to true
            tasks[i].active = true;

            // Return 0 if task is found & successfully resume
            return 0;
        }
    }

    // Return 1 if no task with given ID was found
    return 1;
}


// Save tasks to allow for persistent storage when user quits (write to the .txt file)
void save_tasks() {
    // Open the file for writing (if exists, will be overwritten, if not, will be created) and create a pointer to it
    FILE *txt = fopen(TASK_FILE, "w");

    // Check if the file opened successfully
    if (!txt) {
        fprintf(stderr, "Unable to save tasks to tasks.txt. Changes won't be saved.\n");
        perror("Failed to open tasks file");
        return;
    }

    // Loop through task array
    for (int i = 0; i < task_count; i++) {
        // Write all relevant task fields to file in single line using █ as delimiter
        fprintf(txt, "%d█%s█%d█%ld█%d\n", tasks[i].id, tasks[i].command, tasks[i].interval_seconds, tasks[i].last_run, tasks[i].active);
    }

    // Close the file
    fclose(txt);
}


// Loads tasks to allow them to be used when user returns (read the .txt file)
void load_tasks() {
    // Buffer to store each line (max length of each line)
    char buffer[1024]; 

    // Open the file for reading and create a pointer to it
    FILE *txt = fopen(TASK_FILE, "r");

    // Check if the file opened successfully / tasks exist
    if (!txt) {
        fprintf(stderr, "No tasks saved yet.\n");
        perror("Failed to open task file");
        return;
    }

    // Clear current tasks in memory
    task_count = 0;

    // Read line by line until end is reached & process the line stored in buffer
    while (fgets(buffer, sizeof(buffer), txt) != NULL) {
        // Remove newline char
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Parse fields by delimiter
        char *token = strtok(buffer, DELIMITER);
        if (token == NULL) continue;
        // Set id
        int id = atoi(token);

        token = strtok(NULL, DELIMITER);
        if (token == NULL) continue;
        // Set command
        char command[MAX_COMMAND_LEN];
        strncpy(command, token, MAX_COMMAND_LEN - 1);
        command[MAX_COMMAND_LEN - 1] = '\0';

        token = strtok(NULL, DELIMITER);
        if (token == NULL) continue;
        // Set interval
        int interval_seconds = atoi(token);

        token = strtok(NULL, DELIMITER);
        if (token == NULL) continue;
        // Set last run
        time_t last_run = (time_t)atol(token);

        token = strtok(NULL, DELIMITER);
        if (token == NULL) continue;
        // Set active state
        bool active = (atoi(token) != 0);

        // Check if there is space for task
        if (task_count >= MAX_TASKS) {
            printf("Warning: Maximum task limit reached. Some tasks were not loaded.\n");
            break;
        }

        // Store task in array
        tasks[task_count].id = id;
        strncpy(tasks[task_count].command, command, MAX_COMMAND_LEN - 1);
        tasks[task_count].command[MAX_COMMAND_LEN - 1] = '\0';
        tasks[task_count].interval_seconds = interval_seconds;
        tasks[task_count].last_run = last_run;
        tasks[task_count].active = active;

        task_count++;

    }

    // Close the file
    fclose(txt);

    // After loading all tasks, update next_id to avoid ID conflicts
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].id >= next_id) {
            next_id = tasks[i].id + 1;
        }
    }


}

// Check if a stop file exists
bool file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// Check if command added exists
bool command_exists(const char *command) {
    // Extract the first word from command (the executable)
    char cmd_copy[256];
    strncpy(cmd_copy, command, sizeof(cmd_copy)-1);
    cmd_copy[sizeof(cmd_copy)-1] = '\0';

    char *first_word = strtok(cmd_copy, " ");
    if (!first_word) {
        return false;
    }

    // Build shell command to check existence
    char check_cmd[300];
    snprintf(check_cmd, sizeof(check_cmd), "command -v %s >/dev/null 2>&1", first_word);

    // Run the command
    int ret = system(check_cmd);

    // system() returns 0 on success (command found)
    return (ret == 0);
}

// Update last_run property w/o rewriting whole task
void update_last_run(int task_id, time_t new_last_run) {
    // Open the original tasks file for reading
    FILE *in = fopen(TASK_FILE, "r");
    // Exit the func if file can't be opened
    if (!in) return;

    // Open a temp file for writing updated tasks
    FILE *out = fopen("tasks_temp.txt", "w");
    // Check if output file can't be opened
    if (!out) {
        // Close the file & exit the func
        fclose(in);
        return;
    }

    // Create a buffer to hold each line read
    char buffer[1024];

    // Read file line by line
    while (fgets(buffer, sizeof(buffer), in)) {
        // Skip blank lines
        if (buffer[0] == '\n' || buffer[0] == '\0') {
            continue;
        }

        // Make a working copy of line
        char line[1024];
        // Copy the buffer into the line
        strncpy(line, buffer, sizeof(line) - 1);  
        line[sizeof(line) - 1] = '\0';

        // Array to store toekns from line
        char *tokens[5];
        // Index for token array
        int i = 0;

        // Tokenize line by the delimiter
        char *token = strtok(line, "█");
        while (token && i < 5) {
            tokens[i++] = token;
            token = strtok(NULL, "█");
        }

        // Check if line has exactly 5 tokens & if task ID matched
        if (i == 5 && atoi(tokens[0]) == task_id) {
            // Format buffer line w/ updated last_run time
            snprintf(buffer, sizeof(buffer), "%s█%s█%s█%ld█%s\n",
                    tokens[0], tokens[1], tokens[2],
                    new_last_run, tokens[4]);
        }

        // Write the line if it has enough tokens
        if (i >= 4) {
            fputs(buffer, out);
        }
    }

    // Close the input & output files
    fclose(in);
    fclose(out);

    // Replace old file with updated temp file
    rename("tasks_temp.txt", TASK_FILE);
}


// Display history from logs
void display_history() {
    // Open log file
    FILE *txt = fopen(LOG_FILE, "r");

    // If txt is NULL, it doesn't exist or can't be opened
    if (!txt) {
        printf("No history found. Failed to open log file.\n");
        perror("System error");
        return;
    }

    // Use a buffer
    char line[1024];

    // Track if file is empty
    bool empty = true;

    // Print header
    printf("\n=== Task Run History ===\n\n");

    // Read & print each line
    while (fgets(line, sizeof(line), txt)) {
        empty = false;
        printf("%s", line);
    }
    
    if (empty) {
        printf("No task runs recorded yet.\n");
    }

    // Close file
    fclose(txt);
}


// Display history from logs for specifc task id
void display_history_filtered(int task_id) {
    // Open log file
    FILE *txt = fopen(LOG_FILE, "r");

    // If txt is NULL, it doesn't exist or can't be opened
    if (!txt) {
        printf("No history found. Failed to open log file.\n");
        perror("System error");
        return;
    }

    // Use a buffer
    char line[1024];

    // Track if file is empty
    bool empty = true;

    // Print header
    printf("\n=== Task Run History: ID = %d ===\n\n", task_id);

    // Read & print each line
    while (fgets(line, sizeof(line), txt)) {
        char *pos = strstr(line, "Ran task #");
        if (pos != NULL) {
            // Move pointer to after "Ran task #"
            pos += strlen("Ran task #");

            // Parse integer task id from pos
            int id_in_line = atoi(pos);

            if (id_in_line == task_id) {
                empty = false;
                printf("%s", line);
            }
        }

    }
    
    if (empty) {
        printf("No task runs recorded yet.\n");
    }

    // Close file
    fclose(txt);
}


// Archive log file to ensure it doesn't get too large
int archive_logs() {
    // Open log file
    FILE *txt = fopen(LOG_FILE, "r");

    // Check if file exists
    if (!txt) {
        printf("Log file doesn't exist or cannot be opened.\n");
        perror("System error");
        return 1;

    } else {
        fseek(txt, 0, SEEK_END);
        long size = ftell(txt);
        if (size == 0) {
            printf("Log is empty. No tasks have run yet.");
            return 1;
        }
    }
    fclose(txt);

    // Check if archive directory exists
    struct stat st = {0};
    if (stat(ARCHIVE_DIR, &st) == -1) {
        // Create archive directory
        if (mkdir(ARCHIVE_DIR, 0755) != 0) {
            perror("Failed to create archive directory");
            return 1;
        }
    }

    // Create timestamped archive file name
    time_t current_time = time(NULL);
    struct tm *tm_info = localtime(&current_time);
    char archive_filename[256];
    strftime(archive_filename, sizeof(archive_filename), ARCHIVE_DIR "/tasks_%Y-%m-%d_%H-%M-%S.log", tm_info);

    // Move current log file to archive file
    if (rename(LOG_FILE, archive_filename) != 0) {
        perror("Failed to archive log file");
        return 1;
    }
    
    // Create new empty log to allow that process to continue
    FILE *new_log = fopen(LOG_FILE, "w");
    if (!new_log) {
        printf("Failed to create new log file\n");
        perror("System error");
        return 1;
    }
    fclose(new_log);

    // Confirm proccess has succeeded
    printf("Logs archived successfully to '%s'\n", archive_filename);

    return 0;
}


void print_usage() {
    printf("\nUsage: ./flux <command> [options]\n\n");
    printf("Available commands:\n");
    printf("  add \"<command>\" <interval>   Add a new task\n");
    printf("  list                         List all tasks\n");
    printf("  delete <id>                  Delete a task by ID\n");
    printf("  pause <id>                   Pause a task\n");
    printf("  resume <id>                  Resume a task\n");
    printf("  start                        Start the scheduler (run enabled tasks)\n");
    printf("  stop                         Stop the scheduler (stop all tasks)\n");
    printf("  status                       Show status of the scheduler\n");
    printf("  history [id]                 Show run history (optionally filtered by ID)\n");
    printf("  archive                      Archive the log file\n");
    printf("  help                         Show this message\n\n");
}