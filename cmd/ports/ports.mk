#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)ports:ports.mk	1.6"
TITLE = ports
BOOTDIR=/boot

all: ports

ports: $(FRC)
	cp ports.sh ports

install: all
	cp ports $(ROOT)/etc/ports
	$(CH)chmod 555 $(ROOT)/etc/ports
	$(CH)chgrp 2 $(ROOT)/etc/ports
	$(CH)chown 2 $(ROOT)/etc/ports

clobber:
	rm -f ports

clean:

FRC:
