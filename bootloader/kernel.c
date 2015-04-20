/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/
void printString(char* message);

int main(){
  //writeStringToScreen(3, 2, "Hello World!");
  printString("Hello World");

  while(1){}
  return 0;
}

void printString(char* message){
  char* current = message;
  while(*current != '\0'){
    interrupt(0x10, 0xe*256+*current, 0, 0, 0);
    current++;
  }
}
