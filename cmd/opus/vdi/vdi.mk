#  @(#)vdi.mk	1.2

INSDIR = ${ROOT}/usr/lib
CFLAGS = -O -I${ROOT}/usr/include
AR=ar
INS=:

OBJ= v0.o v1.o

libvdi.a: $(OBJ)
	rm -f libvdi.a
	$(AR) -r libvdi.a $(OBJ)
	$(INS) $(INSDIR) libvdi.a

all:	install gannt clobber

gannt:
	$(CC) $(CFLAGS) gannt.c -lvdi -o gannt

install:
	$(MAKE) -f vdi.mk INS="install -f"

clean:
	-rm -f *.o

clobber: clean
	rm -f libvdi.a gannt
