#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#	Copyright (c) 1986 by National Semiconductor Corp.
#	All Rights Reserved

#ident	"@(#)bbh.mk	1.4	10/30/86 Copyright (c) 1986 by National Semiconductor Corp."

INCRT = $(ROOT)/usr/include
#CFLAGS = -O -I$(INCRT) -Uu3b -Uvax -Updp11 -Uu3b5 -Uu3b2 -Ddb332
CFLAGS = -O -I$(INCRT)
LDFLAGS = -s
MAKE = make "AS=$(AS)" "CC=$(CC)" "LD=$(LD)"
INS = install

#all:	hdeadd hdefix hdelogger hdeupdate
all:	opus

opus:	
	@echo "*************************"
	@echo "'bbh' not built for opus."
	@echo "*************************"

hdeadd:	dconv.o edio.o
	$$(CC) $(CFLAGS) $(LDFLAGS) -o hdeadd hdeadd.c dconv.o edio.o $(LDLIBS)

hdefix:	edio.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o hdefix hdefix.c edio.o $(LDLIBS)

hdelogger: edio.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o hdelogger hdelogger.c edio.o $(LDLIBS)

hdeupdate: edio.o hdedoscan.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o hdeupdate hdeupdate.c edio.o \
		hdedoscan.o $(LDLIBS)

install: all
	@#$(INS) -n $(ROOT)/etc hdeadd
	@#$(INS) -n $(ROOT)/etc hdefix
	@#$(INS) -o -f $(ROOT)/etc hdelogger
	@#$(INS) -n $(ROOT)/etc hdeupdate

clean:
	-rm -f hdeadd.o hdefix.o hdelogger.o edio.o dconv.o \
	hdedoscan.o hdeupdate.o

clobber: clean
	-rm -f hdeadd hdefix hdelogger hdeupdate

FRC:

#
# Header dependencies
#

hdeadd: hdeadd.c \
	$(INCRT)/fcntl.h \
	$(INCRT)/stdio.h \
	$(INCRT)/sys/hdeioctl.h \
	$(INCRT)/sys/hdelog.h \
	$(INCRT)/sys/stat.h \
	$(INCRT)/sys/sysmacros.h \
	$(INCRT)/sys/types.h \
	$(INCRT)/sys/disk.h \
	dconv.h \
	edio.h \
	hdecmds.h \
	$(FRC)

hdefix: hdefix.c \
	$(INCRT)/fcntl.h \
	$(INCRT)/stdio.h \
	$(INCRT)/sys/filsys.h \
	$(INCRT)/sys/hdeioctl.h \
	$(INCRT)/sys/hdelog.h \
	$(INCRT)/sys/param.h \
	$(INCRT)/sys/signal.h \
	$(INCRT)/sys/stat.h \
	$(INCRT)/sys/sysmacros.h \
	$(INCRT)/sys/types.h \
	$(INCRT)/sys/uadmin.h \
	$(INCRT)/sys/disk.h \
	$(INCRT)/utmp.h \
	edio.h \
	hdecmds.h \
	$(FRC)

hdelogger: hdelogger.c \
	$(INCRT)/stdio.h \
	$(INCRT)/sys/hdeioctl.h \
	$(INCRT)/sys/hdelog.h \
	$(INCRT)/sys/signal.h \
	$(INCRT)/sys/stat.h \
	$(INCRT)/sys/sysmacros.h \
	$(INCRT)/sys/types.h \
	$(INCRT)/sys/disk.h \
	$(INCRT)/time.h \
	edio.h \
	hdecmds.h \
	$(FRC)

edio.o: edio.c \
	$(INCRT)/errno.h \
	$(INCRT)/fcntl.h \
	$(INCRT)/stdio.h \
	$(INCRT)/sys/errno.h \
	$(INCRT)/sys/hdeioctl.h \
	$(INCRT)/sys/hdelog.h \
	$(INCRT)/sys/sysmacros.h \
	$(INCRT)/sys/types.h \
	$(INCRT)/sys/disk.h \
	edio.h \
	hdecmds.h \
	$(FRC)

dconv.o: dconv.c \
	$(INCRT)/time.h \
	$(FRC)

hdeupdate: hdeupdate.c \
	$(INCRT)/fcntl.h \
	$(INCRT)/stdio.h \
	$(INCRT)/sys/hdeioctl.h \
	$(INCRT)/sys/hdelog.h \
	$(INCRT)/sys/stat.h \
	$(INCRT)/sys/sysmacros.h \
	$(INCRT)/sys/types.h \
	$(INCRT)/sys/disk.h \
	edio.h \
	hdecmds.h \
	$(FRC)

hdedoscan.o: hdedoscan.c \
	$(INCRT)/stdio.h \
	$(INCRT)/ctype.h \
	$(INCRT)/varargs.h \
	$(INCRT)/values.h \
	$(INCRT)/errno.h \
	$(FRC)
