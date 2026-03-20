#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include"dfkeyexch.h"

int main(){
	
	int p=23, g=5;
	int b = 15;
	int B = mod_exp(g, b, p);
	int fd_write, fd_read;
	int A, K;
	char msg[100];
	
	fd_read = open("pipe_ab", O_RDONLY);
	fd_write = open("pipe_ba", O_WRONLY);
	
	// Recive A
	read(fd_read, &A, sizeof(A));
	
	// Send B
	write(fd_write, &B, sizeof(B));
	
	
	//Compute the key
	K = mod_exp(A, b, p);
	printf("Valoarea chei: %d \n",K);
					
	while(1)
	{
		read(fd_read, msg, sizeof(msg));
		xor_encrypt(msg, K);
		
		printf("Alice: %s", msg);
		
		printf("Bob(You): ");
		fgets(msg, sizeof(msg), stdin);
		
		if(strcmp(msg, "exit\n") == 0)
			break;
			
		xor_encrypt(msg, K);
		write(fd_write, msg, sizeof(msg));
		
	
	}
	
	close(fd_write);
	close(fd_read);
	
	return 0;
}