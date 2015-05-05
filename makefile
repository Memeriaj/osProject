#Written by: Austin Fahsl, Alex Memering and Joel Shapiro
KERNELDIR = kernel/
FLOPPYDIR = $(KERNELDIR)floppy/
SHELLDIR = shell/

LINKEXEC =	$(KERNELDIR)kernel \
						$(SHELLDIR)shell
LINKOBJ = $(LINKEXEC:%=%.o)
LINKASM = $(LINKEXEC:%=%_asm.o)

.PHONY : main clean $(KERNELPHONY)


main : bochsTest

include $(KERNELDIR)makefile


$(FLOPPYDIR)shell : $(SHELLDIR)shell
	mv $< $@

$(LINKEXEC) : % : %.o %_asm.o %.h definitions.h
	ld86 -o $@ -d $*.o $*_asm.o

$(KERNELDIR)kernel_asm.o : $(KERNELDIR)kernel.asm
$(SHELLDIR)shell_asm.o : $(SHELLDIR)lib.asm
$(LINKASM) :
	as86 $< -o $@

#Make sure that bcc is installed
$(LINKOBJ) : %.o : %.c
	bcc -ansi -c -W -o $@ $<

clean: cleanFloppy cleanKernel
	rm -f $(LINKEXEC) \
				$(LINKOBJ) \
				$(LINKASM)
