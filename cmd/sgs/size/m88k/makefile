#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)size:m88k/makefile	1.15"
#
#	BELLMAC-32 SIZE MAKEFILE
#
#
SGS=m88k
OWN=bin
GRP=bin
CC=cc
CFLAGS=-O -c
ARCH=AR32W
#	Set ARFORMAT to "PORTAR" for portable archive format (post 5.0)
#	Set ARFORMAT to OLDAR for pre-5.0 archive format
ARFORMAT=PORTAR
#	Set FLEX to -DFLEXNAMES for flexnames (post 6.0)
FLEX= -DFLEXNAMES
DEFLIST=-D$(ARCH) -D$(ARFORMAT) -DUNIX $(FLEX)
INCLIST=-I$(M88KINC) -I$(COMINC) -I$(COMSIZE) -I$(USRINC)
ENV=
CC_CMD=$(CC) $(CFLAGS) $(DEFLIST) $(INCLIST) $(ENV)
#
LDFLAGS=$(FFLAG)
LD_CMD=$(CC) $(LDFLAGS)
#
LDLIBS=
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
COMSIZE= $(BASE)/size/common
COMINC= $(BASE)/inc/m88k
#
LIBLD	= $(BASE)/libld/m88k/libld.a
OBJECTS = main.o process.o fcns.o
#
all:	size
size:	$(OBJECTS) $(LIBLD)
	$(LD_CMD) -o size $(OBJECTS) $(LIBLD) $(LDLIBS)
#
main.o:	$(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(M88KINC)/sgs.h $(COMSIZE)/defs.h \
	$(COMSIZE)/main.c
	$(CC_CMD) $(COMSIZE)/main.c
#
process.o:	$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/ldfcn.h \
		$(COMSIZE)/process.h $(COMSIZE)/defs.h $(COMSIZE)/process.c \
		$(M88KINC)/sgs.h
	$(CC_CMD) $(COMSIZE)/process.c
#
fcns.o:	$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/ldfcn.h \
	$(M88KINC)/sgs.h $(COMSIZE)/defs.h $(COMSIZE)/fcns.c
	$(CC_CMD) $(COMSIZE)/fcns.c
#
save:	$(BINDIR)/$(SGS)size
	-rm -f $(BINDIR)/$(SGS)size.back
	cp $(BINDIR)/$(SGS)size $(BINDIR)/$(SGS)size.back
#
install:	$(BINDIR)/$(SGS)size
#
$(BINDIR)/$(SGS)size:	size
	cp size size.bak
	-$(STRIP_CMD) size
	sh $(BASE)/sgs.install 755 $(OWN) $(GRP) $(BINDIR)/$(SGS)size size
	mv size.bak size
#
uninstall:	$(BINDIR)/$(SGS)size.back
	-rm -f $(BINDIR)/$(SGS)size
	cp $(BINDIR)/$(SGS)size.back $(BINDIR)/$(SGS)size
#
shrink:
	-rm -f $(OBJECTS) size
#
lint:	$(COMSIZE)/main.c $(COMSIZE)/process.c $(COMSIZE)/fcns.c \
	$(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(COMINC)/scnhdr.h \
	$(M88KINC)/sgs.h $(COMSIZE)/process.h $(COMSIZE)/defs.h
	$(LINT_CMD) $(DEFLIST) $(INCLIST) $(COMSIZE)/*.c
