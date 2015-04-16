/*Written by: Austin Fahsl, Alex Memering and Joel Shapiro*/
int findAddress(int line, int pos);
void writeStringToScreen(int line, int startPos, char* str);

int main(){
  writeStringToScreen(3, 2, "Hello World!");

  while(1){}
  return 0;
}

int findAddress(int line, int pos){
  return (80*(line-1)+pos)*2+0x8000;
}

void writeStringToScreen(int line, int startPos, char* str){
  int pos = startPos;
  while(*str != '\0'){
    int loc = findAddress(line, pos);
    putInMemory(0xB000, loc, *str);
    putInMemory(0xB000, loc+1, 0x7);
    str += 1;
    pos += 1;
  }
  return;
}
