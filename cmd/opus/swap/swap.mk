CFLAGS = -O -I${ROOT}/usr/include
INS = /etc/install
INSDIR = ${ROOT}/opus/bin
LDFLAGS = -s

all:	swapbin swapfs fs_size

swapbin: swapbin.c
	-if ns32332 || ns32000 ; then \
	$(CC) $(CFLAGS) -DNS32000 -o swapbin swapbin.c; \
	elif clipper ; then \
	$(CC) $(CFLAGS) -DCLIPPER -o swapbin swapbin.c; \
	elif m88k ; then \
	$(CC) $(CFLAGS) -DM88K -o swapbin swapbin.c; \
	fi

swapfs: swapfs.c
	$(CC) $(CFLAGS) -o swapfs swapfs.c

fs_size: fs_size.c
	$(CC) $(CFLAGS) -o fs_size fs_size.c

install: all
	-mkdir $(INSDIR)
	-chmod 775 $(INSDIR)
	-chown root $(INSDIR)
	-chgrp sys $(INSDIR)
	$(INS) -f $(INSDIR) -m 775 -u bin -g bin swapbin
	$(INS) -f $(INSDIR) -m 775 -u bin -g bin swapfs
	$(INS) -f $(INSDIR) -m 775 -u root -g other fs_size

clean:
	rm -f *.o

clobber: clean
	rm -f swapbin swapfs fs_size
