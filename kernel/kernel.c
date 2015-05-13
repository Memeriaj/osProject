/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/
#include "../definitions.h"
#include "kernel.h"

int main(){
  makeInterrupt21();
  initializeProcessTable();

  executeProgram("shell", NOONE);
  makeTimerInterrupt();

  while(1);
  return 0;
}



void printString(char* message){
  char* current = message;
  while(*current != '\0'){
    interrupt(PRINTLETTERINTERTUPT, LETTEROFFSET+(*current), 0, 0, 0);
    current++;
  }
  return;
}

void readString(char* store){
  int cur = 0;
  store[cur] = interrupt(READCHARINTERRUPT, 0, 0, 0, 0);
  while(store[cur] != ENTERCODE){
    if(store[cur] == BACKSPACECODE && cur > 0){

      interrupt(PRINTLETTERINTERTUPT, LETTEROFFSET+store[cur], 0, 0, 0);
      interrupt(PRINTLETTERINTERTUPT, LETTEROFFSET+' ', 0, 0, 0);
      interrupt(PRINTLETTERINTERTUPT, LETTEROFFSET+store[cur], 0, 0, 0);
      cur -= 1;
    } else if (store[cur] != BACKSPACECODE){
      interrupt(PRINTLETTERINTERTUPT, LETTEROFFSET+*(store+cur), 0, 0, 0);
      cur++;
    }

    store[cur] = interrupt(READCHARINTERRUPT, 0, 0, 0, 0);
  }
  interrupt(PRINTLETTERINTERTUPT, LETTEROFFSET+'\n', 0, 0, 0);
  interrupt(PRINTLETTERINTERTUPT, LETTEROFFSET+'\r', 0, 0, 0);

  *(store+cur) = '\r';
  cur++;
  *(store+cur) = '\n';
  cur++;
  *(store+cur) = '\0';
  return;
}



void readSector(char* buffer, int sector){
  readWriteSector(buffer, sector, READSECTORINDICATOR);
  return;
}

int mod(int a, int b){
  while(a >= b){
    a = a-b;
  }
  return a;
}

int div(int a, int b){
  int quotient = 0;
  while((quotient+1)*b <= a){
    quotient++;
  }
  return quotient;
}



int readFile(char* name, char* buffer){
  char directory[SECTORSIZE];
  int entry, matched;

  readSector(directory, DIRECTORYSECTOR);
  for(entry=0; entry<MAXFILEENTRY; entry++){
    matched = matchNames(name, directory+entry*FILEENTRYLENGTH, NAMELENGTH);
    if(matched == 1){
      break;
    }
  }
  buffer[0] = '\0';
  if(matched == 0){
    return 0;
  }

  loadFileSectors(buffer, directory+entry*FILEENTRYLENGTH);

  return 1;
}

int matchNames(char* first, char* second, int length){
  int namePos, firstEnd, secondEnd;
  for(namePos=0; namePos<length; namePos++){
    firstEnd = first[namePos] == '\r' || first[namePos] == '\0';
    secondEnd = second[namePos] == '\r' || second[namePos] == '\0';
    if(firstEnd && secondEnd){
      return 1;
    }
    if(first[namePos] != second[namePos]){
      return 0;
    }
  }
  return 1;
}

void loadFileSectors(char* buffer, char* dir){
  int sector, curSector;
  for(sector=0; sector<FILESECTORLENGTH; sector++){
    curSector = *(dir+NAMELENGTH+sector);
    if(curSector != FILLERSECTOR){
      readSector(buffer+SECTORSIZE*sector, curSector);
    }else{
      return;
    }
  }
  return;
}



