// Shell command definition
typedef struct {
    char** argsList;
    int argsCount;
} ShellCommand;

//Functions to implement:
void startShell();
char* CommandPrompt(); // Display current working directory and return user input
ShellCommand* ParseCommandLine(char* input); // Process the user input (As a shell command)
void ExecuteCommand(ShellCommand* command); //Execute a shell command