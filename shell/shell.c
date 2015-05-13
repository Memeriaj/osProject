/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/
#include "../definitions.h"
#include "shell.h"

int main(){
  char line[MAXLINELENGTH];

  enableInterrupts();
  while(1){
    interrupt(0x21, 0, "Shell> ", 0, 0);
    interrupt(0x21, 1, line, 0, 0);
    matchCommand(line);
  }
  interrupt(0x21, 5, 0, 0, 0);
  return 0;
}



void matchCommand(char* line){
  char* args[MAXARGSLENGTH];
  breakApartArgs(args, line);

  if(match(args[0], "type\0")){
    typeCommand(args);
  }else if(match(args[0], "execute\0")){
    executeCommand(args);
  }else if(match(args[0], "delete\0")){
    deleteCommand(args);
  }else if(match(args[0], "copy\0")){
    copyCommand(args);
  }else if(match(args[0], "dir\0")){
    dirCommand(args);
  }else if(match(args[0], "create\0")){
    createCommand(args);
  }else if(match(args[0], "kill\0")){
    killCommand(args);
  }else if(match(args[0], "clear\0")){
    clearCommand(args);
  }else if(match(args[0], "quit\0")){
    quitCommand(args);
  }else if(match(args[0], "executeforeground\0")){
    executeForegroundCommand(args);
  }else if(match(args[0], "edit\0")){
    editCommand(args);
  }else if(match(args[0], "top\0")){
    listProcessCommand(args);
  }else if(match(args[0], "help\0")){
    helpCommand(args);
  }else{
    interrupt(0x21, 0, "Bad Command!\r\n", 0, 0);
  }
  return;
}

int match(char* line, char* command){
  int q;
  for(q=0; line[q] != '\n' && line[q] != '\0' && command[q] != '\0'; q++){
    if(line[q] != command[q]){
      return 0;
    }
  }
  if(!((line[q] == '\n' || line[q] == '\0') && command[q] == '\0')){
    return 0;
  }
  return 1;
}

void breakApartArgs(char* args[], char* line){
  int q;
  int curArg = 0;

  args[curArg] = line;
  for(q=0; line[q] != '\0'; q++){
    if(line[q] == ' '){
      curArg++;
      line[q] = '\0';
      args[curArg] = line+q+1;
    }
    if(line[q] == '\r'){
      curArg++;
      line[q] = '\0';
      break;
    }
  }
  return;
}



void editCommand(char* args[]){

  return;
}



void typeCommand(char* args[]){
  char buffer[MAXFILESIZE];
  interrupt(0x21, 0x3, args[1], buffer, 0);
  interrupt(0x21, 0x0, buffer, 0 , 0);
  return;
}



void executeCommand(char* args[]){
  interrupt(0x21, 0x4, args[1], EXECUTEAREA, 0);
  return;
}


void executeForegroundCommand(char* args[]){
  interrupt(0x21, 0xa, args[1], 0, 0);
  return;
}



void deleteCommand(char* args[]){
  interrupt(0x21, 0x7, args[1], 0, 0);
  return;
}



void copyCommand(char* args[]){
  char buffer[MAXFILESIZE];
  int charCount = 0;

  interrupt(0x21, 0x3, args[1], buffer, 0);
  while(buffer[charCount] != '\0' && charCount < MAXFILESIZE){
    charCount++;
  }
  charCount++;

  interrupt(0x21, 0x8, args[2], buffer , neededSectors(charCount));
  return;
}

int neededSectors(int charactersToStore){
  int rem = 0;
  if(charactersToStore % SECTORSIZE != 0){
    rem = 1;
  }
  return (charactersToStore / SECTORSIZE) + rem;
}



