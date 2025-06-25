# Flux - Simple Task Scheduler CLI

Flux is a lightweight command-line task scheduler that lets you add, run, pause, resume, and delete recurring tasks easily. It runs tasks at specified intervals in the background and supports basic task management.

---

## Features & Commands

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

## Sample Use Cases

- Remind yourself to stretch or take breaks with desktop notifications  
- Run cleanup scripts periodically (e.g., clear temp files)  
- Ping services or websites on a schedule for uptime checks  
- Execute any shell command repeatedly without manual intervention  

---

## Getting Started

1. Compile the program using `make`  
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
