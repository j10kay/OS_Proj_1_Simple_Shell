#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;
char *token;
pid_t pid;

void int_handler(int signum)
{
  printf("\n");
  kill(pid, SIGKILL);
}

void timed_handler(int signum)
{
  printf("\n");
  kill(pid, SIGKILL);
}

int main() {
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];
  
    // Stores the current working directory. 
    char cwd[MAX_COMMAND_LINE_LEN];
  
    	
    while (true) {
      
        getcwd(cwd, sizeof(cwd));
        do{ 
            // Print the shell prompt.
            printf("%s%s", cwd, prompt);
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }
      
         int i = 0;
         token = strtok(command_line, delimiters);
         while (token != NULL)
         {
           arguments[i] = token;
           i++;
           token = strtok(NULL, delimiters);
         }
      
         if (strcmp(arguments[0], "pwd") == 0){  // Task 1
            printf("%s\n", cwd);
            continue;
         } else if (strcmp(arguments[0], "echo") == 0){
            char output[MAX_COMMAND_LINE_LEN];
            int output_index = 1;
            while (output_index < i){
              if (output_index > 1){
                 strcat(output, " ");
              }
              if (arguments[output_index][0] == '$'){
                char* path_variable;
                char *temp = strtok(arguments[output_index], "$"); 
                path_variable = getenv(temp);
                if (path_variable != NULL)
                  strcat(output, path_variable);
              } else {
                strcat(output, arguments[output_index]);
              }
              output_index += 1;
            }
            printf("%s\n", output);
            strcpy(output, "");
            continue;
         } else if (strcmp(arguments[0], "exit") == 0){
            break;
         } else if (strcmp(arguments[0], "cd") == 0){
           chdir(arguments[1]);
           continue;
         } else if (strcmp(arguments[0], "setenv") == 0){
           char *token = strtok(arguments[1], "="); 
           char *env_var = token;
           char *env_val;
            while (token != NULL) 
            {
                env_val = token; 
                token = strtok(NULL, "="); 
            } 
           setenv(env_var, env_val, 1);
           continue;
         } else if (strcmp(arguments[0], "env") == 0){
           char **env = environ;
           while (*env != 0)
            {
              char *thisEnv = *env;
              printf("%s\n", thisEnv);
              env++;
            }
           continue;
         }
      
          pid = fork();  // Task 2 - Adding Processes 
          int background_process = 0;  // Task 3 - Adding Background Processes
					if (arguments[1] != NULL){
						if (strcmp(arguments[1], "&") == 0){
							background_process = 1;
							arguments[1] = NULL;
						}
					}
          if (pid < 0) {  
              perror("Fork error!\n");
              exit(1);
          } else if (pid == 0){
            signal(SIGINT, int_handler); // Task 4 - Signal Handling
            
            int fd0;  // Task 6 - Extra Credit 
						int out = 0;
						char output[64];
            int j; 
						for(j=0; arguments[j] != '\0'; j++)
						{
								if(strcmp(arguments[j], ">")==0){        
										arguments[j]=NULL;
										strcpy(output, arguments[j+1]);
										out=1;           
								}                       
						}					
						if(out > 0)
						{
								int fd1 ;
								if ((fd1 = creat(output , 0644)) < 0) {
										perror("Error occurred while trying to open the output file");
										exit(0);
								}           

								dup2(fd1, 1);
								close(fd1);
						}
            
            if (execvp(arguments[0], arguments) < 0 ){
              perror("execvp() failed: No such file or directory\n");   // Input is not executable.
              exit(1); 
            }
            exit(0);
          } else {
            signal(SIGINT, int_handler); // Task 4 - Signal Handling
            signal(SIGALRM, timed_handler); // Task 5 - Killing off long processes 
            alarm(10);
            if (background_process == 1){
              background_process = 0;
            } else {
              wait(NULL);
            }
          }

    }
    // This should never be reached.
    return -1;
}