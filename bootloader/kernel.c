/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/
void printString(char* message);
void readString(char* store);

int main(){
  char line[80];
  printString("Enter a line: \0");
  readString(line);
  printString(line);

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
