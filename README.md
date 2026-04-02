# Operating Systems Concepts Showcase

This repository showcases different core functionalities of operating systems, with a focus on inter-process communication (IPC), process management, and basic security concepts.

All projects were developed and tested on a virtual machine running the QNX operating system.

---


## 🔹 Anonymous Pipe – Mini Shell

In the `Anonymous_Pipe` directory, I implemented a mini shell using anonymous pipes.

### Key Features:

* The parent process reads user commands from standard input
* Commands are sent to a child process via an anonymous pipe
* The child process parses the command and executes it using `execvp()`
* The shell runs in a loop and supports an `exit` command


## 🔹 Named Pipe (FIFO) – Secure Communication

In the `Named_Pipe/FIFO` directory, I implemented a communication system between two independent processes using named pipes (FIFOs).

### Key Features:

* Two separate processes (Alice and Bob) communicate using two FIFO channels
* A Diffie–Hellman key exchange is simulated to establish a shared secret
* After the key is established, messages are encrypted using a simple symmetric encryption method (XOR)
* The system allows bidirectional, secure message exchange


## 🔹 Events & Signals – Round-Robin Process Scheduler

In the `Events_Signals` directory, I implemented a custom process scheduler where a parent process coordinates the execution of multiple child processes using UNIX signals.

### Key Features:

* The parent process manages 3 child processes using a Round-Robin scheduling approach with a 1-second time quantum
* Context switching is handled via `SIGALRM`, pausing the current task with `SIGSTOP` and resuming the next with `SIGCONT`
* Child processes perform CPU-intensive tasks (calculating prime numbers) and stream progress updates to the parent via an anonymous pipe
* Reaching a computation target triggers a `SIGUSR1` signal, causing the parent to perform a graceful system shutdown using `SIGTERM`


## Motivation

This repository was created as part of my learning process in operating systems and embedded software, with a focus on practical implementations that reflect real-world system behavior.
