int main(){
	int q, w;
	int count = 12345;
	int max = 1000;
    interrupt(0x21, 0x0, "Looping\r\n", 0, 0);
    for(q=0; q<max; q++){
    	for(w=0; w<max; w++){
    		// count = count^0x1234 * count + q*w;
    		interrupt(0x21, 0x0, "\0", 0, 0);
    	}
    }
    interrupt(0x21, 0x0, "Done\r\n", 0, 0);
    interrupt(0x21, 0x5, 0, 0, 0);
}