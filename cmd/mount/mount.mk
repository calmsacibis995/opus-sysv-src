#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident "@(#)mount:mount.mk	1.10"
ROOT =
TESTDIR = .
INSDIR = $(ROOT)/etc
INCRT = $(ROOT)/usr/include
INS = :
CFLAGS = -O -s
NFSLIBS = -lrpc -lrpcsvc -lyp -lrpc -lsocket
#LDFLAGS = -lns -lnsl_s -lcrypt $(LDLIBS)
LDFLAGS = -lns -lnsl -lcrypt $(LDLIBS) $(NFSLIBS)
FRC =

all: mount

mount: mount.c 
	$(CC) -I$(INCRT) $(CFLAGS) -o $(TESTDIR)/mount mount.c

install: all
	install -n $(INSDIR) $(TESTDIR)/mount

clean:
	rm -f *.o

clobber: clean
	rm -f $(TESTDIR)/mount
FRC:
