#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)as:m88k/makefile	1.24"
#
#	M88000 AS MASTER MAKEFILE
#
# Architectures:
#
#	AR16WR = DEC 11/45-70
#	AR32WR = VAX 11/780
#	AR32W  = 3B family, IBM 370, Motorola m68k, Motorola m88k
#
MAKE =		make
OWN =		bin
GRP =		bin
TARGET=m88k
PFX=$(SGS)
CC =cc 
GFLAG =
OFLAG = -O
CFLAGS =	$(GFLAG) $(OFLAG) $(DEFLIST) $(INCLIST)
ARCH =		AR32W

CCPARMS =	CC="$(CC)" CFLAGS="$(CFLAGS)" FFLAG="$(FFLAG)" \
		SGSENV="$(SGSENV)" VERSION="$(VERSION)"
SGSPARMS =	MAKE="$(MAKE)" SGS="$(SGS)" ARCH="$(ARCH)"
DIRPARMS =	BINDIR="$(BINDIR)" LIBDIR="$(LIBDIR)" ROOT="$(ROOT)"

#
#	Internal Directories
#

BASE=		../..

# Notes:
#
#	The assembler is now a single process assembler only.  All support
#	for !ONEPROC has been removed.
#
#	The assembler is now a FLEXNAMES assembler only.  All support for
#	!FLEXNAMES has been removed.
#

#	Define EXPR to create output with an .expr section to be used with 
#	sld.    
#EXPRSECT = 	-DEXPR

DBUG =		-DNO_DBUG
DEFLIST =	-D$(ARCH) $(DBUG) $(EXPRSECT) -DTARGET=$(TARGET)
INCLIST =	-I$(COMAS) -I$(COMINC) -I$(M88KINC) -I.
CC_CMD =	$(CC) $(CFLAGS)
SYS_CC = cc

LDFLAGS =	$(FFLAG) $(GFLAG)
LIBS =		$(LIBM) $(LIBDBUG)
LD_CMD =	$(CC) $(LDFLAGS)

STRIP =		strip
SPFLAGS =
STRIP_CMD =	$(STRIP) $(SPFLAGS)

LINT =		lint
LINTFLAGS =
LINT_CMD =	$(LINT) -Dlint $(LINTFLAGS)

#
#	Set COMINC to the directory containing common includes.
#	Set M88KINC to the directory containing machine specific includes.
#

COMINC =	../../inc/m88k
M88KINC =	../../inc/m88k

#
#	Internal Directories
#

BASE =		../..
COMAS =		../m88k
BINDIR =	$(BASE)/xenv/m88k/bin
LIBDIR =	$(BASE)/xenv/m88k/lib

#
#	External Directories
#

ROOT =
VERSION =

#LIBM =		-lm
#LIBDBUG =	-ldbug

OFILES =	as_ctype.o pass0.o program.o operand.o gencode.o pass1.o\
		symbols.o perf.o code.o expand1.o expand2.o errors.o\
		newerrors.o addr2.o float.o pass2.o addr1.o codeout.o\
		getstab.o obj.o symlist.o unused.o frexp.o

CFILES =	as_ctype.c $(COMAS)/pass0.c \
		operand.c program.c $(COMAS)/code.c $(COMAS)/errors.c \
		$(COMAS)/newerrors.c \
		gencode.c $(COMAS)/perf.c \
		$(COMAS)/pass1.c $(COMAS)/symbols.c \
		expand1.c expand2.c float.c \
		$(COMAS)/addr1.c addr2.c $(COMAS)/codeout.c \
		$(COMAS)/getstab.c $(COMAS)/pass2.c $(COMAS)/obj.c \
		$(COMAS)/symlist.c $(COMAS)/unused.c

HFILES =	as_ctype.h $(COMAS)/codeout.h gendefs.h instab.h \
		$(COMAS)/section.h $(COMAS)/symbols.h \
		$(COMAS)/temppack.h $(M88KINC)/paths.h $(COMAS)/expand.h \
		expand2.h program.h \
		$(COMINC)/reloc.h $(COMINC)/syms.h $(COMINC)/storclass.h\
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMINC)/linenum.h \
		$(M88KINC)/sgs.h

all :		$(SGS)as

sifas :		sifas.c
		$(CC) -o sifas $(CFLAGS) sifas.c

$(SGS)as :	$(OFILES)
		$(LD_CMD) -o $@ $(OFILES) $(LIBS)

