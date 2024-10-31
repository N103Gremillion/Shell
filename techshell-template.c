/*
* Name(s): Nathan Gremillion
* Date: 8/31/2024
* Description: **Include what you were and were not able to handle!**
*
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

// Shell command definition
typedef struct {
    char* curCommand;
    char** args;
    int argsCount;
} ShellCommand;

//Functions to implement:
char* CommandPrompt(); // Display current working directory and return user input
ShellCommand ParseCommandLine(char* input); // Process the user input (As a shell command)
void ExecuteCommand(ShellCommand command); //Execute a shell command

int main() {

    char* input;
    ShellCommand command;

    //struct ShellCommand command;

    // repeatedly prompt the user for input
    for (;;)
    {
        input = CommandPrompt();
        // parse the command line
        command = ParseCommandLine(input);
        // execute the command
        //ExecuteCommand(command);
        free(input);
    }
    exit(0);
}

// parse the input form the user into a CommandPromt format that is easier to read from allocats 256 bytes for the struct
ShellCommand ParseCommandLine(char* input){
    ShellCommand command;
    return command;
}

// get input form user and return using scanf() and alloc a 
char* CommandPrompt() {

    long size;
    char* cwd;
    // buffer for user input
    char* commands;

    // get the current path size
    size = pathconf(".", _PC_PATH_MAX);
    cwd = (char*)malloc(size);
    commands = (char*)malloc(256);

    if (cwd == NULL) {
        printf("Error allocating buffer");
        return NULL;
    }

    // try and get the current directory
    if (getcwd(cwd, size) == NULL) {
        printf("Error when trying to get the current directory");
        free(cwd);
        return NULL;
    }

    printf("%s$ ", cwd);

    // get a input form the user
    scanf("%255s", commands);
    
    free(cwd);

    return commands;
}