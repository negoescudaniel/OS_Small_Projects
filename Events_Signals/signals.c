#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

// Basically, the parent process coordinates the execution of 3 child processes
// Every second (implemented with SIGALRM) it stops the current child (SIGSTOP) and starts the next one (SIGCONT) similar to a scheduler
// The children calculate prime numbers and report their progress to the parent via an anonymous pipe
// When a child reaches the target of 300,000 prime numbers, it notifies its parent via a SIGUSR1 signal

#define NUM_CHILDREN 3
#define SEC 1
#define TARGET_PRIMES 300000 // The number of primes that must be found to finish

pid_t pids[NUM_CHILDREN];
int curent = 0;
int pipe_fd[2];

// action when a child sends SIGUSR1, meaning it found the target number of prime numbers
void handle_sigusr1(int sig) {
    int i;
    
    printf("\n-----------------------------\n");
    printf("[Parent] Received the SIGUSR1 signal.\n");
    printf("[Parent] One of the tasks has reached its goal!\n");
    printf("[Parent] Stopping the rest of the tasks and shutting down the system...\n");
    printf("-------------------------------\n");

    // Send the termination signal to all children
    for (i = 0; i < NUM_CHILDREN; i++) {
        kill(pids[i], SIGTERM); 
    }
    
    for (i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL); 
    }
    
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    
    printf("[Parent] System automatically shut down successfully.\n");
    exit(0);
}

// alarm action
void handle_sigalrm(int sig) {
    kill(pids[curent], SIGSTOP);

    curent = (curent + 1) % NUM_CHILDREN;

    kill(pids[curent], SIGCONT);

    alarm(SEC);
}

// Child process logic
void task_fiu(int id) {
    char buffer[150];
    long num = 2;
    long j;
    int is_prime;
    long primes_found = 0;

    // The child only writes to the pipe
    close(pipe_fd[0]); 

    while(1) {
        is_prime = 1;
        
        // Search for prime numbers 
        for (j = 2; j * j <= num; j++) {
            if (num % j == 0) {
                is_prime = 0;
                break;
            }
        }
        
        if (is_prime) {
            primes_found++;
        }

        // Check if we have reached the set target
        if (primes_found > TARGET_PRIMES) {
            snprintf(buffer, sizeof(buffer), "!!! TASK %d FINISHED WORK !!! Found %d primes.", id, TARGET_PRIMES);
            write(pipe_fd[1], buffer, strlen(buffer) + 1);
            
            // send the SIGUSR1 signal to the parent process 
            kill(getppid(), SIGUSR1);
            
            // infinite loop and wait
            while(1) pause(); 
        }

        // Send a progress report to the parent (from time to time)
        if (num % 50000 == 0) {
            snprintf(buffer, sizeof(buffer), "Task %d (PID %d) checking number %ld... Primes found: %ld", id, getpid(), num, primes_found);
            write(pipe_fd[1], buffer, strlen(buffer) + 1);
        }
        
        num++;
    }
}

int main() {
    int i; 
    char read_buffer[150];
    int bytes_read;

    if (pipe(pipe_fd) < 0) {
        perror("Error creating pipe");
        exit(1);
    }

    // set handler in the parent;
    signal(SIGUSR1, handle_sigusr1);

    // child processes 
    for (i = 0; i < NUM_CHILDREN; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("Error calling fork");
            exit(1);
        }
        if (pids[i] == 0) {
            task_fiu(i);
            exit(0); 
        } else {
            /* The parent pauses the child immediately after creation */
            kill(pids[i], SIGSTOP);
        }
    }

    // The parent only reads from the pipe 
    close(pipe_fd[1]); 

    printf("[Parent] System start. %d tasks are competing to find %d primes.\n", NUM_CHILDREN, TARGET_PRIMES);
    printf("[Parent] Time quantum per task: %d seconds.\n\n", SEC);

    signal(SIGALRM, handle_sigalrm);
    
    printf("[Parent] Allocating CPU for Task %d (PID: %d). Resuming...\n", curent, pids[curent]);
    kill(pids[curent], SIGCONT);
    alarm(SEC);

    while(1) {
        bytes_read = read(pipe_fd[0], read_buffer, sizeof(read_buffer));
        if (bytes_read > 0) {
            printf("  -> [Monitor] %s\n", read_buffer);
        }
    }

    return 0;
}