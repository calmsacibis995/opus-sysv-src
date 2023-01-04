#	Copyright (c) 1984 Fairchild Semiconductor Corp.
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF
#	FAIRCHILD SEMICONDUCTOR CORPORATION
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)ramdisk:ramdisk.mk	1.1 (Fairchild) 1/9/87"

ROOT =
INSDIR = $(ROOT)/etc
CFLAGS = -O
LDFLAGS = -s

all: 
	if clipper; then make -f ramdisk.mk ramdisk; fi;

ramdisk: ramdisk.o
	$(CC) $(LDFLAGS) -o $@ ramdisk.o $(LDLIBS)

install: all
	if clipper;\
	then install -f $(INSDIR) ramdisk;\
	fi;

clean:
	rm -f ramdisk.o

clobber: clean
	rm -f ramdisk
