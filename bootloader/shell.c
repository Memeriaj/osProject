/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/\
#define MAXLINELENGTH 256

void matchCommand(char* line);

int main(){
  char line[MAXLINELENGTH];

  while(1){
    interrupt(0x21, 0, "Shell> ", 0, 0);
    interrupt(0x21, 1, line, 0, 0);
    matchCommand(line);
  }
  interrupt(0x21, 5, 0, 0, 0);
}


void matchCommand(char* line){
  interrupt(0x21, 0, "Bad Command!\r\n", 0, 0);
}
