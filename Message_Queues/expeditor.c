#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>

int main(void) {
    mqd_t coada;
    unsigned int prioritate;
    char input[100];
    char mesaj_final[100];

    // open the queue for writing only
    coada = mq_open("/coada_qos", O_WRONLY);
    if (coada == (mqd_t)-1) {
        perror("Error connecting to the queue (does a queue exist?)");
        exit(EXIT_FAILURE);
    }

    printf("------NETWORK TRAFFIC GENERATOR CONNECTED-----\n");
    printf("Available commands:\n");
    printf(" - VOIP     (Send video/voice traffic - HIGH priority)\n");
    printf(" - DOWNLOAD (Send file traffic - LOW priority)\n");
    printf(" - SHUTDOWN (Close the Router, respectively the queue)");
    printf(" - quit     (Stop the application)\n\n");

    // Infinite loop for reading commands
    while (1) {
        printf("Enter the traffic type > ");
        // Read from keyboard
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break; 
        }

        // Clean the 'newline' (\n) character left by fgets at the end of the string
        input[strcspn(input, "\n")] = 0;

        // approach if the user enters 'quit'
        if (strcmp(input, "quit") == 0 || strcmp(input, "QUIT") == 0) {
            printf("Stopping traffic generator...\n");
            break;
        }
        
        if (strcmp(input, "SHUTDOWN") == 0) {
            prioritate = 1;
            strcpy(mesaj_final, "SHUTDOWN");
            
            // send the message
            mq_send(coada, mesaj_final, strlen(mesaj_final) + 1, prioritate);
            
            printf(">> SHUTDOWN command sent! Exiting sender....\n");
            break;
        }

        // set the priority and the message based on input
        if (strcmp(input, "VOIP") == 0) {
            prioritate = 5;
            strcpy(mesaj_final, "Live voice/video traffic");
        } else if (strcmp(input, "DOWNLOAD") == 0) {
            prioritate = 2;
            strcpy(mesaj_final, "Download .zip file");
        } else {
            printf("Invalid command! Type VOIP, DOWNLOAD or quit.\n\n");
            continue; 
        }

        // send the message to the queue, associating its priority
        if (mq_send(coada, mesaj_final, strlen(mesaj_final) + 1, prioritate) == -1) {
            perror("Error at mq_send");
        } else {
            printf("   -> [SUCCESS] Packet sent: %s [OS Priority: %u]\n\n", mesaj_final, prioritate);
        }
    }

    // close the connection to the message queue on program exit
    mq_close(coada);
    return 0;
}