#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main() {
    sem_t *sem;
    FILE *f;
    int balance, i;
    const char *sem_name = "/bank_sem";
    const char *file_name = "bank_account.txt";

    // Try to open the semaphore
    sem = sem_open(sem_name, 0);
    if (sem == SEM_FAILED) {
        printf("SECONDARY CARD - Error: Start the Main Card first!\n");
        return 1;
    }

    srand(time(NULL) + getpid());
    for (i = 0; i < 3; i++) {
        sleep(rand() % 3 + 1);

        printf("SECONDARY CARD - Waiting for ATM access...\n");
        sem_wait(sem); // Wait if the Main Card is working

        f = fopen(file_name, "r");
        if (f) {
            fscanf(f, "%d", &balance);
            fclose(f);

            if (balance >= 100) {
                balance -= 100;
                f = fopen(file_name, "w");
                fprintf(f, "%d", balance);
                fclose(f);
                printf("SECONDARY CARD - I withdrew 100 RON. Remaining balance: %d\n", balance);
            }
        }

        sem_post(sem); 
    }

    sem_close(sem);
    printf("SECONDARY CARD - Transactions finished.\n");
    return 0;
}