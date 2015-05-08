/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/

/*Print letters*/
#define UPPERCONVERSION 256
#define PRINTLETTERINTERTUPT 0x10
#define LETTEROFFSET 0xe*UPPERCONVERSION

/*Read letters*/
#define READCHARINTERRUPT 0x16
#define BACKSPACECODE 0x8
#define ENTERCODE 0xd

/*Sector indicators*/
#define SECTORINTERRUPT 0x13
#define READSECTORINDICATOR 2
#define WRITESECTORINDICATOR 3
#define SECTORSTOREAD 1
#define DEVICENUMBER 0



/*Interrupt 21 functions*/
void handleInterrupt21(int ax, int bx, int cx, int dx);
void printString(char* message);
void readString(char* store);
void readSector(char* buffer, int sector);
void readFile(char* name, char* buffer);
void executeProgram(char* name, int segment);
void terminate();
void writeSector(char* toWrite, int sectorNum);
void deleteFile(char* name);
void writeFile(char* filename, char* contents, int numSectors);

/*Time intterupt functions*/
void handleTimerInterrupt(int segment, int sp);

/*Helper functions*/
int mod(int a, int b);
int div(int a, int b);
int matchNames(char* first, char* second, int length);
void loadFileSectors(char* buffer, char* dir);
void readWriteSector(char* buffer, int sector, int readWrite);
