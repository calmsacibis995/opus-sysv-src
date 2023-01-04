#
#	Macro definitions:
#
#	DBUG	=>	Set to "-DNO_DBUG" if the macro based
#			debugging package "dbug" is not available.
#			Set to "-DDBUG_OFF" if the package is available
#			but not to be used.
#
#	LIBDBUG	=>	Set to "-ldbug" if the dbug package is used.
#
#	ARFORMAT =>	Format of the archives to be examined.
#

TARGET=		m88k
BASE=		..
BINDIR =	$(BASE)/xenv/m88k/bin
DBUG =		-DNO_DBUG
ARFORMAT =	PORTAR

ROOT =
LIB =		$(ROOT)/lib
USRLIB =	$(ROOT)/usr/lib
#LIBC =		$(LIB)/libc.a
LIBDBUG =
LIBLD = 	../libld/m88k/libld.a
INC =		../inc/m88k
#BSD =		-DBSD
#OFLAG =	-O

#
#	Macros derived from other macros.
#

DEFLIST =	-DV68 -D$(ARFORMAT) $(DBUG) $(BSD) -DTARGET=$(TARGET)
CFLAGS =	$(OFLAG) $(DEFLIST) -I$(INC)
LIBS =		$(LIBLD) $(LIBDBUG) $(LIBC)

#
#	List of objects which make up the common object module dump
#	utility.
#

OBJ =		coffdump.o dump.o errors.o filehdr.o reloc.o scnhdr.o \
		syms.o usage.o utils.o

#
#	Make target "native" to remake for native system.
#

native:
		$(MAKE) ROOT= LIBDBUG=$(LIBDBUG) $(SGS)coffdump

#
#	Make target "bsd4.1" to remake for vax bsd4.1 environment with
#	system V emulation environment.
#

bsd4.1:
		$(MAKE) ROOT=/v2/V68 USRLIB=$(LIB) coffdump

#
#	The remake the executable.
#

$(SGS)coffdump :	$(OBJ)
			$(CC) -o $@ $(OBJ) $(LIBS)

#
#	What each object module depends on.
#

$(OBJ) :	coffdump.h

#
#	cleanup
#

shrink:
	rm -f *.o core
#
#	Install the executable.
#

install	 :			$(BINDIR)/$(SGS)coffdump

$(BINDIR)/$(SGS)coffdump:	$(SGS)coffdump
				sh $(BASE)/sgs.install 755 $(OWN) $(GRP) $@ $?

		


