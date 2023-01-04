INCRT = /usr/$(PFX)include
LIB2 = ../lib2
CC = cc
AS = as
LD = ld
CPP = /lib/$(PFX)cpp
HFILES = $(INCRT)/sys/iorb.h $(INCRT)/sys/commpage.h $(INCRT)/sys/cp.h

all:
	-if test "$(SYS)" != ""; then $(MAKE) comp SYS=$(SYS); \
	elif m88k ; then $(MAKE) comp SYS=OPIO MACH=pm400 ; \
	elif clipper ; then $(MAKE) comp SYS=OPIO MACH=pm300 ; \
	elif ns32332 ; then $(MAKE) comp SYS=OPIO MACH=pm200 ; \
	elif ns32000 ; then $(MAKE) comp SYS=OPMEM MACH=pm100 ; fi

comp:	opsash.$(MACH) opdiag.$(MACH)

opsash.pm400: smon.o sash.o boot.o
	scc -o opsash smon.o sash.o boot.o

opdiag.pm400: low.o Amon.o
	#scc -o opdiag low.o Amon.o
	scc -o opdiag Amon.o

opsash.pm300: smon.o sash.o boot.o
	scc -o opsash smon.o sash.o boot.o

opdiag.pm300: low.o Amon.o
	scc -o opdiag low.o Amon.o

opsash.pm200: smon.o sash.o boot.o
	scc -o opsash smon.o sash.o boot.o

opdiag.pm200: low.o Amon.o
	scc -o opdiag low.o Amon.o

opdiag.pm100: asm.pm100.o low.pm100.o Amon.o boot.o
	$(PFX)ld -o opdiag ld.pm100 asm.pm100.o low.pm100.o Amon.o boot.o \
		-l$(PFX)2

opsash.pm100: sasm.pm100.o smon.o sboot.o sash.o Ainit.o Aconf.o \
	OPMEMdk.o open.o close.o read.o write.o fstat.o lseek.o
	$(PFX)ld -o opsash ld.pm100 sasm.pm100.o smon.o \
		sboot.o sash.o Ainit.o Aconf.o OPMEMdk.o open.o close.o read.o \
		write.o fstat.o lseek.o -l$(PFX)2

Amon.o:	$(HFILES) ../lib2/Amon.c 
	$(PFX)$(CC) -D$(MACH) -Dopus -O -c -DBOOT $(LIB2)/Amon.c
	
low.o: $(HFILES) ../lib2/low.c
	$(PFX)$(CC) -D$(MACH) -Dopus -O -c -DBOOT $(LIB2)/low.c

sash.o: sash.c
	$(PFX)$(CC) -D$(MACH) -Dopus -O -c -DSASH sash.c

boot.o: boot.c
	$(PFX)$(CC) -D$(MACH) -Dopus -O -c boot.c

asm.pm100.o: $(HFILES) asm.pm100.h asm.pm100.s
	$(CPP) -D$(MACH) -I$(INCRT) < asm.pm100.s > asm.pm100.i
	$(PFX)as -o asm.pm100.o asm.pm100.i
	rm -f asm.pm100.i

sasm.pm100.o: $(HFILES) asm.pm100.h asm.pm100.s
	$(CPP) -D$(MACH) -I$(INCRT) -DOPSASH < asm.pm100.s > asm.pm100.i
	$(PFX)as -o sasm.pm100.o asm.pm100.i
	rm -f asm.pm100.i

smon.o:	$(HFILES) $(LIB2)/Amon.c
	cp $(LIB2)/Amon.c smon.c
	$(PFX)$(CC) -D$(MACH) -O -c -DSASH smon.c
	rm -f smon.c

low.pm100.o: $(HFILES) $(LIB2)/low.pm100.c
	$(PFX)$(CC) -D$(MACH) -O -c -DBOOT $(LIB2)/low.pm100.c

Ainit.o: $(HFILES) $(LIB2)/Ainit.c
	$(PFX)$(CC) -D$(MACH) -O -c -DSASH $(LIB2)/Ainit.c

Aconf.o: $(HFILES) $(LIB2)/Aconf.c
	$(PFX)$(CC) -D$(MACH) -O -c -DSASH $(LIB2)/Aconf.c

OPMEMdk.o: $(HFILES) $(LIB2)/OPMEMdk.c
	$(PFX)$(CC) -D$(MACH) -O -c -DSASH $(LIB2)/OPMEMdk.c

open.o: $(HFILES) $(LIB2)/open.c
	$(PFX)$(CC) -D$(MACH) -O -c -DSASH $(LIB2)/open.c

close.o: $(HFILES) $(LIB2)/close.c
	$(PFX)$(CC) -D$(MACH) -O -c -DSASH $(LIB2)/close.c

read.o: $(HFILES) $(LIB2)/read.c
	$(PFX)$(CC) -D$(MACH) -O -c -DSASH $(LIB2)/read.c

write.o: $(HFILES) $(LIB2)/write.c
	$(PFX)$(CC) -D$(MACH) -O -c -DSASH $(LIB2)/write.c

fstat.o: $(HFILES) $(LIB2)/fstat.c
	$(PFX)$(CC) -D$(MACH) -O -c -DSASH $(LIB2)/fstat.c

lseek.o: $(HFILES) $(LIB2)/lseek.c
	$(PFX)$(CC) -D$(MACH) -O -c -DSASH $(LIB2)/lseek.c

sboot.o: $(HFILES) boot.c
	cp boot.c sboot.c
	$(PFX)$(CC) -D$(MACH) -O -c -DSASH sboot.c
	rm -f sboot.c

install: all
	-mkdir /stand
	cpset opdiag /stand 0755 bin bin
	cpset opsash /stand 0755 bin bin

clean:
	rm -f *.o *.i smon.c sboot.c

clobber: clean
	rm -f opdiag opsash 
