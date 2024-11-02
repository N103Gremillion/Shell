# Shell
Name: Nathan Gremillion

this program acts as a shell session and is able to act as a bash shell. 


****************************************techsell********************************

techshell.h 
  - this file contains the declorations for the functions used and also the struct ShellCommand

techshell.c
  - CommandPrompt() - prints the currrent working directory and then uses a buffer to get user input uses pathconf() and getcwd() to get the currentPath / also uses scanf() to get commandLine arguments from user

  - ParseCommandLine() - parse the input form the user into a CommandPromt format that is easier to read from this is a (ShellCommand struct) / the ShellCommand struct contains a argsCount(number of args in input), argsList(List of strings in the input)

  - ExecuteCommand() - takes in a CommandPrompt stuct and executes the arguments in it in a forked process(if not a cd command) uses fork() to create a child proceess and execv() to execute a command in the forked process

  - startShell() - startup the techshell and runs CommandPrompt() to print current directory then get input form user / then runs ParseCommand(input) on the input from CommandPrompt() returning a ShellCommand instance / uses the ExecuteCommand(command) to try and execute the current ShellCommand /
  repetedly loops over using for(;;) to continue the shell prompt

main.c 
  - into to the program runs startShell();