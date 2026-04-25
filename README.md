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


## 🔹 QNX Message Passing – Parent-Child Encryption

In the `messages` directory, I implemented a client-server style communication model between a parent and a child process utilizing QNX channels (`simple_messages.c`).

### Key Features:

* The parent process creates a local communication channel and waits in a blocked state for incoming requests.
* The child process establishes a connection to the parent's channel, reads user input, and transmits it using a blocking `MsgSend` function.
* The parent receives the payload, processes it by applying a basic encryption algorithm (shifting character ASCII values), and returns the encrypted text via `MsgReply`, which unblocks the child.
* Demonstrates synchronous, tightly coupled inter-process communication native to the QNX Neutrino RTOS architecture.


## 🔹 POSIX Message Queues – QoS Traffic Priority Simulator

In the `message_queues` directory, I implemented a Quality of Service (QoS) network traffic prioritization system using POSIX message queues with two independent programs (`expeditor.c` and `router.c`).

### Key Features:

* Communication is handled via a shared POSIX message queue (`/coada_qos`) bridging a traffic generator and a routing simulator.
* The `expeditor` process acts as a user interface to generate network traffic, assigning strict OS-level priorities to different data types (e.g., Priority 5 for time-sensitive VOIP traffic, Priority 2 for standard DOWNLOADs).
* The `router` process continuously polls the queue and extracts messages strictly based on priority rather than chronological order (FIFO), ensuring critical packets are processed first.
* Includes a built-in `SHUTDOWN` command that allows the sender to safely terminate the router process and successfully clean up the queue from memory.

## Motivation

This repository was created as part of my learning process in operating systems and embedded software, with a focus on practical implementations that reflect real-world system behavior.
