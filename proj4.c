/*
Class: CPSC 346-02
Team Member 1: Will Fraisl
Team Member 2: N/A 
GU Username of project lead: wfraisl
Pgm Name: proj4.c
Pgm Desc: Runs a shell 
Usage: ./a.out
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 80
#define TRUE 80

char **getInput();
char **parseInput(char *);
void dispOutput(char **);
void addToHistory(char *);
void printHistory();
/*
global variable for num of arguments
pointer to last arg for hist
*/
char **history_args;

int main(int argc, char *argv[])
{
    //A pointer to an array of pointers to char.  In essence, an array of
    //arrays.  Each of the second level arrays is a c-string. These hold
    //the command line arguments entered by the user.
    //as exactly the same structure as char* argv[]
    char **args;
    pid_t pid;
    int status;

    //setup history
    history_args = calloc(10, sizeof(char *));
    int i;
    for (i = 0; i < 10; i++)
    {
        history_args[i] = calloc(80, sizeof(char));
    }
    while (TRUE)
    {
        printf("myShell> ");
        fflush(stdout);
        args = getInput();
        //dispOutput(args);
        //if the user has entered "quit" break out of the loop.

        if (!strncmp(args[0], "quit", 4))
        {
            printf("quit %d", pid);
            exit(0);
        }
        else if (!strncmp(args[0], "cd", 2))
        {
            chdir(args[1]);
        }
        else if (!strncmp(args[0], "history", 7))
        {
            printHistory();
        }
        else if (!strncmp(args[0], "!!", 2)){
            pid = fork();
            if (pid < 0) //fork failed
            {
                fprintf(stderr, "Fork Error");
                return 1;
            }
            else if (pid == 0)
            {                          //child process
                char **temp = parseInput(history_args[1]);
                execvp(temp[0], temp); //load args[0] into the child's address space
                exit(0);
            }
            else //parent process
            {
                wait(NULL);
            }
        }
        else
        {
            pid = fork();
            if (pid < 0) //fork failed
            {
                fprintf(stderr, "Fork Error");
                return 1;
            }
            else if (pid == 0)
            {                          //child process
                execvp(args[0], args); //load args[0] into the child's address space
                exit(0);
            }
            else //parent process
            {
                wait(NULL);
            }
        }
    }
    return 0;
}

/*
Reads input string from the key board.   
invokes parseInput and returns the parsed input to main
*/
char **getInput()
{
    char *input = calloc(80, sizeof(char));
    fgets(input, sizeof(char) * 80, stdin);
    strtok(input, "\n");
    addToHistory(input);
    return parseInput(input);
}

/*
inp is a cstring holding the keyboard input
returns an array of cstrings, each holding one of the arguments entered at
the keyboard. The structure is the same as that of argv
Here the user has entered three arguements:
myShell>cp x y
*/
char **parseInput(char *input)
{
    int arg_count = 1;
    char *temp = input;

    while (*temp)
    {
        if (*temp == ' ')
        {
            arg_count++;
        }
        *temp++;
    }

    char **parsed_input = calloc(arg_count + 1, sizeof(char *));
    int i;

    for (i = 0; i < arg_count; i++)
    {
        parsed_input[i] = calloc(80, sizeof(char));
    }

    i = 0;
    char *token;
    char *space = " ";

    token = strtok(input, space);
    do
    {
        parsed_input[i] = token;
        token = strtok(NULL, space);
        i++;
    } while (token != NULL);

    parsed_input[arg_count] = NULL;

    return parsed_input;
}

/*
Displays the arguments entered by the user and parsed by getInput
*/
void dispOutput(char **args)
{
    int i = 0;
    while (args[i] != '\0')
    {
        printf("%s ", args[i]);
        i++;
    }
    printf("\b\n");
}

void addToHistory(char *input){
    int i;
    for (i = 9; i > 0; i--){        
        history_args[i]=history_args[i-1];
    }
    history_args[0] = input;
}

void printHistory(){
    int i = 0;
    for(i=9; i>=0; i--){
        printf("%s\n", history_args[i]);
    }
}