#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)nlsadmin:nlsadmin.mk	1.5"

ROOT =
TESTDIR = .
INSDIR = $(ROOT)/usr/bin
INC = $(ROOT)/usr/include
INS = :
CFLAGS = -O -US4 -I$(INC)
LDFLAGS = -s
SOURCE = main.c util.c nlsadmin.h nlsaddr.c nstoa.c nlsstr.c
OBJECTS = main.o util.o nstoa.o nlsaddr.o nlsstr.o
FRC =

all: nlsadmin

nlsadmin: $(OBJECTS)
	if clipper; then \
	$(CC) $(CFLAGS) -Uu3b2 -Dclipper -o $(TESTDIR)/nlsadmin $(LDFLAGS) \
		 $(OBJECTS); \
	elif ns32000; then \
	$(CC) $(CFLAGS) -Uu3b2 -Dns32000 -o $(TESTDIR)/nlsadmin $(LDFLAGS) \
		 $(OBJECTS); \
	else \
	$(CC) $(CFLAGS) -o $(TESTDIR)/nlsadmin $(LDFLAGS) $(OBJECTS); \
	fi

install: all
	install -n $(INSDIR) $(TESTDIR)/nlsadmin

clean:
	rm -f $(OBJECTS)

clobber: clean
	rm -f $(TESTDIR)/nlsadmin
FRC:


main.o: $(INC)/sys/types.h \
	$(INC)/sys/stat.h \
	$(INC)/dirent.h \
	$(INC)/stdio.h \
	$(INC)/ctype.h \
	$(INC)/signal.h \
	$(INC)/string.h \
	$(INC)/fcntl.h \
	$(INC)/errno.h \
	nlsadmin.h 
util.o: $(INC)/stdio.h \
	$(INC)/ctype.h \
	$(INC)/string.h \
	$(INC)/fcntl.h \
	nlsadmin.h
nlsaddr.o: $(INC)/stdio.h \
	$(INC)/ctype.h
nstoa.o:$(INC)/stdio.h \
	$(INC)/memory.h \
	$(INC)/malloc.h \
	$(INC)/ctype.h \
	nsaddr.h
