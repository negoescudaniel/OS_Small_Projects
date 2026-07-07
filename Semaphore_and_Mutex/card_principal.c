#include stdio.h
#include stdlib.h
#include semaphore.h
#include fcntl.h
#include unistd.h
#include time.h

int main() {
    sem_t sem;
    int balance, i;
    const char sem_name = bank_sem;
    const char file_name = bank_account.txt;

     Create semaphoreresources
    FILE f = fopen(file_name, w);
    if (!f) { perror(Error creating file); return 1; }
    fprintf(f, %d, 1000);  Initial balance
    fclose(f);

     Create the named semaphore
    sem = sem_open(sem_name, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) { perror(sem_open); return 1; }

    printf(MAIN CARD - System initialized with 1000 RON.n);

     Simulate transactions
    srand(time(NULL));
    for (i = 0; i  3; i++) {
        sleep(rand() % 2 + 1);
        
        sem_wait(sem);  Lock access
        
        f = fopen(file_name, r);
        fscanf(f, %d, &balance);
        fclose(f);

        if (balance = 100) {
            balance -= 100;
            f = fopen(file_name, w);
            fprintf(f, %d, balance);
            fclose(f);
            printf([MAIN] Withdrawal 100 RON. New balance %dn, balance);
        }
        
        sem_post(sem);  Release access
    }

     Wait and cleanup
    printf(MAIN CARD - Own transactions completed. Waiting 15 seconds for the other card...n);
    sleep(15); 

    sem_close(sem); 
    sem_unlink(sem_name); 
    remove(file_name);  

    printf(MAIN CARD - Resources deleted. Program terminated.n);
    return 0;
}