#Written by: Austin Fahsl, Alex Memering and Joel Shapiro
KERNELPHONY = bochsTest \
							cleanKernel \
							$(FLOPPYPHONY)

include $(FLOPPYDIR)makefile


$(KERNELDIR)floppya.img : $(FLOPPYDIR)floppya.img
	mv $< $@

$(FLOPPYDIR)kernel : $(KERNELDIR)kernel
	mv $< $@

bochsTest: $(KERNELDIR)opsys.bxrc $(KERNELDIR)floppya.img
	cd $(KERNELDIR) && \
		bochs -f opsys.bxrc


cleanKernel: cleanFloppy
	rm -f $(KERNELDIR)floppya.img \
				$(KERNELDIR)bochsout.txt
