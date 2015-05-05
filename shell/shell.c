/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/
#include "../definitions.h"
#include "shell.h"

int main(){
  char line[MAXLINELENGTH];

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
  return 1;
}

void breakApartArgs(char* args[], char* line){
  int q;
  int curArg = 0;
  int pos = 0;

  args[curArg] = line;
  for(q=0; line[q] != '\0'; q++){
    if(line[q] == ' '){
      curArg++;
      pos = 0;
      line[q] = '\0';
      args[curArg] = line+q+1;
    }
  }
  return;
}



void typeCommand(char* args[]){
  char buffer[MAXFILESIZE];
  interrupt(0x21, 0x3, args[1], buffer, 0);
  interrupt(0x21, 0x0, buffer, 0 , 0);
  return;
}

void executeCommand(char* args[]){
  interrupt(0x21, 0x4, args[1], 0x2000, 0);
  return;
}

void deleteCommand(char* args[]){
  interrupt(0x21, 0x7, args[1], 0, 0);
  return;
}

void copyCommand(char* args[]){
  char buffer[MAXFILESIZE];
  int charCount = 0;
  int rem = 0;
  interrupt(0x21, 0x3, args[1], buffer, 0);
  while(buffer[charCount] != '\0' && charCount < MAXFILESIZE){
    charCount++;
  }
  charCount++;
  if(charCount % SECTORSIZE != 0){
    rem = 1;
  }
  interrupt(0x21, 0x8, args[2], buffer , (charCount / SECTORSIZE) + rem);
  return;
}

void dirCommand(char* args[]) {
  char directory[SECTORSIZE];
  char output[SECTORSIZE];
  int pos, entry, q, sectorCount;

  interrupt(0x21, 0x2, directory, DIRECTORYSECTOR, 0);

  pos = 0;
  for(entry=0; entry<MAXFILEENTRY; entry++){
    if(directory[entry*FILEENTRYLENGTH] != FILLERSECTOR){
      for(q=0; q<NAMELENGTH; q++){
        if(directory[entry*FILEENTRYLENGTH+q] != FILLERSECTOR){
          output[pos] = directory[entry*FILEENTRYLENGTH+q];
        }else{
          output[pos] = ' ';
        }
        pos++;
      }

      output[pos] = ' ';
      pos++;
      output[pos] = ' ';
      pos++;

      for(sectorCount=0; sectorCount<FILESECTORLENGTH; sectorCount++){
        if(directory[entry*FILEENTRYLENGTH+NAMELENGTH+sectorCount] == FILLERSECTOR){
          break;
        }
      }

      /*'0' is charater #48*/
      output[pos] = (sectorCount / 10)+48;
      pos++;
      q = sectorCount & 15;
      if(q >= 10){
        q = q-10;
      }
      output[pos] = q+48;
      pos++;
      output[pos] = '\r';
      pos++;
      output[pos] = '\n';
      pos++;
      output[pos] = '\0';
    }
  }
  interrupt(0x21, 0x0, output, 0, 0);
  return;
}

void createCommand(char* args[]) {
  char buffer[MAXFILESIZE];
  char line[MAXLINELENGTH];
  int charCount = 0;
  int rem = 0;
  int q;

  while(1){
    interrupt(0x21, 0, "> ", 0, 0);
    interrupt(0x21, 1, line, 0, 0);

    if(line[0] == '\r'){
      buffer[charCount] = '\0';
      break;
    }

    q=0;
    while(line[q] != '\0'){
      buffer[charCount] = line[q];
      charCount++;
      q++;
    }
  }
  charCount++;
  if(charCount % SECTORSIZE != 0){
    rem = 1;
  }

  interrupt(0x21, 0x8, args[1], buffer, (charCount / SECTORSIZE) + rem);
  return;
}
