#include <time.h>
#include <stdbool.h>  

#define MAX_COMMAND_LEN 256

// Task struct
typedef struct {
    int id;
    char command[MAX_COMMAND_LEN];
    int interval_seconds;
    time_t last_run;
    bool active;
} Task;

// Function declarations (prototypes)
int add_task(const char *command, int interval_seconds);

void display_task();

int scheduler();

int delete_task(int given_id);

int pause_task(int given_id);

int resume_task(int given_id);

void save_tasks();

void load_tasks();

void print_usage();

bool file_exists(const char *filename);

bool command_exists(const char *command);

void update_last_run(int task_id, time_t new_last_run);

void display_history();

void display_history_filtered(int task_id);

int archive_logs();