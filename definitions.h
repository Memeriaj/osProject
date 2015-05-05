/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/

/*Sector information*/
#define FILLERSECTOR 0x00
#define SECTORSIZE 512
#define MAXFILESIZE FILESECTORLENGTH*SECTORSIZE
#define EXECUTEAREA 0x2000

/*Directory configuration*/
#define MAXFILEENTRY 16
#define FILEENTRYLENGTH 32
#define NAMELENGTH 6
#define FILESECTORLENGTH (FILEENTRYLENGTH - NAMELENGTH)

/*Sector locations*/
#define DIRECTORYSECTOR 2
#define MAPSECTOR 1
