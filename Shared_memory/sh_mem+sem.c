#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <errno.h>

#define SHM_NAME "/crypto_shm_anon"

// Define the structure that will be placed in the shared memory
// It contains both the data and the anonymous semaphores
typedef struct {
    sem_t sem_parent_child;   
    sem_t sem_child_parent;   
    char initial_message[256];
    char encrypted_message[256];
    char encryption_key;
} CryptoData;

// Simple XOR encryption function
void xor_encrypt(const char *input, char *output, char key) {
    int i = 0;
    while (input[i] != '\0') {
        output[i] = input[i] ^ key; 
        i++;
    }
    output[i] = '\0';
}

int main() {
    int fd;
    CryptoData *shm_data;
    pid_t pid;
    int i; 

    // Open shared memory object
    fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0777);
    if (fd == -1) {
        perror("Error creating shared memory");
        exit(EXIT_FAILURE);
    }

    // Configure the size of the shared memory object
    if (ftruncate(fd, sizeof(CryptoData)) == -1) {
        perror("Error sizing shared memory");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory object into the process address space
    shm_data = mmap(0, sizeof(CryptoData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_data == MAP_FAILED) {
        perror("Error mapping shared memory");
        exit(EXIT_FAILURE);
    }

    // Initialize anonymous semaphores (second argument '1' means shared between processes)
    sem_init(&shm_data->sem_parent_child, 1, 0); 
    sem_init(&shm_data->sem_child_parent, 1, 0);

    system("clear");
    printf("--- Application Start ---\n\n");

    // Create a new process
    pid = fork();

    if (pid < 0) {
        perror("Error on fork");
        exit(EXIT_FAILURE);
    } 
    else if (pid > 0) {
        // PARENT PROCESS 
        
        strcpy(shm_data->initial_message, "TOP SECRET: The project has been approved.");
        shm_data->encryption_key = 'K'; 
        
        printf("[PARENT] I wrote the initial message: '%s'\n", shm_data->initial_message);
        printf("[PARENT] I set the encryption key: '%c'\n", shm_data->encryption_key);
        
        // Signal the child process that data is ready
        sem_post(&shm_data->sem_parent_child);
        
        printf("[PARENT] Waiting for the child to encrypt the data...\n");
        
        // Wait for the child process to finish encryption
        sem_wait(&shm_data->sem_child_parent);
        
        printf("\n[PARENT] Child has finished! Encrypted message obtained from shared memory:\n");
        
        // Print the encrypted message in Hexadecimal format
        for (i = 0; i < strlen(shm_data->initial_message); i++) {
            printf("%02X ", (unsigned char)shm_data->encrypted_message[i]);
        }
        printf("\n");
        
        // Wait for child process to clean up and exit
        wait(NULL);
        
        // Destroy semaphores
        sem_destroy(&shm_data->sem_parent_child);
        sem_destroy(&shm_data->sem_child_parent);
        
        // Unmap and close shared memory
        munmap(shm_data, sizeof(CryptoData));
        close(fd);              
        shm_unlink(SHM_NAME);    
        
        printf("\n[PARENT] Resources released. Program terminated.\n");
    } 
    else {
        // CHILD PROCESS (Cryptographic Module)
        
        // Wait for the parent to write the initial data
        sem_wait(&shm_data->sem_parent_child);
        
        printf("\n[CHILD] Data received. Starting XOR encryption...\n");
        sleep(2); // Simulate processing time
        
        xor_encrypt(shm_data->initial_message, shm_data->encrypted_message, shm_data->encryption_key);
        
        printf("[CHILD] Encryption finished. Saving to shared memory and unlocking the parent.\n");
        
        // Signal the parent that encryption is done
        sem_post(&shm_data->sem_child_parent);
        
        exit(EXIT_SUCCESS);
    }

    return 0;
}