void executeProgram(char* name, int toWaitOn){
  char buffer[MAXFILESIZE];
  int curLoadChar;
  int q, found, process, charTemp;
  int segment = 0;

  found = readFile(name, buffer);
  if(found != 1){
    return;
  }

  setKernelDataSegment();
  for(q=0; q<NUMBEROFPROCESSENTRIES; q++){
    if(processTable[q].active == 0){
      segment = findProcessTableSegment(q);
      break;
    }
  }
  restoreDataSegment();
  process = q;

  if(segment == 0){
    printString("Too many processes");
    return;
  }

  for(curLoadChar=0; curLoadChar<MAXFILESIZE; curLoadChar++){
    putInMemory(segment, curLoadChar, buffer[curLoadChar]);
  }

  initializeProgram(segment);

  for(q=0; q<NAMELENGTH; q++){
    charTemp = name[q];
    setKernelDataSegment();
    processTable[process].name[q] = charTemp;
    restoreDataSegment();
  }
  setKernelDataSegment();
  processTable[process].name[q] = '\0';

  if(toWaitOn != NOONE){
    processTable[toWaitOn].waitingOn = process;
  }
  processTable[process].active = 1;
  processTable[process].stackPointer = INTITALSTACKLOCATION;
  restoreDataSegment();
  return;
}



void terminate(){
  int cur;
  setKernelDataSegment();
  cur = currentProcess;
  restoreDataSegment();
  killProcess(cur);
  while(1);
  return;
}



void writeSector(char* toWrite, int sectorNum){
  readWriteSector(toWrite, sectorNum, WRITESECTORINDICATOR);
  return;
}

void readWriteSector(char* buffer, int sector, int readWrite){
  int q;

  int trackNumber = div(sector, 36);
  int realativeSectorNumber = mod(sector, 18)+1;
  int headNumber = mod(div(sector,18),2);

  interrupt(SECTORINTERRUPT, readWrite*UPPERCONVERSION+SECTORSTOREAD, buffer,
    trackNumber*UPPERCONVERSION+realativeSectorNumber,
    headNumber*UPPERCONVERSION+DEVICENUMBER);
  return;
}



void deleteFile(char* name){
  char directory[SECTORSIZE], map[SECTORSIZE];
  int entry, matched;
  int sector, curSector;

  readSector(directory, DIRECTORYSECTOR);
  readSector(map, MAPSECTOR);

  for(entry=0; entry<MAXFILEENTRY; entry++){
    matched = matchNames(name, directory+entry*FILEENTRYLENGTH, NAMELENGTH);
    if(matched == 1){
      break;
    }
  }
  if(matched == 0){
    return;
  }

  directory[entry*FILEENTRYLENGTH] = FILLERSECTOR;

  for(sector=0; sector<FILESECTORLENGTH; sector++){
    curSector = *(directory+entry*FILEENTRYLENGTH+NAMELENGTH+sector);
    if(curSector != FILLERSECTOR){
      map[curSector] = FILLERSECTOR;
    }else{
      break;
    }
  }

  writeSector(directory, DIRECTORYSECTOR);
  writeSector(map, MAPSECTOR);

  return;
}



void writeFile(char* filename, char* contents, int numSectors){
  char directory[SECTORSIZE], map[SECTORSIZE];
  int entry, q, filenameEnd;
  int sector, sectorCount, currentChar;
  int remnants;

  readSector(directory, DIRECTORYSECTOR);
  readSector(map, MAPSECTOR);

  /*Find empty directory*/
  for(entry=0; entry<MAXFILEENTRY; entry++){
    if(directory[entry*FILEENTRYLENGTH] == FILLERSECTOR){
      break;
    }
  }

  /*Write filename to directory*/
  filenameEnd = 0;
  for(q=0; q<NAMELENGTH; q++){
    if(filename[q] == '\0' || filename[q] == '\r' || filenameEnd){
      filenameEnd = 1;
      directory[entry*FILEENTRYLENGTH+q] = FILLERSECTOR;
    }else{
      directory[entry*FILEENTRYLENGTH+q] = filename[q];
    }
  }

  /*Find and fill free sectors*/
  sectorCount = 0;
  for(sector=DIRECTORYSECTOR+1; sector<SECTORSIZE && sectorCount<numSectors; sector++){
    if(map[sector] != FILLERSECTOR){
      map[sector] = FILLERSECTOR;
      directory[entry*FILEENTRYLENGTH+NAMELENGTH+sectorCount] = sector;
      writeSector(contents+sectorCount*SECTORSIZE, sector);
      sectorCount++;
    }
  }

  /*Zero remnants of entry*/
  for(remnants=NAMELENGTH+sectorCount+1; remnants<FILEENTRYLENGTH; remnants++){
    directory[entry*FILEENTRYLENGTH+remnants] = FILLERSECTOR;
  }

  writeSector(directory, DIRECTORYSECTOR);
  writeSector(map, MAPSECTOR);

  return;
}



