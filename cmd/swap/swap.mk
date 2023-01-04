#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)swap:swap.mk	1.4"
#
#		Copyright 1985 AT&T
#

ROOT =
INCRT = $(ROOT)/usr/include
CFLAGS = -O -I$(INCRT)
LDFLAGS = -s
INS = install

all:	swap

install: all
	$(INS) -f $(ROOT)/etc swap

swap:
	-if ns32332; then \
		$(CC) $(CFLAGS) -Dpm200 $(LDFLAGS) -o swap swap.c $(LDLIBS); \
	elif ns32000; then \
		$(CC) $(CFLAGS) -Dpm100 $(LDFLAGS) -o swap swap.c $(LDLIBS); \
	elif clipper; then \
		$(CC) $(CFLAGS) -Dpm300 $(LDFLAGS) -o swap swap.c $(LDLIBS); \
	elif m88k; then \
		$(CC) $(CFLAGS) -Dpm400 $(LDFLAGS) -o swap swap.c $(LDLIBS); \
	fi

clean:
	-rm -f swap.o

clobber: clean
	-rm -f swap

FRC:

#
# Header dependencies
#

swap:	swap.c \
	$(INCRT)/stdio.h \
	$(INCRT)/fcntl.h \
	$(INCRT)/sys/types.h \
	$(INCRT)/sys/param.h \
	$(INCRT)/sys/dir.h \
	$(INCRT)/sys/map.h \
	$(INCRT)/sys/swap.h \
	$(INCRT)/sys/immu.h \
	$(INCRT)/sys/stat.h \
	$(FRC)
