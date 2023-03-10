#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)m4:m4.mk	1.4"
#	@(#)m4:m4.mk	1.4
ROOT=
INC=$(ROOT)/usr/include
UBIN=$(ROOT)/usr/bin
TESTDIR = .
YACCRM=-rm
FRC =
INS = install
INSDIR = $(UBIN)
CFLAGS = -O -I$(INC)

all: m4

m4:	m4.o m4ext.o m4macs.o m4y.o
	$(CC) $(LDFLAGS) -s -o $(TESTDIR)/m4 m4.o m4ext.o m4macs.o m4y.o -ly

m4.o:	m4.c $(FRC)
m4ext.o: m4ext.c $(FRC)
m4macs.o: m4macs.c $(FRC)
m4y.o:	m4y.y $(FRC)

test:
	rtest $(TESTDIR)/m4

install: all
	$(INS) -f $(INSDIR) $(TESTDIR)/m4

clean:
	-rm -f *.o
	$(YACCRM) -f y.tab.c 

clobber: clean
	-rm -f $(TESTDIR)/m4

FRC:
