/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/

/*Entered line*/
#define MAXLINELENGTH 256
#define MAXARGSLENGTH 100
#define MAXARGCHARS 100

/*Sector information*/
#define FILLERSECTOR 0x00
#define SECTORSIZE 512
#define MAXFILESIZE FILESECTORLENGTH*SECTORSIZE

/*Directory configuration*/
#define MAXFILEENTRY 16
#define FILEENTRYLENGTH 32
#define NAMELENGTH 6
#define FILESECTORLENGTH (FILEENTRYLENGTH - NAMELENGTH)

/*Sector locations*/
#define DIRECTORYSECTOR 2
#define MAPSECTOR 1
