#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

	(void)argc;
	(void)argv;

	char *args[100];
		
	char *userIO = NULL;
	size_t n = 0;

	signed int check = chdir(getenv("HOME"));
	if(check == -1)
		return 0;

	char *homeAddress = getenv("HOME");
	unsigned int homeSize = 0;
	if(homeAddress != NULL){
		homeSize = strlen(homeAddress);
	}
	

	while(1){
		
		char *dirNow = getcwd(NULL, 0); 
		
		if(homeAddress != NULL && strncmp(dirNow, homeAddress, homeSize) == 0){
			printf("sate-shell:~%s$", dirNow + homeSize);
		}

 	        fflush(stdout);		/*  forces text out of the buffer */
	
		ssize_t lineENTERED = getline(&userIO, &n, stdin);
	
		if (lineENTERED == -1){
			printf("\n");
			free(userIO);
			break;
		}
		
		char *token = strtok(userIO, " \n");
		args[0] = token;	

		unsigned int i = 1;	
		while(token != NULL){
			token = strtok(NULL, " \n");					
			args[i] = token;
			
			++i;	
		}

		args[i] = NULL;

		if(args[0] == NULL){
			continue;
		}

		if(strcmp(args[0], "exit") == 0){
			printf("\n");
			free(userIO);
			break;
		}
		else if(strcmp(args[0], "cd") == 0){
			if(args[1] == NULL){
				chdir(getenv("HOME"));
			}
			else if(args[1] != NULL){
				signed int cdir = chdir(args[1]);
				if(cdir < 0){
					printf("No directory with the path: %s\n", args[1]);
					continue;
				}
				else{
					continue;
				}
			}
		}

		pid_t pid = fork();
		
		if (pid < 0) { /* Fork failure */

			printf("Error: System is out of memory/processes.\n");
	
		} else if (pid == 0) { /* Child Process */
    			execvp(args[0], args);
			
   			printf("Command not found\n");
    			exit(1);
		} else {  /* Parent Process */
    			waitpid(pid, NULL, 0);
		}
	}


	return 0;
}
