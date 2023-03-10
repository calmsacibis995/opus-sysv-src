#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)setclk:setclk.mk	1.3.1.1"
#
#		Copyright 1984 AT&T
#

ROOT =
INCRT = $(ROOT)/usr/include
CFLAGS = -O -I$(INCRT) -Uu3b -Uvax -Updp11 -Uu3b15 -Du3b2
LDFLAGS = -s
MAKE = make "AS=$(AS)" "CC=$(CC)" "LD=$(LD)"
INS = install
FRC =

all:	setclk

install: all
	@#$(INS) -n $(ROOT)/etc setclk

setclk:
	@echo "****************************"
	@echo "'setclk' not built for opus."
	@echo "****************************"
	@#$(CC) $(CFLAGS) $(LDFLAGS) -o setclk setclk.c $(LDLIBS)

clean:
	-rm -f setclk.o

clobber: clean
	-rm -f setclk

FRC:

#
# Header dependencies
#

setclk: setclk.c \
	$(INCRT)/stdio.h \
	$(INCRT)/sys/firmware.h \
	$(INCRT)/sys/immu.h \
	$(INCRT)/sys/nvram.h \
	$(INCRT)/sys/sbd.h \
	$(INCRT)/sys/sys3b.h \
	$(INCRT)/sys/types.h \
	$(INCRT)/time.h \
	$(FRC)
