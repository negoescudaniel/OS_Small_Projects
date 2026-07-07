#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

// Structure placed in shared memory 
typedef struct {
    pthread_mutex_t mutex;
    int counter;
    char log[1024];
} SharedResource;

int main() {
    // Variable declaration 
    SharedResource *sr;
    pthread_mutexattr_t attr;
    pid_t pid;
    int i;

    // Create shared memory 
    sr = (SharedResource*) mmap(NULL, sizeof(SharedResource), 
                                 PROT_READ | PROT_WRITE, 
                                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (sr == MAP_FAILED) {
        perror("Error mmap");
        exit(1);
    }

    // Configure Mutex attributes 
    pthread_mutexattr_init(&attr);
    // Set the mutex to be shared between processes
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    
    // Initialize the mutex with the set attributes 
    pthread_mutex_init(&sr->mutex, &attr);
    
    sr->counter = 0;
    strcpy(sr->log, "Log start:\n");

    printf("###Resource initialized. Start processing...\n");

    pid = fork(); // Create child process 

    if (pid < 0) {
        perror("Error fork");
        exit(1);
    }


    for (i = 0; i < 5; i++) {
        // Block access to the resource 
        pthread_mutex_lock(&sr->mutex);
        
        // Both processes modify the same data
        sr->counter++;
        if (pid == 0) {
            strcat(sr->log, " - Modified by CHILD\n");
        } else {
            strcat(sr->log, " - Modified by PARENT\n");
        }
        
        // Simulate a small processing effort
        usleep(100000); 

        // Release the resource for the other process 
        pthread_mutex_unlock(&sr->mutex);
        usleep(50000); 
    }

    if (pid > 0) {
        // Parent process waits for the child to finish 
        wait(NULL); 
        
        printf("\n--- FINAL RESULT ---\n");
        printf("Counter value (expected 10): %d\n", sr->counter);
        printf("Log Content:\n%s", sr->log);

        // Free memory and destroy mutex
        pthread_mutex_destroy(&sr->mutex);
        pthread_mutexattr_destroy(&attr);
        munmap(sr, sizeof(SharedResource));
        printf("\n### Resources released.\n");
    } else {
        // Child process ends here 
        exit(0);
    }

    return 0;
}