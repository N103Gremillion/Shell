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
#include <string.h>
#include "techshell.h"

#define TRUE 1
#define FALSE 0
#define MAXARGS 10

void startShell(){
    char* input;
    ShellCommand* command;

    // repeatedly prompt the user for input
    for (;;)
    {
        input = CommandPrompt();

        // parse the command line
        command = ParseCommandLine(input);

        printf("The total number of args in is : %d.\n", command->argsCount);
        printf("The first arg is : %s.\n", command->curArg);

        for (int i = 0; i < command->argsCount; i++){
            printf("The %d commmand is : %s. \n", i, command->argsList[i]);
        }

        printf("\n");

        // execute the command
        //ExecuteCommand(command);

        free(input);
    }
    exit(0);
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
    }

    if (argsCount == 0){
        free(argsList);
        free(command);
        return NULL;
    }

    command->curArg = argsList[0];
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