void handleInterrupt21(int ax, int bx, int cx, int dx){
  int cur;
  switch(ax){
    case 0x0: /*Print String*/
      printString(bx);
      break;
    case 0x1: /*Read String*/
      readString(bx);
      break;
    case 0x2: /*Read Sector*/
      readSector(bx, cx);
      break;
    case 0x3: /*Read File*/
      readFile(bx, cx);
      break;
    case 0x4: /*Execute Program*/
      executeProgram(bx, NOONE);
      break;
    case 0x5: /*Terminate Program*/
      terminate();
      break;
    case 0x6: /*Write Sector*/
      writeSector(bx, cx);
      break;
    case 0x7: /*Delete File*/
      deleteFile(bx);
      break;
    case 0x8: /*Write File*/
      writeFile(bx, cx, dx);
      break;
    case 0x9: /*Kill Process*/
      killProcess(bx);
      break;
    case 0xa: /*Execute Program in Blocking Fashion*/
      setKernelDataSegment();
      cur = currentProcess;
      restoreDataSegment();
      executeProgram(bx, cur);
      break;
    case 0xb:
      listProcesses();
      break;
    default:
      printString("Interrupt21 got undefined ax.");
      break;
  }

  return;
}



void handleTimerInterrupt(int segment, int sp){
  int q;
  int nextProcess;

  if(segment == findProcessTableSegment(currentProcess)){
    processTable[currentProcess].stackPointer = sp;
  }

  nextProcess = currentProcess + 1;
  for(q=0; q<NUMBEROFPROCESSENTRIES; q++){
    while(nextProcess >= NUMBEROFPROCESSENTRIES){
      nextProcess -= NUMBEROFPROCESSENTRIES;
    }

    if(processTable[nextProcess].active == 1 && processTable[nextProcess].waitingOn == NOONE){
      break;
    }
    nextProcess++;
  }

  sp = processTable[nextProcess].stackPointer;
  segment = findProcessTableSegment(nextProcess);
  currentProcess = nextProcess;

  returnFromTimer(segment, sp);
}


int findProcessTableEntry(int segment){
  return (segment / 0x1000) - 2;
}
int findProcessTableSegment(int index){
  return (index + 2) * 0x1000;
}


void initializeProcessTable(){
  int q;
  for(q=0; q<NUMBEROFPROCESSENTRIES; q++){
    processTable[q].active = 0;
    processTable[q].stackPointer = INTITALSTACKLOCATION;
    processTable[q].waitingOn = NOONE;
  }
  currentProcess = 0;
  return;
}



void killProcess(int id){
  int q;

  setKernelDataSegment();
  processTable[id].stackPointer = INTITALSTACKLOCATION;
  processTable[id].waitingOn = NOONE;

  for(q=0; q<NUMBEROFPROCESSENTRIES; q++){
    if(processTable[q].waitingOn == id){
      processTable[q].waitingOn = NOONE;
    }
  }

  processTable[id].active = 0;
  restoreDataSegment();
  return;
}


void listProcesses(){
  int q;
  char mess[20];
  char proc[5];
  proc[0] = ' ';
  proc[1] = '#';
  proc[2] = '\r';
  proc[3] = '\n';
  proc[4] = '\0';
  mess[0] = 'P';
  mess[1] = 'r';
  mess[2] = 'o';
  mess[3] = 'c';
  mess[4] = 'e';
  mess[5] = 's';
  mess[6] = 's';
  mess[7] = 'e';
  mess[8] = 's';
  mess[9] = ':';
  mess[10] = '\r';
  mess[11] = '\n';
  mess[12] = '\0';
  printString(mess);

  setKernelDataSegment();  
  for(q=0; q<NUMBEROFPROCESSENTRIES; q++){
    if(processTable[q].active == 1){
      printString(processTable[q].name);

      restoreDataSegment();
      proc[1] = '0'+q;
      printString(proc);
      setKernelDataSegment();  
    }
  }
  restoreDataSegment();

  return;
}
