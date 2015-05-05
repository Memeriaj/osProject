/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/

/*Configures input and hands it to correct command*/
void matchCommand(char* line);
int match(char* line, char* command);
void breakApartArgs(char* args[], char* line);

/*Shell commands*/
void typeCommand(char* args[]);
void executeCommand(char* args[]);
void deleteCommand(char* args[]);
void copyCommand(char* args[]);
void dirCommand(char* args[]);
void createCommand(char* args[]);