void dirCommand(char* args[]) {
  char directory[SECTORSIZE];
  char output[SECTORSIZE];
  int pos, entry, q, sectorCount;

  interrupt(0x21, 0x2, directory, DIRECTORYSECTOR, 0);

  pos = 0;
  for(entry=0; entry<MAXFILEENTRY; entry++){
    if(directory[entry*FILEENTRYLENGTH] != FILLERSECTOR){
      pos += addEntryName(output+pos, directory+entry*FILEENTRYLENGTH);

      output[pos] = ' ';
      pos++;
      output[pos] = ' ';
      pos++;

      pos += addSectorCount(output+pos, directory+entry*FILEENTRYLENGTH+NAMELENGTH);

      output[pos] = '\r';
      pos++;
      output[pos] = '\n';
      pos++;
    }
  }
  output[pos] = '\0';
  interrupt(0x21, 0x0, output, 0, 0);
  return;
}

int addEntryName(char* output, char* name){
  int q;
  for(q=0; q<NAMELENGTH; q++){
    if(name[q] != FILLERSECTOR){
      output[q] = name[q];
    }else{
      output[q] = ' ';
    }
  }
  return q;
}

int addSectorCount(char* output, char* sectors){
  int sectorCount;
  int pos = 0;

  for(sectorCount=0; sectorCount<FILESECTORLENGTH; sectorCount++){
    if(sectors[sectorCount] == FILLERSECTOR){
      break;
    }
  }

  output[pos] = (sectorCount / 10)+'0';
  pos++;
  output[pos] = fastMod10(sectorCount)+'0';
  pos++;
  return pos;
}

int fastMod10(int num){
  int out = num & 15;
  if(out >= 10){
    out = out-10;
  }
  return out;
}



void createCommand(char* args[]) {
  char buffer[MAXFILESIZE];
  char line[MAXLINELENGTH];
  int charCount = 0;
  int q;

  while(1){
    interrupt(0x21, 0, "> ", 0, 0);
    interrupt(0x21, 1, line, 0, 0);

    if(line[0] == '\r'){
      buffer[charCount] = '\0';
      charCount++;
      break;
    }

    q = 0;
    while(line[q] != '\0'){
      buffer[charCount] = line[q];
      charCount++;
      q++;
    }
  }

  interrupt(0x21, 0x8, args[1], buffer, neededSectors(charCount));
  return;
}



void killCommand(char* args[]){
  interrupt(0x21, 0x9, args[1][0] - '0', 0, 0);
  return;
}

void clearCommand(char* args[]){
  int i = 0;
  while (i < CONSOLEHEIGHT){
    interrupt(0x21, 0x0, "\n", 0, 0);
    i++;
  }   
}

void quitCommand(char* args[]){
  interrupt(0x21, 0x5, 0, 0, 0);
}

void listProcessCommand(char* args[]){
  interrupt(0x21, 0xb, 0, 0, 0);
  return;
}

void helpCommand(char* args[]){
  char* commands[12];
  int count;
  commands[0] = "execute arg1 - execute the program of name specified in arg1\r\n\0";
  commands[1] = "delete arg1 - delete the file of name specified in arg1\r\n\0";
  commands[2] = "copy arg1 arg2 - copy the file of name arg1 to a new file named arg2\r\n\0";
  commands[3] = "dir - display the names and sizes (# sectors) of all files in the system\r\n\0";
  commands[4] = "create arg1 - create a new file of name arg1 and begin filling its contents. Type text and press enter to start a new line, and press enter on an empty line to finish.\r\n\0";
  commands[5] = "kill arg1 - kill the process of id arg1. The shell has a process id of 0.\r\n\0";
  commands[6] = "clear - clear the terminal window of all previously printed text\r\n\0";
  commands[7] = "quit - exit the shell\r\n\0";
  commands[8] = "executeforeground arg1 - execute the program of name arg1 in the foreground, blocking all other processes from running in the meantime\r\n\0";
  commands[9] = "edit arg1 - edit the file of name arg1. Upon editing, each line of the file will appear and be editable in the console. Press enter to go to the next line, or exit the file if on the last line.\r\n\0";
  commands[10] = "top - display the list of top-level processes with names and their process ids\r\n\0"; 
  commands[11] = "help - display this help menu\r\n\0";
  for (count = 0; count < 12; count++) {
    interrupt(0x21, 0x0, commands[count], 0, 0);
  }
  return;
}