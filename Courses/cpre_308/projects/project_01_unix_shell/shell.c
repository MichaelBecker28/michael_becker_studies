
//Include library statements for shell

#include <stdio.h>      // I/O functions (printf, etc.)
#include <stdlib.h>     // standard lib
#include <string.h> 
#include <signal.h>
#include <unistd.h>
#include <errno.h>      // For error codes
#include <sys/wait.h>   // For process waiting

//Constants and define statements

#define max_in 1024 //sets the max input line length
#define max_arg 64  // max number of arguments
#define prompt "308sh>" //This would be the defaut shell prompt


// function prototypes to define functions

void input_parse (char *input,char **arguments, int *background); // will split input into separate arguments and will check if the command should run in the background
int  built_in (char **arguments);  //for built in shell commands
void execute_program(char **arguments,int background);// (arguments)-array containing command. (background)- flag to run in background
void background_proc_check();//check if any background processes have completed.

//Main Function

int main (int arg_count, char **arg_array){
    //variables
    char *shell_prompt = prompt;
    char input[max_in];
    char *args[max_arg];
    int background;

    /*
    This for loop gets the command line argument and checks if it 
    is a special prompt. It checks if the argument is "-p" 
    */
    for (int i=1; i<arg_count;i++){
        if(strcmp(arg_array[i],"-p")== 0&& i+1 <arg_count){
           shell_prompt = arg_array[i+1];
           i++;
        }
    }

    // main loop for the shell

    while (1){
        background_proc_check();

        //display a prompt:
        printf("%s ",shell_prompt);
        //force display
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin)==NULL){
        break;
        }
    

    //parse the input
    input_parse(input, args,&background);

    //Continue if there was no command
    if (args[0]== NULL){
        continue;
    }

    //try different commands built in and not
    if (!built_in(args)){
        execute_program(args, background);

    }
    }

    return 0;
}

/**
 * This section will contain my helper methods that were seen at the top
 * 
 * **********************************************************************
 * 
 */

 /**
  * Parse the input into separate sections and arguments
  * 
  * @param input the user input being parsed
  * @param arguments array that stores the parsed inputs
  * @param background background pointer for flagging 
  */
 void input_parse(char *input, char **arguments, int *background){
    int i =0;
    char *tok;

    tok =strtok(input, "\t");
    //while loop to store token in the argument array
    while (tok !=NULL && i<max_arg-1){
        arguments[i++] = tok;
        tok= strtok(NULL, "\t");
    }

    //Now need to check if the last is & to see for background processes
    if (i>0 && strcmp(arguments[i-1], "&")==0){
        *background =1; //backround flag
        arguments[i-1]=NULL;//remove it from arguments
    }else{
        arguments[i]=NULL;
    }


    }

 /**
  * Now we are focusing on the built in shell commands 
  * 
  * @param arguments (this is an array of command arguments)
  * @return this returns 1 if the command was built in and completed
  *         if it doesn't then it will return 0
  */
 int built_in(char **arguments){
    //check if the command is built in
    if (strcmp(arguments[0],"exit")==0){
        exit(0);
        return 1;
    }else if (strcmp(arguments[0],"pid")==0){
        printf("%d\n",getpid());
        return 1;
    } else if (strcmp(arguments[0], "ppid") == 0) {
        printf("%d\n", getppid());   // Print parent's process ID
        return 1;
    } else if (strcmp(arguments[0], "pwd") == 0) {
        char cwd[max_in];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);     // Print current working directory
        } else {
            perror("getcwd");        // Error getting directory
        }
        return 1;
    } else if (strcmp(arguments[0], "cd") == 0) {
        if (arguments[1] == NULL) {
            // No argument, change to HOME directory
            char *home = getenv("HOME");
            if (home != NULL) {
                if (chdir(home) != 0) {
                    perror("chdir");
                }
            }
        } else {
            // Change to specified directory
            if (chdir(arguments[1]) != 0) {
                perror("chdir");
            }
        }
        return 1;
    }
    
    // Not a built-in command
    return 0;
}

/**
 * This will implement different programs and stuff alike
 * 
 * @param arguments (same as before an array of commands)
 * @param background same as before a flag if it should run in the back
 * 
 */

 void execute_program(char **arguments, int background){
    pid_t pid;

    //create child with fork
    pid = fork();

    if (pid<0){
        //error forking
        perror("fork failed");
        return;
    }else if (pid ==0){
        //child 
        execvp(arguments[0],arguments);

        //fail of execvp
        printf("Cannot exec");
        exit(EXIT_FAILURE);
    }else{
        //this would be the parent process
        printf("[%d] %s\n",pid,arguments[0]);

        if (!background){
            //wait for the processes not in the background to finish

            int status;
            waitpid(pid, &status,0);
            //print the exit status
            if (WIFEXITED(status)){
                printf("[%d] %s exit %d\n", pid, arguments, WEXITSTATUS(status));
            } 
            else if (WIFSIGNALED(status)) {
                printf("[%d] %s Killed (%d)\n", pid, arguments[0], WTERMSIG(status));
            }
            }
        }
    }
/**
 * Check if any background processes have completed
 */
void background_proc_check() {
    int status;
    pid_t pid;
    
    // Check for any completed background processes non-blocking
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Print process exit status
        if (WIFEXITED(status)) {
            printf("[%d] Exit %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[%d] Killed (%d)\n", pid, WTERMSIG(status));
        }
    }
}
 