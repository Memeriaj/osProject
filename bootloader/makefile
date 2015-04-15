
.PHONY : clean bochsTest main

main : floppya.img

# Portion 2 for Milestone 1
floppya.img : bootload
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=bootload of=$@ bs=512 count=1 conv=notrunc

bootload : bootload.asm
	nasm $< #Make sure to have installed nasm

# Portion 3 for Milestone 1
bochsTest: opsys.bxrc floppya.img
	bochs -f opsys.bxrc

#floppya.img : test.img
#	cp $< $@


clean:
	rm -f floppya.img bochsout.txt bootload
