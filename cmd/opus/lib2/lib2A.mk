#	@(#)lib2A.mk	1.2
CC = $(PFX)scc -I. -DSTAND
AR = $(PFX)ar
MACH =

all:	Aconf.o Ainit.o Amon.o
	$(AR) rvc lib2A.a Aconf.o Ainit.o Amon.o

install: all
	/etc/install -f $(ROOT)/usr/lib lib2A.a

Amon.o: Amon.c
	$(CC) -Dopus -D$(MACH) -O -c Amon.c

Aconf.o: Aconf.c low.c
	$(CC) -Dopus -D$(MACH) -O -c Aconf.c

Ainit.o: Ainit.c
	$(CC) -Dopus -D$(MACH) -O -c Ainit.c

clean:
	rm -f Aconf.o Ainit.o

clobber: clean
	rm -f lib2A.a