as_ctype.o :	as_ctype.h as_ctype.c
		$(CC) -c $(CFLAGS) as_ctype.c

pass0.o	 :	$(COMAS)/pass0.c $(COMAS)/errors.h $(M88KINC)/paths.h \
		$(M88KINC)/sgs.h gendefs.h systems.h
		$(CC) -c $(CFLAGS) $(COMAS)/pass0.c

perf.o :	$(COMAS)/perf.c $(COMAS)/dbug.h
		$(CC) -c $(CFLAGS) $(COMAS)/perf.c

program.o :	as_ctype.h program.c $(COMAS)/symbols.h \
		$(COMINC)/filehdr.h program.h instab.h \
		systems.h ind.out typ.out mnm.out sem.out gendefs.h \
		$(COMINC)/scnhdr.h $(COMINC)/storclass.h $(M88KINC)/sgs.h \
		$(COMINC)/syms.h
		$(CC) -c $(CFLAGS) program.c

maketables :	maketables.c
		$(SYS_CC) $(DEFLIST) $(INCLIST) -o maketables maketables.c $(LIBDBUG)

ind.out typ.out mnm.out sem.out :	ops.in maketables
		./maketables <ops.in

operand.o:	operand.c as_ctype.h program.h $(COMAS)/symbols.h instab.h\
		systems.h
		$(CC) -c $(CFLAGS) operand.c

code.o	 :	$(COMAS)/code.c $(COMAS)/symbols.h $(COMAS)/codeout.h \
		$(COMINC)/scnhdr.h \
		$(COMAS)/section.h gendefs.h systems.h $(COMAS)/temppack.h
		$(CC) -c $(CFLAGS) $(COMAS)/code.c

errors.o :	$(COMAS)/errors.c gendefs.h systems.h
		$(CC) -c $(CFLAGS) $(COMAS)/errors.c

newerrors.o :	$(COMAS)/newerrors.c $(COMAS)/errors.h
		$(CC) -c $(CFLAGS) $(COMAS)/newerrors.c

pass1.o	 :	$(COMAS)/pass1.c $(COMAS)/codeout.h $(COMAS)/section.h \
		$(M88KINC)/paths.h $(COMAS)/symbols.h gendefs.h systems.h
		$(CC) -c $(CFLAGS) $(COMAS)/pass1.c

gencode.o :	gencode.c $(COMAS)/symbols.h instab.h systems.h \
		program.h gendefs.h $(COMAS)/expand.h expand2.h
		$(CC) -c $(CFLAGS) gencode.c

expand1.o :	expand1.c $(COMAS)/expand.h $(COMAS)/symbols.h gendefs.h systems.h
		$(CC) -c $(CFLAGS) expand1.c

expand2.o :	expand2.c $(COMAS)/expand.h expand2.h $(COMAS)/symbols.h systems.h
		$(CC) -c $(CFLAGS) expand2.c

float.o	 :	float.c instab.h $(COMAS)/symbols.h systems.h program.h
		$(CC) -c $(CFLAGS) float.c

addr1.o	 :	$(COMAS)/addr1.c $(COMINC)/reloc.h $(COMINC)/syms.h \
		$(COMINC)/storclass.h $(COMINC)/linenum.h $(COMINC)/filehdr.h \
		gendefs.h $(COMAS)/symbols.h $(COMAS)/codeout.h systems.h
		$(CC) -c $(CFLAGS) $(COMAS)/addr1.c

addr2.o	 :	addr2.c $(COMINC)/reloc.h $(COMINC)/storclass.h systems.h \
		$(COMINC)/scnhdr.h $(COMINC)/syms.h gendefs.h $(COMAS)/symbols.h \
		$(COMAS)/codeout.h instab.h
		$(CC) -c $(CFLAGS) addr2.c

codeout.o :	$(COMAS)/codeout.c $(COMAS)/symbols.h $(COMAS)/codeout.h \
		$(COMAS)/section.h gendefs.h systems.h $(COMAS)/temppack.h
		$(CC) -c $(CFLAGS) $(COMAS)/codeout.c

getstab.o :	$(COMAS)/getstab.c gendefs.h $(COMAS)/symbols.h systems.h
		$(CC) -c $(CFLAGS) $(COMAS)/getstab.c

