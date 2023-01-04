#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)edittbl:edittbl.mk	1.6"
TITLE = edittbl
BOOTDIR=/boot

all: edittbl

edittbl: $(FRC)
	cp edittbl.sh edittbl

install: all
	cp edittbl $(ROOT)/etc/edittbl
	$(CH)chmod 755 $(ROOT)/etc/edittbl
	$(CH)chgrp 2 $(ROOT)/etc/edittbl
	$(CH)chown 2 $(ROOT)/etc/edittbl
	-mkdir $(BOOTDIR)
	cp mondev.c $(BOOTDIR)/opmon

clobber:
	rm -f edittbl

clean:

FRC:
