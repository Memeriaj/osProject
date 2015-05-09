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
  interrupt(0x21, 0x4, args[1], EXECUTEAREA, 0);
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