pass2.o	 :	$(COMAS)/pass2.c $(COMAS)/codeout.h $(COMAS)/section.h gendefs.h \
		$(COMINC)/filehdr.h $(COMINC)/scnhdr.h $(COMAS)/symbols.h systems.h
		$(CC) -c $(CFLAGS) $(COMAS)/pass2.c

obj.o	 :	$(COMAS)/obj.c $(COMINC)/filehdr.h $(COMINC)/linenum.h instab.h \
		$(COMINC)/reloc.h $(COMINC)/scnhdr.h $(COMINC)/syms.h \
		$(COMAS)/codeout.h $(COMAS)/section.h $(COMINC)/storclass.h \
		$(COMAS)/symbols.h $(COMAS)/codeout.h gendefs.h $(M88KINC)/sgs.h systems.h
		$(CC) -c $(CFLAGS) $(COMAS)/obj.c

symlist.o :	$(COMAS)/symlist.c $(COMAS)/symbols.h $(COMINC)/syms.h \
		$(COMINC)/storclass.h gendefs.h systems.h
		$(CC) -c $(CFLAGS) $(COMAS)/symlist.c

symbols.o :	$(COMAS)/symbols.c $(COMAS)/symbols.h symbols2.h \
		$(COMINC)/syms.h $(COMINC)/storclass.h systems.h
		$(CC) -c $(CFLAGS) $(COMAS)/symbols.c

unused.o :	$(COMAS)/unused.c symbols2.h $(COMINC)/filehdr.h \
		$(COMINC)/linenum.h $(COMINC)/reloc.h $(COMINC)/scnhdr.h \
		$(COMINC)/syms.h $(COMINC)/storclass.h
		$(CC) -c $(CFLAGS) $(COMAS)/unused.c

lint.out :	$(CFILES) $(HFILES) 
		$(LINT_CMD) $(DEFLIST) $(INCLIST) $(CFILES) $(LIBS) >lint.out
		wc -l lint.out

#install :	$(BINDIR)/$(SGS)as $(LIBDIR)/cm4defs $(LIBDIR)/cm4tvdefs

#install :	$(LIBDIR)/$(SGS)as $(BINDIR)/$(SGS)as
install :	$(BINDIR)/$(SGS)as

$(BINDIR)/$(SGS)as:	$(SGS)as
			sh $(BASE)/sgs.install 755 $(OWN) $(GRP) /bin/as $(SGS)as

#$(BINDIR)/$(SGS)as:	sifas
			#sh $(BASE)/sgs.install 755 $(OWN) $(GRP) /bin/as sifas

$(LIBDIR)/cm4defs:	cm4defs
			sh $(BASE)/sgs.install 444 $(OWN) $(GRP) $(LIBDIR)/cm4defs cm4defs

$(LIBDIR)/cm4tvdefs:	cm4tvdefs
		sh $(BASE)/sgs.install 444 $(OWN) $(GRP) $(LIBDIR)/cm4tvdefs cm4tvdefs

save	 :	$(BINDIR)/$(SGS)as $(LIBDIR)/cm4defs $(LIBDIR)/cm4tvdefs
		-rm -f $(BINDIR)/$(SGS)as.back
		cp $(BINDIR)/$(SGS)as $(BINDIR)/$(SGS)as.back
		-rm -f $(LIBDIR)/cm4defs.back $(LIBDIR)/cm4tvdefs.back
		cp $(LIBDIR)/cm4defs $(LIBDIR)/cm4defs.back
		cp $(LIBDIR)/cm4tvdefs $(LIBDIR)/cm4tvdefs.back

uninstall :	$(BINDIR)/$(SGS)as.back \
		$(LIBDIR)/cm4defs.back $(LIBDIR)/cm4tvdefs.back
		-rm -f $(BINDIR)/$(SGS)as
		cp $(BINDIR)/$(SGS)as.back $(BINDIR)/$(SGS)as
		-rm -f $(LIBDIR)/cm4defs $(LIBDIR)/cm4tvdefs
		cp $(LIBDIR)/cm4defs.back $(LIBDIR)/cm4defs
		cp $(LIBDIR)/cm4tvdefs.back $(LIBDIR)/cm4tvdefs

clean	:
		rm -f $(OFILES) maketables *.out

shrink	:	clean

clobber	:	clean
		rm -f $(SGS)as sifas

listall	:	$(CFILES) $(HFILES)
		pr -f $(HFILES) $(CFILES) | lp

FRC:
