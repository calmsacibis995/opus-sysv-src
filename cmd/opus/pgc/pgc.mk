#  @(#)pgc.mk	1.2

INSDIR = ${ROOT}/usr/lib
CFLAGS = -O -I${ROOT}/usr/include
AR=ar
INS=:

OBJ= libpgc.o

libpgc.a: $(OBJ)
	rm -f libpgc.a
	$(AR) -r libpgc.a $(OBJ)
	$(INS) $(INSDIR) libpgc.a

all:	install clobber

install:
	$(MAKE) -f pgc.mk INS="install -f"

clean:
	-rm -f *.o

clobber: clean
	rm -f libpgc.a 
