#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/wait.h>

int main(void) {
    int chid;
    pid_t pid;

    // parent creates the communication channel
    chid = ChannelCreate(0); 
    if (chid == -1) {
        perror("Error creating channel");
        exit(EXIT_FAILURE);
    }

    // create the child process
    pid = fork();
    if (pid < 0) {
        perror("Error at fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // CHILD PROCESS 
        int coid;
        char text_original[100];
        char text_criptat[100];

        // the child connects to the channel created by the parent
        coid = ConnectAttach(0, getppid(), chid, _NTO_SIDE_CHANNEL, 0);
        if (coid == -1) {
            perror("[CHILD] Connection error");
            exit(EXIT_FAILURE);
        }

        // user text
        printf("Child process: Enter the text to encrypt: ");
        fgets(text_original, sizeof(text_original), stdin);
        
        text_original[strcspn(text_original, "\n")] = 0; 


        // MsgSend blocks the child until the parent executes MsgReply()
        if (MsgSend(coid, text_original, strlen(text_original) + 1, text_criptat, sizeof(text_criptat)) == -1) {
            perror("Child process: Error at MsgSend");
        } else {
            printf("\nChild process UNBLOCKED! Received response from parent.\n");
            printf("Child process: The encrypted text is: %s\n", text_criptat);
        }

        // close the connection and stop the child's execution
        ConnectDetach(coid);
        exit(EXIT_SUCCESS);

    } else {

        // PARENT PROCESS
        int rcvid;
        char buffer_mesaj[100];
        int i; 

        printf("Parent process: Waiting for messages on channel %d...\n", chid);

        // MsgReceive is blocking. The parent waits until a message arrives
        rcvid = MsgReceive(chid, buffer_mesaj, sizeof(buffer_mesaj), NULL);
        if (rcvid == -1) {
            perror("Parent process: Error at MsgReceive");
        } else {
            printf("\nParent process: Received text from child: \"%s\"\n", buffer_mesaj);
            printf("Parent process: Encrypting the message...\n");

            // apply the "encryption"
            for (i = 0; i < strlen(buffer_mesaj); i++) {
                buffer_mesaj[i] = buffer_mesaj[i] + 1;
            }

            //sleep(2); 

            // send the processed message back and unblock the child
            MsgReply(rcvid, 0, buffer_mesaj, strlen(buffer_mesaj) + 1);
            printf("Parent process: sent response back to child.\n");
        }

        // wait for the child to finish its execution 
        wait(NULL);
        // destroy the communication channel at the end
        ChannelDestroy(chid);
        printf("Parent process: Channel destroyed. Execution finished successfully.\n");
    }

    return 0;
}