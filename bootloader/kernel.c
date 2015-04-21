/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/
void printString(char* message);
void readString(char* store);
void readSector(char* buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);

int mod(int a, int b);
int div(int a, int b);

int main(){
  char line[80];
  char buffer[512];

  printString("Enter a line: \0");
  readString(line);
  printString(line);

  readSector(buffer, 30);
  printString(buffer);

  makeInterrupt21();
  interrupt(0x21, 1, line, 0, 0);
  interrupt(0x21, 0, line, 0, 0);

  while(1){}
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
    interrupt(0x10, 0xe*256+*(store+cur), 0, 0, 0);
    cur++;

    if(*(store+cur) == 0x8 && cur > 0){
      /* We might want to write a <space> to the location to blank it */
      cur-= 2;
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
  int q;

  int read = 2;
  int sectorsToRead = 1;
  int trackNumber = div(sector, 36);
  int realativeSectorNumber = mod(sector, 18)+1;
  int headNumber = mod(div(sector,18),2);
  int deviceNumber = 0;

  char* retrievedSector = interrupt(0x13, read*256+sectorsToRead,
                                      buffer,
                                      trackNumber*256+realativeSectorNumber,
                                      headNumber*256+deviceNumber);
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
    default:
      printString("Interrupt21 got an ax that was undefined.");
  }

  return;
}
