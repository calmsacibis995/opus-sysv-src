#
#	Copyright (c) 1987 Motorola Inc
#	All Rights Reserved
# 	@(#)io.mk	10.12
# 
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)kern-port:io/io.mk	10.2"
ROOT =
INCRT = ..
LIBNAME = $(LIBDIR)/lib.io

DASHG =
DASHO = -O
PFLAGS = $(DASHG) -I$(INCRT) -DINKERNEL 
CFLAGS = $(DASHO) $(PFLAGS) $(BRDEFS) -D$(CPU)
FRC =
OWN = bin
GRP = bin

DFILES = \
	$(LIBNAME)(as.o) \
	$(LIBNAME)(clist.o) \
	$(LIBNAME)(clk.o) \
	$(LIBNAME)(cons.o) \
	$(LIBNAME)(cpt.o) \
	$(LIBNAME)(dk.o) \
	$(LIBNAME)(err.o) \
	$(LIBNAME)(fpu881a.o) \
	$(LIBNAME)(gentty.o) \
	$(LIBNAME)(gn.o) \
	$(LIBNAME)(ipc.o) \
	$(LIBNAME)(lp.o) \
	$(LIBNAME)(mem.o) \
	$(LIBNAME)(msg.o) \
	$(LIBNAME)(mt.o) \
	$(LIBNAME)(net.o) \
	$(LIBNAME)(opus.o) \
	$(LIBNAME)(osm.o) \
	$(LIBNAME)(partab.o) \
	$(LIBNAME)(pm400dos.o) \
	$(LIBNAME)(pm400pc.o) \
	$(LIBNAME)(prf.o) \
	$(LIBNAME)(pty.o) \
	$(LIBNAME)(ram.o) \
	$(LIBNAME)(sem.o) \
	$(LIBNAME)(shm.o) \
	$(LIBNAME)(stubs.o) \
	$(LIBNAME)(subr_log.o) \
	$(LIBNAME)(sxt.o) \
	$(LIBNAME)(syms.o) \
	$(LIBNAME)(tpoke00.o) \
	$(LIBNAME)(tt1.o) \
	$(LIBNAME)(tty.o)

all:	subdirs pickup $(LIBNAME)

.PRECIOUS:	$(LIBNAME)

pickup:
	-@if [ "`ls *.o 2>/dev/null`" ] ; \
	then \
		echo "\t$(AR) ruv $(LIBNAME) *.o" ; \
		$(AR) ruv $(LIBNAME) *.o ; \
		echo "\trm -f *.o" ; \
		rm -f *.o  ; \
	fi

subdirs:
	-@for i in * ; \
	do \
		if [ -d $$i -a -f $$i/$$i.mk ] ; then \
			cd $$i; \
			echo "	$(MAKE) -f $$i.mk \"MAKE=$(MAKE)\" \"CPU=$(CPU)\" \"AR=$(AR)\" \"CC=$(CC)\" \"FRC=$(FRC)\" \"INCRT=$(INCRT)\" \"BRDEFS=$(BRDEFS)\" \"DASHO=$(DASHO)\" "; \
			$(MAKE) -f $$i.mk "MAKE=$(MAKE)" "CPU=$(CPU)" "AR=$(AR)" "CC=$(CC)" "FRC=$(FRC)" "INCRT=$(INCRT)" "BRDEFS=$(BRDEFS)" "DASHO=$(DASHO)" ; \
			cd ..; \
		fi ; \
	done

$(LIBNAME):	$(DFILES)
	$(AR) ruv $(LIBNAME) *.o
	rm -f *.o

.c.a:
	$(CC) $(CFLAGS) -c $<

clean:
	-@for i in * ; \
	do \
		if [ -d $$i -a -f $$i/$$i.mk ] ; then \
			cd $$i; \
			echo "	$(MAKE) -f $$i clean"; \
			$(MAKE) -f $$i.mk clean; \
			cd ..; \
		fi ; \
	done
	-rm -f *.o

clobber:
	-@for i in * ; \
	do \
		if [ -d $$i -a -f $$i/$$i.mk ] ; then \
			cd $$i; \
			echo "	$(MAKE) -f $$i clobber"; \
			$(MAKE) -f $$i.mk clobber; \
			cd ..; \
		fi ; \
	done
	-rm -f *.o z8530.c
	-rm -f $(LIBNAME)

FRC:
