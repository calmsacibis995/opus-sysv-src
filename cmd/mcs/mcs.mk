#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)mcs:mcs.mk	1.3"
ROOT=
FRC =
INSDIR = $(ROOT)/usr/bin 
LDFLAGS = -s
PCC=/usr/bin/cc

all:	mcs

mcs:	mcs.c $(FRC)
	-if clipper && ghs ; then \
		$(PCC) $(CFLAGS) $(LDFLAGS) -o mcs mcs.c ; \
	else \
		$(CC) $(CFLAGS) $(LDFLAGS) -o mcs mcs.c ; \
	fi

install:	mcs
	cpset mcs $(INSDIR)

clean:
	-rm -f *.o

clobber:	clean
	-rm -f mcs

lint:
	lint $(CFLAGS) mcs.c

FRC:

