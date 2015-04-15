
.PHONY : clean bochsTest

bochsTest: opsys.bxrc floppya.img
	bochs -f opsys.bxrc

floppya.img : test.img
	cp $< $@

clean:
	rm -f floppya.img bochsout.txt
