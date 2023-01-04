#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)mkunix:mkunix.mk	1.5"
#
#		Copyright 1984 AT&T
#
#

ROOT =
INCRT = $(ROOT)/usr/include
INS = install
INSDIR = $(ROOT)/etc
FRC =

all: mkunix mkbootdir udate

install: mkunix mkbootdir udate
	$(INS) -n $(INSDIR) mkunix
	$(INS) -n $(INSDIR) mkkernel
	chown root udate
	chgrp bin udate
	chmod 4555 udate
	cp udate $(ROOT)/opus/bin

mkunix:
	cp mkunix.sh mkunix
	-if ns32332; \
	then cp mkkernel.pm200 mkkernel; \
	elif ns32000; \
	then cp mkkernel.pm100 mkkernel; \
	elif clipper; \
	then cp mkkernel.pm300 mkkernel; \
	fi

mkbootdir:
	-if ns32332; \
	then make -b -f mkKLIB.pm200 CPU=pm200 TYPE=ns32000 all; \
	elif ns32000; \
	then make -b -f mkKLIB.pm100 CPU=pm100 TYPE=ns32000 all; \
	elif clipper; \
	then make -b -f mkKLIB.pm300 CPU=pm300 TYPE=clipper all; \
	fi

udate:
	$(CC) $(CFLAGS) -o udate udate.c

clobber: clean

clean:
	rm -f mkunix
	rm -f mkkernel
	rm -f udate
	rm -rf tmp
	-if ns32000; \
	then make -b -f mkKLIB.pm100 CPU=pm100 TYPE=ns32000 clobber; \
	elif ns32332; \
	then make -b -f mkKLIB.pm200 CPU=pm200 TYPE=ns32000 clobber; \
	elif clipper; \
	then make -b -f mkKLIB.pm300 CPU=pm300 TYPE=clipper clobber; \
	fi

