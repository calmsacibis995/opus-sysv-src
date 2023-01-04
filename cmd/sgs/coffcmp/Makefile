#
#	Macro definitions:
#
#	DBUG	=>	Set to "-DNO_DBUG" if macro based
#			debugging package is not available.
#			Set to "-DDBUG_OFF" if dbug package
#			is available but not wanted.  This
#			results in minimal code size.
#			Left undefined otherwise.
#
#	DBUGLIB	=>	If macro based debugger is used, must
#			be set to "-ldbug" to include the
#			runtime support package.
#

TARGET=		m88k
BASE=		..
BINDIR =	$(BASE)/xenv/m88k/bin
DBUG =		-DNO_DBUG

#LIBC =		-lc
LIBDBUG =
LIBLD =		../libld/m88k/libld.a
INC =		../inc/m88k
#OFLAG =		-O

#
#	Macros derived from other macros.
#

CFLAGS =	$(OFLAG) $(DBUG) -I$(INC) -DTARGET=$(TARGET)
LIBS =		$(LIBLD) $(LIBDBUG) $(LIBC)

#
#	List of objects which make up the common object module dump
#	utility.
#

OBJ =		coffcmp.o errors.o usage.o utils.o

#
#	The default is to remake the executable.
#

$(SGS)coffcmp :	$(OBJ)
		$(CC) -o $@ $(OBJ) $(LIBS)

#
#	What each object module depends on.
#

$(OBJ) :	coffcmp.h

#
#	cleanup
#

shrink:
	rm -f core *.o

clobber: shrink
	rm -f $(SGS)coffcmp
#
#	Install the executable.
#

install	 :			$(BINDIR)/$(SGS)coffcmp

$(BINDIR)/$(SGS)coffcmp:	$(SGS)coffcmp
				sh $(BASE)/sgs.install 755 $(OWN) $(GRP) $@ $?


