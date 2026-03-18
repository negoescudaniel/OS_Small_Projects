#include<sys/wait.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/uio.h>
#include<string.h>
int main(){
	while(1){
		int p[2];
		int pid;
		char command[30];
		pipe(p);
		
		printf("small-shell> ");
		fgets(command, sizeof(command),stdin);
		
		if(command[0] == '\n')
			continue;
			
		if(strcmp(command, "exit\n")==0)
		{
			printf("Goodbye! Have a good day! \n");
			break;
		}
		
		pid = fork();
		
		if(pid < 0)
		{
			printf("There has been an error while using fork()!\n");
			return(-1);
		}
		else if (pid >0)
		{ //father process
			close(p[0]);
			write(p[1], command, strlen(command) +1);
			close(p[1]);
			//wait for the son process to finish
			wait(NULL);
		}
		else
		{//Son process
			char buffer[40];
			int i = 0;
			char *args[10];
			
			close(p[1]);
			read(p[0], buffer, sizeof(buffer));
			close(p[0]);
			
			// parsing the command
			args[i] = strtok(buffer, " \n");
			while (args[i] != NULL)
			{
				i++;
				args[i] = strtok(NULL, " \n");
			}
			
			//exec
			execvp(args[0], args);
			
			printf("Exec error! \n");
			return(-1);
		}
	
	}
	return 0;
}