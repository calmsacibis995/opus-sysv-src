#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)ps:ps.mk	1.7"
#	ps make file
ROOT=
INSDIR = $(ROOT)/bin
LDFLAGS = -O -s -lgen $(LDLIBS)
INS=:

ps:
	-if clipper ; then \
		echo making pm300 ps; \
		$(CC) -o ps ps.c $(LDFLAGS); \
	elif ns32332 ; then \
		echo making pm200 ps; \
		$(CC) -o ps ps.c -Dpm200 $(LDFLAGS); \
	elif ns32000 ; then \
		echo making pm100 ps; \
		$(CC) -o ps ps.c -Dpm100 $(LDFLAGS); \
	elif m88k ; then \
		echo making pm400 ps; \
		$(CC) -o ps ps.c -Dpm400 $(LDFLAGS); \
	fi;
	$(INS) $(INSDIR) -m 2755 -u root -g sys ps

all:	install clobber

install:
	$(MAKE) -f ps.mk INS="install -f"

clean:
	-rm -f ps.o

clobber: clean
	rm -f ps
