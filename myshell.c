#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Singly linked list node for dynamically storing command history */

struct Node{
	char *history_command;
	struct Node *next;	
};

struct Node *head = NULL;

/* Built-in command and linked list function prototypes */

void help();
void echo(char **args);
void history(unsigned int n);

void insertFirst(char *history_command);
void insertLast(char *history_command);

int main(int argc, char *argv[]){

	(void)argc;
	(void)argv;

	printf("===================================================\n");
	printf("  Welcome to sate-shell! Thanks for testing it out.\n");
	printf("  Type 'help' to see available commands.\n");
	printf("===================================================\n\n");

	char *args[100];
	
	unsigned int history_count = 0;

	char *userIO = NULL;
	size_t n = 0;

	/* Initialize shell in HOME directory and cache path length for '~' prompt formatting */
	signed int check = chdir(getenv("HOME"));
	if(check == -1)
		return 0;

	char *homeAddress = getenv("HOME");
	unsigned int homeSize = 0;
	if(homeAddress != NULL){
		homeSize = strlen(homeAddress);
	}
		
	while(1){
		
		/* Resolve current working directory and render dynamic prompt */
		char *dirNow = getcwd(NULL, 0); 
		
		if(homeAddress != NULL && strncmp(dirNow, homeAddress, homeSize) == 0){
			printf("sate-shell:~%s$", dirNow + homeSize);
		}

 	        fflush(stdout);		/*  forces text out of the buffer */
		
		/* Read raw user input from stdin and handle EOF, End Of File (Ctrl+D) */
		ssize_t lineENTERED = getline(&userIO, &n, stdin);

		if (lineENTERED == -1){
			printf("\n");
			free(userIO);
			break;
		}

		/* Preserve uncut command string for history before strtok mutates userIO */
		char *command = strdup(userIO);

		/* Tokenize input by whitespace and newlines into NULL-terminated args array */
		char *token = strtok(userIO, " \n");
		args[0] = token;	

		unsigned int i = 1;	
		while(token != NULL){
			token = strtok(NULL, " \n");					
			args[i] = token;
			
			++i;	
		}

		args[i] = NULL;

		/* Discard empty/whitespace-only inputs; log valid commands to history */
		if(args[0] == NULL){
			free(command);
			continue;
		}

		insertLast(command);
		++history_count;

		/* Built-in command dispatcher (intercepted before forking child processes) */
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
		else if(strcmp(args[0], "help") == 0){
			help();
			continue;
		}
		else if(strcmp(args[0], "echo") == 0){
			echo(args);
			continue;
		}
		else if(strcmp(args[0], "history") == 0){
			history(history_count);
			continue;
		}

		/* Spawn child process to execute external system binaries */
		pid_t pid = fork();
		
		if (pid < 0) {	/* Fork failure */

			printf("Error: System is out of memory/processes.\n");
	
		} else if (pid == 0) {	/* Child Process */
    			execvp(args[0], args);
			
   			printf("Command not found\n");
    			exit(1);
		} else {	/* Parent Process */
    			waitpid(pid, NULL, 0);
		}
	}


	return 0;
}

/* Allocate and prepend a new command node to the head of the history list */
void insertFirst(char *history_command){
	struct Node *newNode = malloc(sizeof(struct Node));
	newNode->history_command = history_command;
	newNode->next = NULL;

	if(head == NULL) {
		head = newNode;
	}
	else {
		newNode->next = head;
		head = newNode;
	}
}

/* Allocate and append a new command node to the tail of the history list */
void insertLast(char *history_command){
	struct Node *newNode = malloc(sizeof(struct Node));
	newNode->history_command = history_command;
	newNode->next = NULL;
	
	if(head == NULL){
		head = newNode;
	}
	else{
		struct Node* curr = head;
		while(curr->next != NULL){
			curr = curr->next;
		}
		curr->next = newNode;
	}
}

/* Display supported built-in and system commands */
void help(){
	printf("\n--- Built-in Commands (Handled Internally) ---\n");
	printf("  cd      - changes directory based on path given (defaults to HOME if blank)\n");
	printf("  echo    - prints whatever arguments follow it\n");
	printf("  history - lists commands entered previously in the session\n");
	printf("  help    - displays supported commands and shell info\n");
	printf("  exit    - terminates the shell session\n");

	printf("\n--- External System Commands (Executed via $PATH) ---\n");
	printf("  ls      - lists files and folders in the current directory\n");
	printf("  grep    - searches for matching text patterns inside files\n");
	printf("  pwd     - displays current working directory\n");
	printf("  mkdir   - creates new directories\n");
	printf("  touch   - creates new files\n");

	printf("\n[Future Roadmap] sate-shell will be expanded with pipelining (|),\n");
	printf("I/O redirection, and other core features found in Bash.\n\n");
}

/* Print space-separated command arguments to stdout */
void echo(char **args){
	if(args != NULL){
		for(int i = 1; args[i] != NULL; ++i){
			printf("%s ", args[i]);
		}
		printf("\n");
		fflush(stdout);
	}
	else{
		return;
	}
}

/* Traverse the linked list and print numbered command history */
void history(unsigned int n){
	if(head != NULL && n > 0){
		struct Node* curr = head;
		unsigned int i = 0;
		
		/* traversing through history list */
		while(curr != NULL && i < n){
			printf("%d: %s", i + 1, curr->history_command);
			curr = curr->next;
			++i;
		}
	}	
	else{
		return;
	}
}

