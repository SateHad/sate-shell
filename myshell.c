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

	while(1){

		printf("sate-shell>");
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
			token = strtok(NULL, "\n");					
			args[i] = token;
			
			++i;	
		}

		args[i] = NULL;
		
		if(args[0] == NULL){
			continue;
		}
		
		if (pid < 0) { /* Fork failure */

			printf("Error: System is out of memory/processes.\n");
	
		} else if (pid == 0) { /* Child Process */
    			// Hand the array to the OS.
    			execvp(args[0], args);

    			// If execvp succeeds, this process is destroyed and replaced by the target program.
    			// If you reach the lines below execvp, it means the command didn't exist (e.g., user typed "asdf").
   			printf("Command not found\n");
    			exit(1); // Kill the child process so it doesn't accidentally become a second shell.
		} else {  /* Parent Process */
    			// Wait for the child to die before showing the next prompt.
    			waitpid(pid, NULL, 0);
		}
	}


	return 0;
}
