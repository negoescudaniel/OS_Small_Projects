#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include"dfkeyexch.h"

// I wanted to simulare a Diffie-Hellman Key Exchance 
//and then a message exchange unsing symmetric cryptography


int main(){
	
	int p=23, g=5;
	int a = 6;
	int A = mod_exp(g, a, p);
	int fd_write, fd_read;
	int B, K;
	char msg[100];
	
	mkfifo("pipe_ab", 0666);
	mkfifo("pipe_ba", 0666);
	
	fd_write = open("pipe_ab", O_WRONLY);
	fd_read = open("pipe_ba", O_RDONLY);
	
	// Send A
	write(fd_write, &A, sizeof(A));
	
	// Read B
	read(fd_read, &B, sizeof(B));
	
	//compute  key
	K = mod_exp(B, a, p);
					
	while(1)
	{
		printf("Alice(You): ");
		fgets(msg, sizeof(msg), stdin);
		
		if(strcmp(msg, "exit\n") == 0)
			break;
			
		xor_encrypt(msg, K);
		write(fd_write, msg, sizeof(msg));
		
		read(fd_read, msg, sizeof(msg));
		xor_encrypt(msg, K);
		
		printf("Bob: %s", msg);
	}
	
	close(fd_write);
	close(fd_read);
	
	unlink("pipe_ab");
	unlink("pipe_ba");

	return 0;
}