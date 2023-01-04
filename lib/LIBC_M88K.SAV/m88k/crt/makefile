#	MC68000 C RUNTIME MAKEFILE
#
#	@(#)makefile	6.3	
#
# The variable PROF is null by default, causing both the standard C library
# and a profiled library to be maintained.  If profiled object is not 
# desired, the reassignment PROF=@# should appear in the make command line.
#
# The variable IGN may be set to -i by the assignment IGN=-i in order to
# allow a make to complete even if there are compile errors in individual
# modules.
#
#

.SUFFIXES:

.SUFFIXES: .o .s .c .p

ROOT=
INC=../../../../include
INCSYS=../../../../include/sys

M4=m4
M4ARGS= ../m4.def ../reg.def
AS=
CC=
LINT=	lint
AFLAGS=	r
# OFLAG=	-O
OFLAG=
CFLAGS=	$(OFLAG) -I$(INC)
# LFLAGS=	-p
LFLAGS=
LDFLAGS=
PROF=@#

FPPO=	fpdata.o

FPPP=	fpdata.p

FPPC=

OTHERO=	cerror.o mcount.o sigcode.o
OTHERP=	cerror.p mcount.p sigcode.p
OTHERC=

OFILES=		$(FPPO) $(OTHERO)
PFILES=		$(FPPP) $(OTHERP)
CFILES=		$(FPPC) $(OTHERC)


nonprof:	$(VERIFY) $(OFILES)

prof:		$(VERIFY) $(PFILES)

#-------------------------

.c.o:
	$(CC) $(CFLAGS) -c $<

.c.p:
	$(CC) -p $(CFLAGS) -c $< && mv $*.o $*.p

#-------------------------

.s.o:
	$(M4) $(M4ARGS) -DMCOUNT=\;  $*.s >temp.s
	$(CC) $(CFLAGS) -c temp.s && mv temp.o $*.o
	-rm -f temp.s

.s.p:
	$(M4) $(M4ARGS) ../mcount.def  $*.s > temp.s
	$(CC) $(CFLAGS) -c temp.s && mv temp.o $*.p
	-rm -f temp.s

#--------------------------

clean:
	-rm -f $(OFILES) $(PFILES) temp temp.o temp.s

#--------------------------

clobber:	clean

#--------------------------

lint:	$(CFILES) 
	$(LINT) $(LFLAGS) $(CFILES) >lint.out

verify:
	@echo ".../lib/libc/m88k/crt/makefile"; \
	echo "	(CC): $(CC)" ; \
	echo "	(CFILES): $(CFILES)" ; \
	echo "	(CFLAGS): $(CFLAGS)" ; \
	echo "	(FPPC): $(FPPC)" ; \
	echo "	(FPPO): $(FPPO)" ; \
	echo "	(FPPP): $(FPPP)" ; \
	echo "	(INC): $(INC)" ; \
	echo "	(LFLAGS): $(LFLAGS)" ; \
	echo "	(LINT): $(LINT)" ; \
	echo "	(M4): $(M4)" ; \
	echo "	(M4ARGS): $(M4ARGS)" ; \
	echo "	(NONPROF): $(NONPROF)" ; \
	echo "	(OFILES): $(OFILES)" ; \
	echo "	(OFLAG): $(OFLAG)" ; \
	echo "	(OTHERC): $(OTHERC)" ; \
	echo "	(OTHERO): $(OTHERO)" ; \
	echo "	(OTHERP): $(OTHERP)" ; \
	echo "	(PFILES): $(PFILES)" ; \
	echo "	(PROF): $(PROF)" ; \
	echo "	(ROOT): $(ROOT)" ; \
	$(CHKYN) "Continue" || exit 1
