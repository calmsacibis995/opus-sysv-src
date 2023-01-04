h39329
s 00069/00000/00000
d D 1.1 90/03/06 12:47:52 root 1 0
c date and time created 90/03/06 12:47:52 by root
e
u
U
t
T
I 1
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)kern-port:ml/ml.mk	6.3"

LIBNAME = ../$(LIBDIR)/lib.ml
INCRT = ..
#INCRT = /usr/include
CFLAGS = -I$(INCRT) $(BRDEFS) -D$(CPU)
FRC =

MFILES = psl.m equates.m

SFILES = trap.s mmb.s paging.s start.s cswitch.s userio.s math.s \
         misc.s end.s copy16.s

FILES = $(LIBNAME)(locore.o)

all:	pickup $(LIBNAME) 
	-@for i in * ; \
	do \
		if [ -d $$i -a -f $$i/$$i.mk ] ; then \
			cd $$i; \
			echo "	$(MAKE) -f $$i.mk \"MAKE=$(MAKE)\" \"CPU=$(CPU)\" \"AR=$(AR)\" \"CC=$(CC)\" \"FRC=$(FRC)\" \"INCRT=$(INCRT)\" \"BRDEFS=$(BRDEFS)\" \"DASHO=$(DASHO)\" "; \
			$(MAKE) -f $$i.mk "MAKE=$(MAKE)" "CPU=$(CPU)" "AR=$(AR)" "CC=$(CC)" "FRC=$(FRC)" "INCRT=$(INCRT)" "BRDEFS=$(BRDEFS)" "DASHO=$(DASHO)" ; \
			cd ..; \
		fi ; \
	done


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

locore.s:	$(MFILES) $(SFILES) $(FRC)
#locore.s:FRC
	cat $(MFILES) $(SFILES) > locore.s


clean:
	-rm -f locore.s *.o

clobber:	clean
	-rm -f $(LIBNAME)

FRC:
E 1
