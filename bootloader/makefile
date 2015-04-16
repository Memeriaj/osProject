
.PHONY : clean bochsTest main

main : bochsTest

# Portion 6 for Milestone 1
floppya.img : bootload kernel
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=bootload of=$@ bs=512 count=1 conv=notrunc
	dd if=kernel of=$@ bs=512 conv=notrunc seek=3

kernel : kernel.o kernel_asm.o
	ld86 -o $@ -d $^

kernel_asm.o : kernel.asm
	as86 $< -o $@

kernel.o : kernel.c
	bcc -ansi -c -W -o $@ $< #Make sure that bcc is installed

# Portion 4 for Milestone 1
#floppya.img : bootload
#	dd if=/dev/zero of=$@ bs=512 count=2880
#	dd if=bootload of=$@ bs=512 count=1 conv=notrunc

bootload : bootload.asm
	nasm $< #Make sure to have installed nasm

# Portion 3 for Milestone 1
bochsTest: opsys.bxrc floppya.img
	bochs -f opsys.bxrc

#floppya.img : test.img
#	cp $< $@


clean:
	rm -f floppya.img bochsout.txt bootload *.o kernel
