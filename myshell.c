#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

	(void)argc;
	(void)argv;
		
	char *userIO = NULL;
	size_t n = 0;

	while(1){

		printf("sate-shell>");
 	        fflush(stdout);		/*  forces text out of the buffer */
	
		ssize_t lineENTERED = getline(&userIO, &n, stdin);
		printf("%s", userIO);

		if (lineENTERED == -1) {
   			 printf("\n");
   			 free(userIO);
   			 break;
		}
	}


	return 0;
}
