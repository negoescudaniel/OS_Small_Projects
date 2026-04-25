#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>

// function that simulates packet processing
void simuleaza_lucru() {
    printf("   [Router] Inspecting and routing the packet (takes 2 sec)...\n");
    sleep(10);
    printf("   [Router] Packet routed successfully!\n\n");
}

int main(void) {
    mqd_t coada;
    char msg[4096]; 
    unsigned int prioritate;

    // create the message queue
    coada = mq_open("/coada_qos", O_CREAT | O_RDONLY, 0666, NULL);
    if (coada == (mqd_t)-1) {
        perror("Error creating the queue (mq_open)");
        exit(EXIT_FAILURE);
    }

    printf("---------ROUTER STARTED---------\n");
    printf("Blocked for 10 seconds. To give the sender time to send messages \n");
    sleep(10); 

    printf(">>> Starting packet extraction:\n");

    // Extract packets one by one
    while (1) {
        // mq_receive automatically extracts the message with the highest priority
        if (mq_receive(coada, msg, sizeof(msg), &prioritate) == -1) {
            printf("No more packets in the queue or an error occurred.\n");
            break; 
        }
        
        if(strcmp(msg, "SHUTDOWN") == 0){
            printf(">> Initializing router shutdown ...");
            break;
        }

        printf(">> PACKET EXTRACTED: \"%s\" [OS Priority: %u]\n", msg, prioritate);
        simuleaza_lucru(); 
    }

    // close and delete the message queue
    mq_close(coada);
    mq_unlink("/coada_qos");
    printf("Router stopped. The queue has been deleted.\n");

    return 0;
}