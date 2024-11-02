/*
* Name(s): Nathan Gremillion
* Date: 8/31/2024
* Description: **Include what you were and were not able to handle!**
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>
#include <errno.h>
#include "techshell.h"

#define TRUE 1
#define FALSE 0
#define MAXARGS 10
#define DEBUG

void startShell(){
    char* input;
    ShellCommand* command;

    // repeatedly prompt the user for input
    for (;;)
    {
        input = CommandPrompt();

        // parse the command line
        command = ParseCommandLine(input);

        // execute the command
        ExecuteCommand(command);

        free(input);
    }
    exit(0);
}

void ExecuteCommand(ShellCommand* command){
    if (command == NULL) return;

    // check if the command is cd since it can not be run in the child process
    if (strcmp(command->argsList[0], "cd") == 0) {
        char* path = command->argsList[1];
        int status;
        // defualt to home directory if not arg is given to change to 
        if (path == NULL){
            char* home = getenv("HOME");
            status = chdir(home);

            // -1 indicates a error
            if (status == -1){
                fprintf(stderr, "Error: %s\n", strerror(errno));
            }
        }
        else {
            status = chdir(path);

            // if path is invalid
            if (status == -1){
                fprintf(stderr, "Error: %s\n", strerror(errno));
            }
        }
        return;
    }

    pid_t pid = fork();

    if (pid == 0){

        char** argsList = command->argsList;
        char* command = argsList[0];

        int statusCode = execvp(command, argsList);

        if (statusCode == -1){
            fprintf(stderr, "Error: %s\n", strerror(errno));
            exit(1);
        }
        exit(0);
    }
    wait(NULL);
}

// parse the input form the user into a CommandPromt format that is easier to read from allocats 256 bytes for the struct
ShellCommand* ParseCommandLine(char* input){

    if (input == NULL){
        return NULL;
    }

    ShellCommand* command = malloc(sizeof(ShellCommand)); 
    char** argsList = malloc(MAXARGS * sizeof(char*));
    int argsCount = 0;
    char buffer[100] = "";
    int inArg = FALSE;
    int wordLen = 0;

    for (int i = 0; i < strlen(input); i++){
        if (input[i] == ' '){
            if (inArg == TRUE){
                buffer[wordLen] = '\0';
                argsList[argsCount] = strdup(buffer);
                argsCount++;
                wordLen = 0;
                inArg = FALSE;
            }
        }
        else{
            if (inArg == FALSE){
                inArg = TRUE;
            }
            
            if (wordLen < 99){
                buffer[wordLen] = input[i];
                wordLen++;
            }   
        }
    }

    // add the final element since elements are added when ther is a ' '
    if (inArg) {
        buffer[wordLen] = '\0';
        argsList[argsCount] = strdup(buffer);
        argsCount++;
        argsList[argsCount] = NULL;
    }

    if (argsCount == 0){
        free(argsList);
        free(command);
        return NULL;
    }

    command->argsCount = argsCount;
    command->argsList = argsList;
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
    scanf(" %255[^\n]", commands);

    free(cwd);

    return commands;
}