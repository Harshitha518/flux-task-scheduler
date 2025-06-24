# Flux - Simple Task Scheduler CLI

Flux is a lightweight command-line task scheduler that lets you add, run, pause, resume, and delete recurring tasks easily. It runs tasks at specified intervals in the background and supports basic task management.

---

## Features

- Add commands to run repeatedly on a custom interval  
- Run scheduler in the background  
- Pause and resume individual tasks without stopping others  
- View all scheduled tasks with status and last run time  
- Stop scheduler gracefully  
- Persistent task storage between runs  

---

## Commands

| Command                 | Description                              | Usage Example                                    |
|-------------------------|------------------------------------------|-------------------------------------------------|
| `add <command> <interval>` | Add a new task to run every `<interval>` seconds | `./flux add "./backup.sh" 60`       |
| `list`                  | Show all tasks with details              | `./flux list`                                    |
| `start`                 | Start the scheduler (runs in background) | `./flux start`                                   |
| `stop`                  | Stop the running scheduler               | `./flux stop`                                    |
| `pause <task_id>`       | Pause a specific task by its ID          | `./flux pause 2`                                 |
| `resume <task_id>`      | Resume a paused task by its ID            | `./flux resume 2`                                |
| `delete <task_id>`      | Delete a task by its ID                   | `./flux delete 1`                                |
| `help`                  | Show usage instructions                   | `./flux help`                                    |

---

## Sample Use Cases

- Remind yourself to stretch or take breaks with desktop notifications  
- Run cleanup scripts periodically (e.g., clear temp files)  
- Ping services or websites on a schedule for uptime checks  
- Execute any shell command repeatedly without manual intervention  

---

## Getting Started

1. Compile the program using `gcc` or your preferred C compiler  
2. Use `./flux add "<command>" <interval_in_seconds>` to add tasks  
3. Start the scheduler with `./flux start`  
4. Manage tasks using `pause`, `resume`, `delete`, and view them with `list`  

---

## Notes

- Scheduler runs in the background, allowing you to continue using your terminal  
- Tasks persist across program runs using a simple text file  
- Commands run exactly as typed, so ensure validity and security of commands added  

---

Built for simplicity and power, schedule, monitor, and manage tasks with ease.

---

**License:** MIT License  
