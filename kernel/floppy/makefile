#Written by: Austin Fahsl, Alex Memering and Joel Shapiro
FLOPPYPHONY = cleanFloppy

LOADFILES =	shell \
						message.txt \
						tstprg \
						tstpr2 \
						phello
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


$(FLOPPYDIR)floppya.img :	$(REQDDS) \
												$(FLOPPYDIR)loadFile \
												$(REQLOADFILES)
	dd if=/dev/zero of=$@ bs=512 count=2880
	$(foreach item, $(DDS), dd if=$(subst ~, ,$(item)) of=$@ bs=512 conv=notrunc;)
	cd $(FLOPPYDIR) && \
		$(foreach file, $(LOADFILES), ./loadFile $(file);)

$(FLOPPYDIR)loadFile : $(FLOPPYDIR)loadFile.c
	gcc -o $@ $<

#Make sure to have installed nasm
$(FLOPPYDIR)bootload : $(FLOPPYDIR)bootload.asm
	nasm $<


cleanFloppy :
	rm -f $(FLOPPYDIR)bootload \
				$(FLOPPYDIR)loadFile \
				$(FLOPPYDIR)shell \
				$(FLOPPYDIR)kernel \
				$(FLOPPYDIR)floppya.img
