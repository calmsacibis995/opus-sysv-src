#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident "$Header: Makefile 2.8 88/04/26 10:03:49 root Exp $ RISC Source Base"
#
#	M88000 CC MAKEFILE
#
#
TARGET=m88k
SGS=
OWN=bin
GRP=bin
CC=/bin/cc
# -O croaks the Sun optimizer under SunOS 3.2
OFLAG=-O
CFLAGS= $(OFLAG) $(FFLAG) -DTARGET=$(TARGET)
LDLIBS =
ARCH=AR32W
DEFLIST=-D$(ARCH) -DM88K
INCLIST=-I$(M88KINC) -I$(USRINC)
SGSENV=
FFLAG=
CC_CMD=$(CC) $(CFLAGS) $(DEFLIST) $(INCLIST) $(SGSENV)
#
STRIP=strip
SPFLAGS=
STRIP_CMD=$(STRIP) $(SPFLAGS)
#
LINT=lint
LINTFLAGS=-p
LINT_CMD=$(LINT) $(LINTFLAGS)
#
#	External Directories
#
ROOT=
VERSION=
USRINC= $(ROOT)/usr/$(VERSION)include
#
#	Internal Directories
#
BASE= ../..
BINDIR= $(BASE)/xenv/m88k/bin
LIBDIR= $(BASE)/xenv/m88k/lib
M88KINC= $(BASE)/inc/m88k
COMCMD= $(BASE)/cmd/m88k
#
all:		$(SGS)cc
#
$(SGS)cc:	$(M88KINC)/paths.h $(M88KINC)/sgs.h $(COMCMD)/cc.c
		$(CC_CMD) -o $@ $(COMCMD)/cc.c $(LDLIBS)
#
save:		$(BINDIR)/$(SGS)cc
	-rm -f $(BINDIR)/$(SGS)cc.back
	cp $(BINDIR)/$(SGS)cc $(BINDIR)/$(SGS)cc.back
#
install:	$(BINDIR)/$(SGS)cc
#
$(BINDIR)/$(SGS)cc:	$(SGS)cc
		sh $(BASE)/sgs.install 755 $(OWN) $(GRP) $(BINDIR)/$? $?
#
$(BINDIR)/gencc:	$(COMCMD)/gencc
			sh $(BASE)/sgs.install 755 $(OWN) $(GRP) $@ $?
#
uninstall:	$(BINDIR)/$(SGS)cc.back
	-rm -f $(BINDIR)/$(SGS)cc
	cp $(BINDIR)/$(SGS)cc.back $(BINDIR)/$(SGS)cc
#
shrink:		clean
#
lint:		$(M88KINC)/paths.h $(M88KINC)/sgs.h $(COMCMD)/cc.c
		$(LINT_CMD) $(DEFLIST) $(INCLIST) $(COMCMD)/cc.c

clean :
		-rm -f ../*/core ../*/*~ ../*/nohup.out ../*/*.bak

clobber :	clean
		-rm -f $(SGS)cc
