#Written by: Austin Fahsl, Alex Memering and Joel Shapiro
KERNELDIR = kernel/
FLOPPYDIR = $(KERNELDIR)floppy/
SHELLDIR = shell/

TOCOPY = $(SHELLDIR)shell \
						$(SHELLDIR)loop
COPYLOC = $(TOCOPY:$(SHELLDIR)%=$(FLOPPYDIR)%)
COPYLIB = $(TOCOPY:%=%_asm.o)
LINKEXEC =	$(KERNELDIR)kernel \
						$(TOCOPY)
LINKOBJ = $(LINKEXEC:%=%.o)
LINKASM = $(LINKEXEC:%=%_asm.o)

.PHONY : main clean $(KERNELPHONY)


main : bochsTest

include $(KERNELDIR)makefile


$(COPYLOC) : $(FLOPPYDIR)% : $(SHELLDIR)%
	cp $< $@

$(LINKEXEC) : % : %.o %_asm.o %.h definitions.h
	ld86 -o $@ -d $*.o $*_asm.o

$(KERNELDIR)kernel_asm.o : $(KERNELDIR)kernel.asm
$(COPYLIB) : $(SHELLDIR)lib.asm
$(LINKASM) :
	as86 $< -o $@

#Make sure that bcc is installed
$(LINKOBJ) : %.o : %.c
	bcc -ansi -c -W -o $@ $<

clean: cleanFloppy cleanKernel
	rm -f $(LINKEXEC) \
				$(LINKOBJ) \
				$(LINKASM)
