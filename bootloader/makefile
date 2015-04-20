#Written by: Austin Fahsl, Alex Memering and Joel Shapiro
.PHONY : clean bochsTest main

main : bochsTest

floppya.img : bootload kernel
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=bootload of=$@ bs=512 count=1 conv=notrunc
	dd if=kernel of=$@ bs=512 conv=notrunc seek=3
	dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notruc

kernel : kernel.o kernel_asm.o
	ld86 -o $@ -d $^

kernel_asm.o : kernel.asm
	as86 $< -o $@

#Make sure that bcc is installed
kernel.o : kernel.c
	bcc -ansi -c -W -o $@ $<

#Make sure to have installed nasm
bootload : bootload.asm
	nasm $<

bochsTest: opsys.bxrc floppya.img
	bochs -f opsys.bxrc

clean:
	rm -f floppya.img bochsout.txt bootload *.o kernel
