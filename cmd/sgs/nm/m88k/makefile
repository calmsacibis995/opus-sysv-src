#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)nm:m88k/makefile	1.16"
#
#	M88000 NM MAKEFILE
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
INCLIST=-I$(M88KINC) -I$(COMINC) -I$(COMNM) -I$(USRINC)
ENV=
CC_CMD=$(CC) $(CFLAGS) $(DEFLIST) $(INCLIST) $(ENV)
#
LDFLAGS=-i $(FFLAG)
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
COMNM= $(BASE)/nm/common
COMINC= $(BASE)/inc/m88k
#
LIBLD	= $(BASE)/libld/m88k/libld.a
OBJECTS = main.o setflags.o process.o prosym.o decotype.o fcns.o
#
all:	nm
#
nm:	$(OBJECTS) $(LIBLD)
	$(LD_CMD) -o nm $(OBJECTS) $(LIBLD) $(LDLIBS)
#
main.o:	$(COMNM)/main.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(M88KINC)/sgs.h \
	$(COMNM)/defs.h
	$(CC_CMD) $(COMNM)/main.c
#
setflags.o:	$(COMNM)/setflags.c $(M88KINC)/sgs.h $(M88KINC)/paths.h $(COMNM)/defs.h
	$(CC_CMD) $(COMNM)/setflags.c
#
process.o:	$(COMNM)/process.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h \
		$(COMINC)/scnhdr.h $(COMNM)/defs.h
	$(CC_CMD) $(COMNM)/process.c
#
prosym.o:	$(COMNM)/prosym.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(COMINC)/syms.h \
		$(COMINC)/storclass.h $(COMNM)/defs.h $(COMNM)/prosym.h
	$(CC_CMD) $(COMNM)/prosym.c
#
decotype.o:	$(COMNM)/decotype.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h \
		$(COMINC)/syms.h $(COMINC)/storclass.h $(COMNM)/decotype.h $(COMNM)/defs.h
	$(CC_CMD) $(COMNM)/decotype.c
#
fcns.o:	$(COMNM)/fcns.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(M88KINC)/sgs.h \
	$(COMNM)/defs.h
	$(CC_CMD) $(COMNM)/fcns.c
#
save:	$(BINDIR)/$(SGS)nm
	-rm -f $(BINDIR)/$(SGS)nm.back
	cp $(BINDIR)/$(SGS)nm $(BINDIR)/$(SGS)nm.back
#
install:	$(BINDIR)/$(SGS)nm
#
$(BINDIR)/$(SGS)nm: nm
	cp nm nm.bak
	-$(STRIP_CMD) nm
	sh $(BASE)/sgs.install 755 $(OWN) $(GRP) $(BINDIR)/$(SGS)nm nm
	mv nm.bak nm
#
uninstall:	$(BINDIR)/$(SGS)nm.back
	-rm -f $(BINDIR)/$(SGS)nm
	cp $(BINDIR)/$(SGS)nm.back $(BINDIR)/$(SGS)nm
#
shrink:
	-rm -f $(OBJECTS) nm
#
lint:	$(COMNM)/main.c $(COMNM)/setflags.c $(COMNM)/process.c \
	$(COMNM)/prosym.c $(COMNM)/decotype.c $(COMNM)/fcns.c \
	$(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(COMINC)/syms.h \
	$(COMINC)/storclass.h $(M88KINC)/paths.h $(M88KINC)/sgs.h \
	$(COMNM)/prosym.h $(COMNM)/decotype.h \
	$(COMINC)/scnhdr.h $(COMS/defs.h
	$(LINT_CMD) $(DEFLIST) $(INCLIST) $(COMNM)/*.c
