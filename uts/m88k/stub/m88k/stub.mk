#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)kern-port:stub/stub.mk	6.3"

LIBNAME = ../$(LIBDIR)/lib.stub
INCRT = ..
#INCRT = /usr/include
CFLAGS = -I$(INCRT) $(BRDEFS) -D$(CPU)
FRC =

MFILES = ../../ml/m88k/psl.m ../../ml/m88k/equates.m

#SFILES = stream.s du.s
SFILES = du.s

FILES = $(LIBNAME)(stub.o)

all:	pickup $(LIBNAME) 

.PRECIOUS:	$(LIBNAME)

pickup:
	-@if [ "`ls *.o 2>/dev/null`" ] ; \
	then \
		echo "\t$(AR) ruv $(LIBNAME) *.o" ; \
		$(AR) ruv $(LIBNAME) *.o ; \
		echo "\trm -f *.o" ; \
		rm -f *.o  ; \
	fi

$(LIBNAME):	$(FILES)
	$(AR) ruv $(LIBNAME) *.o
	rm -f *.o

.s.a:
	$(CC) $(CFLAGS) -P $<
	mv $*.i temp.s
	$(CC) $(CFLAGS) -c temp.s
	mv temp.o $*.o
	rm -f temp.s

stub.s:	$(MFILES) $(SFILES) $(FRC)
	cat $(MFILES) $(SFILES) > stub.s

clean:
	-rm -f stub.s *.o

clobber:	clean
	-rm -f $(LIBNAME)

FRC:
