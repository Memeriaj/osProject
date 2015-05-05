/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/
#define MAXFILEENTRY 16
#define FILEENTRYLENGTH 32
#define NAMELENGTH 6
#define FILESECTORLENGTH (FILEENTRYLENGTH - NAMELENGTH)
#define FILLERSECTOR 0x00
#define SECTORSIZE 512
#define MAXFILESIZE FILESECTORLENGTH*SECTORSIZE
#define DIRECTORYSECTOR 2
#define MAPSECTOR 1

void printString(char* message);
void readString(char* store);
void readSector(char* buffer, int sector);
void readFile(char* name, char* buffer);
void executeProgram(char* name, int segment);
void terminate();
void writeSector(char* toWrite, int sectorNum);
void deleteFile(char* name);
void writeFile(char* filename, char* contents, int numSectors);
void handleInterrupt21(int ax, int bx, int cx, int dx);

int mod(int a, int b);
int div(int a, int b);
int matchNames(char* first, char* second, int length);
void loadFileSectors(char* buffer, char* dir);
void readWriteSector(char* buffer, int sector, int readWrite);

int main(){
  makeInterrupt21();

  interrupt(0x21, 4, "shell\0", 0x2000, 0);

  return 0;
}



void printString(char* message){
  char* current = message;
  while(*current != '\0'){
    interrupt(0x10, 0xe*256+(*current), 0, 0, 0);
    current++;
  }
  return;
}

void readString(char* store){
  int cur = 0;
  *(store+cur) = interrupt(0x16, 0, 0, 0, 0);
  while(*(store+cur) != 0xd){
    if(*(store+cur) == 0x8 && cur > 0){
      /* We might want to write a <space> to the location to blank it */
      interrupt(0x10, 0xe*256+*(store+cur), 0, 0, 0);
      cur-= 1;
    } else if (*(store+cur) != 0x8){
      interrupt(0x10, 0xe*256+*(store+cur), 0, 0, 0);
      cur++;
    }

    *(store+cur) = interrupt(0x16, 0, 0, 0, 0);
  }
  interrupt(0x10, 0xe*256+'\n', 0, 0, 0);
  interrupt(0x10, 0xe*256+'\r', 0, 0, 0);

  cur++;
  *(store+cur) = '\r';
  cur++;
  *(store+cur) = '\n';
  cur++;
  *(store+cur) = '\0';
  return;
}



void readSector(char* buffer, int sector){
  readWriteSector(buffer, sector, 2);
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



void readFile(char* name, char* buffer){
  char directory[512];
  int entry, matched;

  readSector(directory, 2);
  for(entry=0; entry<MAXFILEENTRY; entry++){
    matched = matchNames(name, directory+entry*FILEENTRYLENGTH, NAMELENGTH);
    if(matched == 1){
      break;
    }
  }
  buffer[0] = '\0';
  if(matched == 0){
    return;
  }

  loadFileSectors(buffer, directory+entry*FILEENTRYLENGTH);

  return;
}

int matchNames(char* first, char* second, int length){
  int namePos;
  for(namePos=0; namePos<length; namePos++){
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



void executeProgram(char* name, int segment){
  char buffer[MAXFILESIZE];
  int curLoadChar;

  readFile(name, buffer);
  for(curLoadChar=0; curLoadChar<MAXFILESIZE; curLoadChar++){
    putInMemory(segment, curLoadChar, buffer[curLoadChar]);
  }

  launchProgram(segment);
  return;
}



void terminate(){
  char shell[6];
  shell[0] = 's';
  shell[1] = 'h';
  shell[2] = 'e';
  shell[3] = 'l';
  shell[4] = 'l';
  shell[5] = '\0';
  interrupt(0x21, 4, shell, 0x2000, 0);
  return;
}



void writeSector(char* toWrite, int sectorNum){
  readWriteSector(toWrite, sectorNum, 3);
  return;
}

void readWriteSector(char* buffer, int sector, int readWrite){
  int q;

  int sectorsToRead = 1;
  int trackNumber = div(sector, 36);
  int realativeSectorNumber = mod(sector, 18)+1;
  int headNumber = mod(div(sector,18),2);
  int deviceNumber = 0;

  char* retrievedSector = interrupt(0x13, readWrite*256+sectorsToRead, buffer,
                                      trackNumber*256+realativeSectorNumber,
                                      headNumber*256+deviceNumber);
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
    if(filename[q] == '\0' || filenameEnd){
      filenameEnd = 1;
      directory[entry*FILEENTRYLENGTH+q] = FILLERSECTOR;
    }else{
      directory[entry*FILEENTRYLENGTH+q] = filename[q];
    }
  }

  /*Find and fill free sectors*/
  sectorCount = 0;
  for(sector=3; sector<SECTORSIZE && sectorCount<numSectors; sector++){
    if(map[sector] == FILLERSECTOR){
      map[sector] == 0xFF;
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
      executeProgram(bx, cx);
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
    default:
      /*printString("Interrupt21 got undefined ax.");*/
      break;
  }

  return;
}
