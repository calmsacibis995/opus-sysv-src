#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)sccs:sccs.mk	6.3"
ROOT =
BIN = $(ROOT)/usr/bin
HELPLIB = $(ROOT)/usr/lib/help
CFLAGS= -O
ARGS =

all: libs cmds helplib
	@echo "SCCS is built"

libs:
	cd lib; $(MAKE) -e -f lib.mk "CC=/usr/bin/cc -g"

cmds:
	cd cmd; $(MAKE) -e -f cmd.mk $(ARGS) "CC=/usr/bin/cc -g"

helplib:
	cd help.d; $(MAKE) -e -f help.mk

install:
	cd lib; $(MAKE) -e -f lib.mk install
	cd cmd; $(MAKE) -e -f cmd.mk $(ARGS) BIN=$(BIN) install
	cd help.d; $(MAKE) -e -f help.mk HELPLIB=$(HELPLIB) install

clean:
	cd lib; $(MAKE) -e -f lib.mk clean
	cd cmd; $(MAKE) -e -f cmd.mk clean
	cd help.d; $(MAKE) -e -f help.mk clean

clobber:
	cd lib; $(MAKE) -e -f lib.mk clobber
	cd cmd; $(MAKE) -e -f cmd.mk clobber BIN=$(BIN)
	cd help.d; $(MAKE) -e -f help.mk clobber HELPLIB=$(HELPLIB)
