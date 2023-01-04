h12270
s 00048/00000/00000
d D 1.1 90/03/06 12:40:45 root 1 0
c date and time created 90/03/06 12:40:45 by root
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

#ident	"@(#)kern-port:fs/fs.mk	10.3"
STRIPOPT = -x -r

INCRT =..
CFLAGS = -I$(INCRT) -DINKERNEL 
FRC =

all:
	-@for i in * ; \
	do \
		if [ -d $$i -a -f $$i/$$i.mk ] ; then \
			cd $$i; \
			echo "	$(MAKE) -f $$i.mk \"MAKE=$(MAKE)\" \"CPU=$(CPU)\" \"AR=$(AR)\" \"CC=$(CC)\" \"FRC=$(FRC)\" \"INCRT=$(INCRT)\" \"BRDEFS=$(BRDEFS)\" \"DASHO=$(DASHO)\" "; \
			$(MAKE) -f $$i.mk "MAKE=$(MAKE)" "CPU=$(CPU)" "AR=$(AR)" "CC=$(CC)" "FRC=$(FRC)" "INCRT=$(INCRT)" "BRDEFS=$(BRDEFS)" "DASHO=$(DASHO)" ; \
			cd ..; \
		fi ; \
	done

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

FRC:
E 1
