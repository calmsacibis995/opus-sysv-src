#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)dump:m88k/makefile	1.14"
#
#	M88000 DUMP MAKEFILE
#
#
SGS=m88k
OWN=bin
GRP=bin
CC=cc
CFLAGS=-O -c
ARCH=AR32W
#	Set ARFORMAT to "PORTAR" for portable archive format (post 5.0)
ARFORMAT=PORTAR
#	Set FLEX to -DFLEXNAMES for flexnames (post 6.0)
FLEX= -DFLEXNAMES
DEFLIST=-D$(ARCH) -D$(ARFORMAT) $(FLEX)
INCLIST=-I$(COMINC) -I$(COMDUMP) -I$(M88KINC) -I$(USRINC)
ENV=
CC_CMD=$(CC) $(CFLAGS) $(DEFLIST) $(INCLIST) $(ENV)
FFLAG=
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
ROOT=
VERSION=
USRINC= $(ROOT)/usr/$(VERSION)include
#
#
#	Internal Directories
BASE= ../..
BINDIR= $(BASE)/xenv/m88k/bin
COMDUMP= $(BASE)/dump/common
COMINC= $(BASE)/inc/m88k
M88KINC= $(BASE)/inc/m88k
#
LIBLD	= $(BASE)/libld/m88k/libld.a
#
OBJECTS= main.o setflags.o openobject.o process.o fcns.o
#
all:	dump
#
dump:	$(OBJECTS) $(LIBLD)
	$(LD_CMD) -o dump $(OBJECTS) $(LIBLD) $(LDLIBS)
#
main.o:	$(COMDUMP)/main.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(COMDUMP)/filedefs.h \
	 $(COMDUMP)/dumpmap.h $(M88KINC)/sgs.h
	$(CC_CMD) $(COMDUMP)/main.c
#
setflags.o:	$(COMDUMP)/setflags.c $(COMDUMP)/flagdefs.h $(M88KINC)/sgs.h
	$(CC_CMD) $(COMDUMP)/setflags.c
#
openobject.o:	$(COMDUMP)/openobject.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h \
	       $(M88KINC)/sgs.h $(COMDUMP)/filedefs.h
	$(CC_CMD) $(COMDUMP)/openobject.c
#
process.o:	$(COMDUMP)/process.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(M88KINC)/sgs.h\
	    $(COMDUMP)/filedefs.h $(COMDUMP)/process.h $(COMDUMP)/flagdefs.h
	$(CC_CMD) $(COMDUMP)/process.c
#
fcns.o:	$(COMDUMP)/fcns.c $(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/reloc.h \
	 $(COMINC)/linenum.h $(COMINC)/syms.h $(COMINC)/storclass.h $(COMINC)/ldfcn.h \
	 $(M88KINC)/sgs.h
	$(CC_CMD) $(COMDUMP)/fcns.c
#
save:	$(BINDIR)/$(SGS)dump
	-rm -f $(BINDIR)/$(SGS)dump.back
	cp $(BINDIR)/$(SGS)dump $(BINDIR)/$(SGS)dump.back
#
install:	$(BINDIR)/$(SGS)dump
#
$(BINDIR)/$(SGS)dump:	dump
	cp dump dump.bak
	-$(STRIP_CMD) dump
	sh $(BASE)/sgs.install 755 $(OWN) $(GRP) $(BINDIR)/$(SGS)dump dump
	mv dump.bak dump
#
uninstall:	$(BINDIR)/$(SGS)dump.back
	-rm -f $(BINDIR)/$(SGS)dump
	cp $(BINDIR)/$(SGS)dump.back $(BINDIR)/$(SGS)dump
#
lint:	$(COMDUMP)/main.c $(COMDUMP)/setflags.c $(COMDUMP)/openobject.c \
	$(COMDUMP)/process.c $(COMDUMP)/fcns.c \
	$(COMDUMP)/dumpmap.h \
	$(COMINC)/filehdr.h $(COMINC)/ldfcn.h $(COMDUMP)/filedefs.h \
	$(M88KINC)/sgs.h $(COMDUMP)/flagdefs.h $(M88KINC)/paths.h \
	$(COMINC)/syms.h $(COMINC)/storclass.h $(COMINC)/linenum.h \
	$(COMINC)/scnhdr.h $(COMINC)/reloc.h
	$(LINT_CMD) $(DEFLIST) $(INCLIST) $(COMDUMP)/*.c
#
shrink:
	-rm -f $(OBJECTS) dump
