#Written by: Austin Fahsl, Alex Memering and Joel Shapiro
KERNELDIR = kernel/
FLOPPYDIR = $(KERNELDIR)floppy/
SHELLDIR = shell/

LINKEXEC =	$(KERNELDIR)kernel \
						$(SHELLDIR)shell
LINKOBJ = $(LINKEXEC:%=%.o)
LINKASM = $(LINKEXEC:%=%_asm.o)

LOADFILES =	shell \
						message.txt \
						tstprg \
						tstpr2
REQLOADFILES = $(LOADFILES:%=$(FLOPPYDIR)%)

REQDDS =	$(FLOPPYDIR)bootload \
					$(FLOPPYDIR)map.img \
					$(FLOPPYDIR)dir.img \
					$(FLOPPYDIR)kernel
DDEXTRAS =	count=1 \
						seek=1 \
						seek=2 \
						seek=3
DDS = $(join $(REQDDS), $(addprefix ~, $(DDEXTRAS)))

.PHONY : clean bochsTest main

main : bochsTest

kernel/floppya.img : $(FLOPPYDIR)floppya.img
	mv $(FLOPPYDIR)floppya.img $(KERNELDIR)floppya.img

$(FLOPPYDIR)floppya.img :	$(REQDDS) \
													$(FLOPPYDIR)loadFile \
													$(REQLOADFILES)
	dd if=/dev/zero of=$@ bs=512 count=2880
	$(foreach item, $(DDS), dd if=$(subst ~, ,$(item)) of=$@ bs=512 conv=notrunc;)
	cd $(FLOPPYDIR) && \
		$(foreach file, $(LOADFILES), ./loadFile $(file);)

$(FLOPPYDIR)shell : $(SHELLDIR)shell
	mv $< $@

$(FLOPPYDIR)kernel : $(KERNELDIR)kernel
	mv $< $@

$(FLOPPYDIR)loadFile : $(FLOPPYDIR)loadFile.c
	gcc -o $@ $<

$(LINKEXEC) : % : %.o %_asm.o
	ld86 -o $@ -d $^

$(KERNELDIR)kernel_asm.o : $(KERNELDIR)kernel.asm
$(SHELLDIR)shell_asm.o : $(SHELLDIR)lib.asm
$(LINKASM) :
	as86 $< -o $@

#Make sure that bcc is installed
$(LINKOBJ) : %.o : %.c
	bcc -ansi -c -W -o $@ $<

#Make sure to have installed nasm
$(FLOPPYDIR)bootload : $(FLOPPYDIR)bootload.asm
	nasm $<

bochsTest: $(KERNELDIR)opsys.bxrc $(KERNELDIR)floppya.img
	cd $(KERNELDIR) && \
		bochs -f opsys.bxrc

clean:
	rm -f $(KERNELDIR)floppya.img \
				$(KERNELDIR)bochsout.txt \
				$(FLOPPYDIR)bootload \
				$(FLOPPYDIR)loadFile \
				$(FLOPPYDIR)shell \
				$(FLOPPYDIR)kernel \
				$(LINKEXEC) \
				$(LINKOBJ) \
				$(LINKASM)
