#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define NUM_CHILDREN 3												//how many children

void sigint_handler(int sig) {										//interrupt handler
    printf("\nControl+C is pressed. Exiting...\n");
    exit(0);
    }

int main() {
    int fd[NUM_CHILDREN][2];
    pid_t pid[NUM_CHILDREN];
    char input[100];

    signal(SIGINT, sigint_handler); 								//interrupt handler

    
    for (int i = 0; i < NUM_CHILDREN; i++) {						//Pipe 
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    
    for (int i = 0; i < NUM_CHILDREN; i++) {						// Fork child 
        
        pid[i] = fork();
                ;
        if (pid[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid[i] == 0) {
			
            close(fd[i][1]);  
			
            char output[100];										// Read data from pipe							
            if (read(fd[i][0], output, sizeof(output)) == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            fork();
            printf("Child %d process received: %s\n", i, output);

            close(fd[i][0]);  

            exit(EXIT_SUCCESS);

        }
    }

   
    for (int i = 0; i < NUM_CHILDREN; i++) {						 // Parent process: write to pipes
        close(fd[i][0]);  											// Close read
        sleep(1);

        printf("Enter a message: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';  

			if (write(fd[i][1], input, strlen(input) + 1) == -1) {	// Write data to pipe
            perror("write");
            exit(EXIT_FAILURE);
        }

        printf("Parent process sent to child %d: %s\n", i, input);

        close(fd[i][1]);  
    }
    
    sleep(1000);
    return 0;
    }