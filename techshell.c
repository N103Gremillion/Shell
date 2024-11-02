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
        
        // check if they exit
        if (command != NULL && strcmp(command->argsList[0], "exit") == 0) {
            exit(0);
        }
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
        if (path == NULL || strcmp(path, "~") == 0){
            char* home = getenv("HOME");
            status = chdir(home);

            // -1 indicates a error
            if (status == -1){
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            }
        }
        else {
            status = chdir(path);

            // if path is invalid
            if (status == -1){
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            }
        }
        return;
    }

    pid_t pid = fork();

    if (pid == 0){

        char** argsList = command->argsList;
        int totalCommands = command->argsCount;
        FILE* inputFile = NULL;
        FILE* outputFile = NULL;

        // loop over the args and check for < and >
        for (int i = 0; i < totalCommands; i++){
            // input redirection
            if (strcmp(argsList[i], "<") == 0 && argsList[i + 1] != NULL){
                inputFile = fopen(argsList[i + 1], "r");
                if (inputFile == NULL){
                    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                    exit(1);
                }
                // redirect the input 0 for stdin
                dup2(fileno(inputFile), 0);
                fclose(inputFile);

                // remove the < and filename from the arguments list
                argsList[i] = NULL;
                argsList[i + 1] = NULL;
                i++;
            }
            // output redirection
            else if (strcmp(argsList[i], ">") == 0 && argsList[i + 1] != NULL){
                outputFile = fopen(argsList[i + 1], "w");
                if (outputFile == NULL){
                    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                    exit(1);
                }
                // 1 is for stdout
                dup2(fileno(outputFile), 1);
                fclose(outputFile);

                // remove the < and filename from the arguments list
                argsList[i] = NULL;
                argsList[i + 1] = NULL;
                i++;
            }
        }

        // filter out all the NULL args
        char* filteredArguments[MAXARGS];
        int j = 0;

        for (int i = 0; i < totalCommands; i++) {
            if (argsList[i] != NULL){
                filteredArguments[j] = argsList[i];
                j++;
            }
        }
        filteredArguments[j] = NULL;

        int statusCode = execvp(filteredArguments[0], argsList);

        if (statusCode == -1){
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
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
    char* home;
    char* displayPath;
    char* commands;

    // Get the path size limit
    size = pathconf(".", _PC_PATH_MAX);
    cwd = (char*)malloc(size);
    commands = (char*)malloc(256);

    if (cwd == NULL || commands == NULL) {
        printf("Error allocating buffer\n");
        return NULL;
    }

    // Try to get the current directory
    if (getcwd(cwd, size) == NULL) {
        printf("Error when trying to get the current directory\n");
        free(cwd);
        free(commands);
        return NULL;
    }

    // Get the user's home directory
    home = getenv("HOME");

    // Allocate buffer for display path
    displayPath = (char*)malloc(size);

    // Check if cwd starts with home, and replace with '~' if it does
    if (home != NULL && strncmp(cwd, home, strlen(home)) == 0) {
        snprintf(displayPath, size, "~%s", cwd + strlen(home));
    } else {
        strncpy(displayPath, cwd, size);
    }

    printf("%s$ ", displayPath);

    // Get input from user
    scanf(" %255[^\n]", commands);
    
    free(cwd);
    free(displayPath);

    return commands;
}