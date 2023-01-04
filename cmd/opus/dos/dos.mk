# SID @(#)dos.mk	1.7

FRC =
INS = /etc/install
INSDIR = ${ROOT}/opus/bin
CFLAGS = -O
LDFLAGS = -s
SYS =

OFILES = opdos.o

LOFILES = doslib.o disklib.o

all:	libopus.a opdos
	
libopus.a: $(LOFILES)
	rm -f libopus.a
	$(AR) -r libopus.a $(LOFILES)

opdos: $(OFILES) libopus.a
	$(CC) $(LDFLAGS) $(OFILES) libopus.a -o opdos

doslib.o:
	if ns32332; then \
		$(CC) -c -Dpm200 doslib.c; \
	elif ns32000; then \
		$(CC) -c -Dpm100 doslib.c; \
	elif clipper; then \
		$(CC) -c -Dpm300 doslib.c; \
	elif m88k; then \
		$(CC) -c -Dpm400 doslib.c; \
	fi

install: libopus.a opdos
	-mkdir $(INSDIR)
	-chmod 755 $(INSDIR)
	-chown root $(INSDIR)
	-chgrp sys $(INSDIR)
	$(INS) -f $(ROOT)/usr/lib -m 664 -u bin -g bin libopus.a
	$(INS) -f $(INSDIR) -m 755 -u root -g sys opdos
	$(INS) -f $(INSDIR) -m 755 -u root -g sys dodos
	$(INS) -f $(INSDIR) -m 755 -u root -g sys dos
	$(INS) -f $(INSDIR) -m 755 -u root -g sys ducp
	$(INS) -f $(INSDIR) -m 755 -u root -g sys udcp

clean:
	-rm -f *.o

clobber: clean
	-rm -f opdos
	-rm -f libopus.a

FRC:
