#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)dis:m88k/makefile	1.17"
#
#	MOTOROLA-88000 DISASSEMBLER MAKEFILE
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
DBO=FBO
B20=
DEFLIST=-D$(ARCH) -DM88K -D$(DBO) -D$(ARFORMAT) $(FLEX)
INCLIST=-I$(M32INC) -I$(COMINC) -I. -I$(COMDIS) -I$(USRINC)
ENV=
CC_CMD=$(CC) $(CFLAGS) $(DEFLIST) $(INCLIST) $(ENV)
#
LDFLAGS=$(FFLAG)
LD_CMD=$(CC) $(LDFLAGS)
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
#	Internal Directories
#
BASE= ../..
BINDIR= $(BASE)/xenv/m88k/bin
M32INC= $(BASE)/inc/m88k
COMINC= $(BASE)/inc/m88k
COMDIS= $(BASE)/dis/common
#
#	External Directories
#
ROOT=
VERSION=
USRINC=$(ROOT)/usr/$(VERSION)include
#
LIBLD	= $(BASE)/libld/m88k/$(SGS)libld.a
#
OBJECTS=dis.o dis_main.o dis_utls.o bits.o dis_extn.o lists.o symfcns.o hashfcns.o
#
#
all:	dis
#
dis:	$(OBJECTS) $(LIBLD)
	$(LD_CMD) -o dis $(OBJECTS) $(LIBLD) $(LDLIBS)
#
dis_extn.o:	dis.h $(COMDIS)/dis_extn.c $(COMINC)/scnhdr.h $(COMINC)/filehdr.h \
	$(COMINC)/ldfcn.h $(COMINC)/linenum.h $(COMINC)/syms.h \
	$(COMINC)/storclass.h $(COMDIS)/structs.h
	$(CC_CMD) $(COMDIS)/dis_extn.c
dis_main.o:	dis.h $(COMDIS)/dis_main.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h \
	$(M32INC)/paths.h $(M32INC)/sgs.h $(COMINC)/scnhdr.h $(COMINC)/syms.h\
	$(COMINC)/storclass.h $(COMINC)/linenum.h $(COMDIS)/structs.h
	$(CC_CMD) $(COMDIS)/dis_main.c
dis_utls.o:	dis.h $(COMDIS)/dis_utls.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h \
	$(COMINC)/scnhdr.h $(COMINC)/syms.h $(COMINC)/linenum.h $(COMINC)/storclass.h \
	$(M32INC)/sgs.h $(COMDIS)/structs.h
	$(CC_CMD) $(B20) $(COMDIS)/dis_utls.c
lists.o:	dis.h $(COMDIS)/lists.c $(COMDIS)/structs.h \
	$(COMINC)/ldfcn.h $(COMINC)/scnhdr.h $(COMINC)/linenum.h \
	$(COMINC)/syms.h $(COMINC)/storclass.h $(M32INC)/paths.h \
	$(M32INC)/sgs.h
	$(CC_CMD) $(COMDIS)/lists.c
symfcns.o:	dis.h $(COMDIS)/symfcns.c $(COMDIS)/structs.h \
	$(COMINC)/ldfcn.h $(COMINC)/scnhdr.h $(COMINC)/linenum.h \
	$(COMINC)/syms.h $(COMINC)/storclass.h $(M32INC)/paths.h \
	$(M32INC)/sgs.h
	$(CC_CMD) $(COMDIS)/symfcns.c
hashfcns.o:	dis.h $(COMDIS)/hashfcns.c $(COMDIS)/structs.h \
	$(COMINC)/ldfcn.h $(COMINC)/scnhdr.h $(COMINC)/linenum.h \
	$(COMINC)/syms.h $(COMINC)/storclass.h $(M32INC)/paths.h \
	$(M32INC)/sgs.h
	$(CC_CMD) $(COMDIS)/hashfcns.c
bits.o:		dis.h bits.c $(COMINC)/filehdr.h $(COMINC)/ldfcn.h \
	$(COMINC)/scnhdr.h $(M32INC)/sgs.h
	$(CC_CMD) $(B20) bits.c
dis.o:
	$(CC_CMD) $(B20) dis.c
#
install:	$(BINDIR)/$(SGS)dis
#
$(BINDIR)/$(SGS)dis:	dis
	cp dis dis.bak
	-$(STRIP_CMD) dis
	sh $(BASE)/sgs.install 755 $(OWN) $(GRP) $(BINDIR)/$(SGS)dis dis
	mv dis.bak dis
#
save:		$(BINDIR)/$(SGS)dis
	-rm -f $(BINDIR)/$(SGS)dis.back
	cp $(BINDIR)/$(SGS)dis $(BINDIR)/$(SGS)dis.back
#
uninstall:	$(BINDIR)/$(SGS)dis.back
	-rm -f $(BINDIR)/$(SGS)dis
	cp $(BINDIR)/$(SGS)dis.back $(BINDIR)/$(SGS)dis
#
shrink:
	-rm -f $(OBJECTS) dis
#
lint:
	$(LINT_CMD) $(DEFLIST) $(INCLIST) $(COMDIS)/*.c

clean:
	-rm -f *.o

clobber: clean
	rm -f dis
