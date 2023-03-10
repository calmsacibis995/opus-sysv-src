#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)make:make.mk	1.8.1.1"

# The rules.c file can be modified locally for people who still like
#	things like fortran.

TESTDIR = .
YACCRM = rm -f
#LDFLAGS = -i -n -s
LDFLAGS = -n -s
INS = install -o
INSDIR = $(ROOT)/bin
LIBS = -lld

OBJECTS =  \
	main.o \
	doname.o \
	misc.o \
	files.o \
	rules.o \
	dosys.o \
	gram.o \
	dyndep.o \
	prtmem.o

CFLAGS = -O -Dexit=mkexit

all:  make

make:  $(OBJECTS)
	if pdp11 ; then \
		$(CC) -o $(TESTDIR)/make $(LDFLAGS) $(OBJECTS) ; \
	else \
		$(CC) -o $(TESTDIR)/make $(LDFLAGS) $(OBJECTS) $(LIBS) ; \
	fi

gram.c:	gram.y

gram.o: gram.c

$(OBJECTS):  defs

install: all
	$(INS) -n $(INSDIR) $(TESTDIR)/make

clean:
	-rm -f *.o
	$(YACCRM) gram.c

clobber: clean
	-rm -f $(TESTDIR)/make

.DEFAULT:
	$(GET) $(GFLAGS) -p s.$< > $<
