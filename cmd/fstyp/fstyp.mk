#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)fstyp:fstyp.mk	1.3.1.1"
#
#	Makefile for the fstyp component
#

ROOT =
DIR1 = $(ROOT)/etc
DIR2 = $(ROOT)/etc/fstyp.d


INC = $(ROOT)/usr/include
LDFLAGS =
LDLIBS =
CFLAGS = -O -s -I$(INC)
STRIP = strip

OBJECTS =  S51Kfstyp.o S52Kfstyp.o s5fstyp.o

#all: fstyp S51Kfstyp S52Kfstyp
all: fstyp s5fstyp

S51Kfstyp:  S51Kfstyp.o
	$(CC) $(CFLAGS)  -o S51Kfstyp  S51Kfstyp.o  $(LDFLAGS) $(LDLIBS)

S52Kfstyp:  S52Kfstyp.o
	$(CC) $(CFLAGS)  -o S52Kfstyp  S52Kfstyp.o  $(LDFLAGS) $(LDLIBS)

S5fstyp:  S5fstyp.o
	$(CC) $(CFLAGS)  -o S5fstyp  S5fstyp.o  $(LDFLAGS) $(LDLIBS)


S51Kfstyp.o:	 $(INC)/stdio.h $(INC)/fcntl.h \
		 $(INC)/sys/types.h $(INC)/sys/stat.h \
		 $(INC)/sys/fstyp.h $(INC)/sys/ustat.h \
		 $(INC)/sys/ino.h	$(INC)/sys/param.h \
		 $(INC)/sys/inode.h	$(INC)/sys/fs/s5dir.h \
		 $(INC)/sys/fs/s5param.h $(INC)/sys/fs/s5filsys.h

S52Kfstyp.o:	 $(INC)/stdio.h $(INC)/fcntl.h \
		 $(INC)/sys/types.h $(INC)/sys/stat.h \
		 $(INC)/sys/fstyp.h $(INC)/sys/ustat.h \
		 $(INC)/sys/ino.h	$(INC)/sys/param.h \
		 $(INC)/sys/inode.h	$(INC)/sys/fs/s5dir.h \
		 $(INC)/sys/fs/s5param.h $(INC)/sys/fs/s5filsys.h

S5fstyp.o:	 $(INC)/stdio.h $(INC)/fcntl.h \
		 $(INC)/sys/types.h $(INC)/sys/stat.h \
		 $(INC)/sys/fstyp.h $(INC)/sys/ustat.h \
		 $(INC)/sys/ino.h	$(INC)/sys/param.h \
		 $(INC)/sys/inode.h	$(INC)/sys/fs/s5dir.h \
		 $(INC)/sys/fs/s5param.h $(INC)/sys/fs/s5filsys.h

clean:
	rm -f $(OBJECTS)

clobber: clean
	rm -f fstyp S51Kfstyp S52Kfstyp s5fstyp

install: all dir
	if ns32000 || clipper || m88k; \
	then \
	cpset s5fstyp $(DIR2)/s5fstyp 00555 root sys; \
	install -f $(DIR1) -m 00555 -u root -g sys fstyp; \
	else \
	install -f $(DIR1) -m 00555 -u root -g sys fstyp; \
	install -f $(DIR2) -m 00555 -u root -g sys S51Kfstyp; \
	install -f $(DIR2) -m 00555 -u root -g sys S52Kfstyp; \
	fi

dir:
	if [ ! -d $(DIR2) ] ;\
	then \
		mkdir $(DIR2) ;\
	fi

strip: all
	$(STRIP) S51Kfstyp S52Kfstyp s5fstyp
