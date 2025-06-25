# Flux - Lightweight CLI Task Scheduler  
#### Video Demo: [https://youtu.be/YOUR_VIDEO_LINK](https://youtu.be/YOUR_VIDEO_LINK)  
#### Description:

Flux is a lightweight, command-line task scheduler written in C that enables users to automate recurring tasks on a customizable interval. Inspired by the idea of creating a simplified alternative to `cron`, Flux was designed to be beginner-friendly and easily configurable.

With Flux, users can add commands to run every n seconds, pause or resume specific tasks, view task history, and stop or restart the background scheduler, all directly from the terminal. The system keeps tasks persistent between runs and logs their output for easy debugging and review.

### Key Features

- Add and run any terminal command at a specified interval
- Pause, resume, and delete tasks using task IDs
- Run a background scheduler process via `fork()`
- View active tasks with their status and last run time
- Track execution history of all or individual tasks
- Archive old logs to keep files clean
- Store tasks persistently in a text file
- Helpful error handling and edge case detection

### Example Use Cases

- Send recurring hydration reminders (ex. every 30 minutes)
- Run GitHub auto-pull every few minutes to keep a local repo synced
- Schedule Python or shell scripts like backups or maintenance
- Periodically clean up temp files or monitor running processes

---

## File Overview

- **`main.c`**: The main CLI interface and command parser. This handles user input, parses commands, and calls functions from `task.c` in response.
- **`task.c`**: Contains the core logic for task creation, deletion, pausing, resuming, and execution. This file defines how each task is stored, interpreted, and run on schedule.
- **`task.h`**: Header file declaring structs and functions used by `task.c` and `main.c`.
- **`tasks.txt`**: Stores active tasks persistently between runs. Each task is saved with its ID, interval, command, and status (active/paused).
- **`task_logs.txt`**: Stores the full history of task executions including timestamp, command, and ID.
- **`Makefile`**: Simplifies compilation using `make`. Also includes a `clean` option to remove build artifacts.
- **`.gitignore`**: Excludes build files, logs, and local environment folders from the repo (e.g., `.DS_Store`, `*.o`, etc.).
- **`README.md`**: This file!

---

## How It Works

The scheduler uses `fork()` to run in the background and manages tasks in a simple event loop. Each task's last run timestamp is compared to the current time to determine if it's ready to execute again. If so, it uses `system()` to run the stored command and logs the result. Each task runs independently based on its interval, and paused tasks are skipped.

I created a custom file format to store task metadata (`tasks.txt`), ensuring persistence between sessions. The scheduler runs quietly in the background, logging all activity into `task_logs.txt`. You can view this history and archive it for long-term use.

---

## Why I Built It

I wanted to something useful yet technically educational. Through CS50x, I learned about file processing, memory safety, command-line tools, and many other useful skills. I also wanted to build something in C, as this language quickly became one of my favorites. Flux was a way for me to bring all of that together in one project that could be expanded in real-life workflows, especially for developers.

---

## Design Choices

- **Task Identification**: Each task has a unique integer ID to make referencing simpler. This also makes pausing, resuming, and deleting clear and easy.
- **Text File Persistence**: Rather than using databases, I chose plain text file for both simplicity and visibility, allowing non-technical users to easily use this tool. This made debugging and verification more straightforward as well.
- **Logging**: I implemented detailed logs to show how the tool works and provide transparency into each execution.
- **Modularity**: I split core logic into `task.c` and `main.c` to separate scheduling logic from command parsing.

---


## 💻 Commands & Usage

| Command                       | Description                                                | Example                                                                 |
|------------------------------|------------------------------------------------------------|-------------------------------------------------------------------------|
| `./flux add "<cmd>" <int>`   | Add a new recurring task with interval in seconds          | `./flux add "echo 'Hello'" 30`                                         |
| `./flux start`               | Start the task scheduler in the background                 | `./flux start`                                                         |
| `./flux list`                | Show all tasks with ID, command, interval, status, etc.    | `./flux list`                                                          |
| `./flux pause <task_id>`     | Pause a running task by ID                                 | `./flux pause 2`                                                       |
| `./flux resume <task_id>`    | Resume a paused task by ID                                 | `./flux resume 2`                                                      |
| `./flux delete <task_id>`    | Delete a task completely by ID                             | `./flux delete 1`                                                      |
| `./flux status`              | Check if the scheduler is currently running                | `./flux status`                                                        |
| `./flux stop`                | Gracefully stop the running scheduler                      | `./flux stop`                                                          |
| `./flux history`             | View all past logs of tasks with timestamps                | `./flux history`                                                       |
| `./flux history <task_id>`   | View logs specific to one task                             | `./flux history 2`                                                     |
| `./flux archive`             | Archive the log file to start fresh logs                   | `./flux archive`                                                       |
| `./flux help`                | Show usage instructions                                    

---

**License:** MIT License  
