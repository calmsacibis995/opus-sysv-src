FRC=
# SID @(#)opus.mk	1.2

INS = /etc/install
INSDIR = ${ROOT}/opus
INSDIR2 = ${ROOT}/opus/bin
INSDIR4 = ${ROOT}/opus/src
CFLAGS = -O -I${ROOT}/usr/include
LDFLAGS = -s
SYS =

SFILES = cgannt.c gannt.c
SRC = swap adb cgi dbg dev dos fsorg hayes lib2 opconfig opsash opterm pgc proc stand swap tape udio vert vdi 

all: $(SRC)

install:
	-mkdir $(INSDIR) $(INSDIR2) $(INSDIR4)
	-chmod 775 $(INSDIR) $(INSDIR2)
	-chown root $(INSDIR) $(INSDIR2)
	-chgrp sys $(INSDIR) $(INSDIR2)
	$(INS) -f $(INSDIR) -m 555 -u root -g sys TERM
	$(INS) -f $(INSDIR) -m 555 -u root -g sys profile
	$(INS) -f $(INSDIR2) -m 4555 -u root -g sys cfg
	$(INS) -f $(INSDIR2) -m 775 -u root -g sys misc.sh
	mv $(INSDIR2)/misc.sh $(INSDIR2)/misc
	if clipper || ns32000 || m88k ; then \
		echo "not making cgi for clipper || ns32000 || m88k" \
	else \
		$(INS) -f $(INSDIR4) -m 664 -u bin -g bin cgi/cgannt.c ; \
		$(INS) -f $(INSDIR4) -m 664 -u bin -g bin vdi/gannt.c ; \
	fi
	$(INS) -f $(INSDIR4) -m 664 -u bin -g bin hayes.c
	$(INS) -f $(INSDIR4) -m 666 -u bin -g bin /usr/src/lib/terminfo/opus.ti
	$(INS) -f $(INSDIR4) -m 666 -u bin -g bin opus.tc
	$(INS) -f $(INSDIR2) -m 775 -u bin -g bin hayes
	if m88k ; then \
		echo "not making adb for m88k" ; \
	else \
		cd adb; $(MAKE) -f adb.mk install; \
	fi
	#cd cgi; $(MAKE) -f cgi.mk install; \
	cd dev; $(MAKE) -f dev.mk install;
	cd dos; $(MAKE) -f dos.mk install;
	cd lib2; $(MAKE) -f lib2.mk install;
	cd opconfig; $(MAKE) -f opconfig.mk install;
	cd opsash; $(MAKE) -f opsash.mk install;
	if m88k ; then \
		echo "not making pgc for m88k" ; \
	else \
		cd pgc; $(MAKE) -f pgc.mk install; \
	fi
	cd stand; $(MAKE) -f stand.mk install;

	#cd swap; $(MAKE) -f swap.mk install;
	cd udio; $(MAKE) -f udio.mk install;
	if m88k ; then \
		echo "not making vdi or vert for m88k" ; \
	else \
		cd vdi; $(MAKE) -f vdi.mk install; \
		cd vert; $(MAKE) -f vert.mk install; \
	fi
	$(INS) -f $(INSDIR2) -m 775 -u bin -g bin tape
	$(INS) -f $(INSDIR2) -m 775 -u bin -g bin fsorg
	$(INS) -f $(INSDIR2) -m 755 -u bin -g bin opload
	$(INS) -f $(INSDIR2) -m 4755 -u root -g sys opterm
	if m88k ; then \
		$(INS) -f $(INSDIR2) -m 755 -u root -g sys dbg ; \
		$(INS) -f $(INSDIR2) -m 755 -u root -g sys proc ; \
	fi

adb: FRC
	if m88k ; then \
	echo "not making adb for m88k"; \
	else \
	cd adb; $(MAKE) -f adb.mk; \
	fi

cgi: FRC
	if m88k ; then \
	echo "not making cgi for m88k"; \
	else \
	cd cgi; $(MAKE) -f cgi.mk; \
	fi

dbg:
	if m88k ; then \
	$(CC) $(CFLAGS) -o dbg dbg.c ; \
	else \
	echo "not making dbg"; \
	fi

opterm:	opterm.c
	$(CC) -I/usr/include -o opterm opterm.c -lgen ;

proc:
	if m88k ; then \
	$(CC) -I/usr/include -g -o proc proc.c ; \
	else \
	echo "not making proc"; \
	fi

dev: FRC
	cd dev; $(MAKE) -f dev.mk

dos: FRC
	cd dos; $(MAKE) -f dos.mk

fsorg:
	$(CC) $(CFLAGS) -o fsorg fsorg.c

lib2: FRC
	cd lib2; $(MAKE) -f lib2.mk

opconfig: FRC
	cd opconfig; $(MAKE) -f opconfig.mk

opsash: FRC
	cd opsash; $(MAKE) -f opsash.mk

pgc: FRC
	if m88k ; then \
	echo "not making pgc for m88k"; \
	else \
	cd pgc; $(MAKE) -f pgc.mk; \
	fi

nvr: FRC
	cc -o nvr nvr.c $(CFLAGS) $(LDFLAGS)

cfg: FRC
	cc -o cfg cfg.c $(CFLAGS) $(LDFLAGS)

misc: FRC

TERM: FRC

profile: FRC

stand: FRC
	cd stand; $(MAKE) -f stand.mk

swap: FRC
	#cd swap; $(MAKE) -f swap.mk

tape:
	$(CC) $(CFLAGS) -o tape tape.c

udio: FRC
	cd udio; $(MAKE) -f udio.mk

vdi: FRC
	if m88k ; then \
	echo "not making vdi for m88k"; \
	else \
	cd vdi; $(MAKE) -f vdi.mk; \
	fi

vert: FRC
	if m88k ; then \
	echo "not making vert for m88k"; \
	else \
	cd vert; $(MAKE) -f vert.mk; \
	fi

clean:
	-rm -f *.o
	cd adb; $(MAKE) -f adb.mk clean;
	cd cgi; $(MAKE) -f cgi.mk clean;
	cd dev; $(MAKE) -f dev.mk clean;
	cd dos; $(MAKE) -f dos.mk clean;
	cd lib2; $(MAKE) -f lib2.mk clean;
	cd pgc; $(MAKE) -f pgc.mk clean;
	cd opconfig; $(MAKE) -f opconfig.mk clean;
	cd opsash; $(MAKE) -f opsash.mk clean;
	cd stand; $(MAKE) -f stand.mk clean;
	cd swap; $(MAKE) -f swap.mk clean;
	cd udio; $(MAKE) -f udio.mk clean;
	cd vdi; $(MAKE) -f vdi.mk clean;
	cd vert; $(MAKE) -f vert.mk clean;

clobber: clean
	-rm -f hayes tape fsorg dbg proc opterm
	cd adb; $(MAKE) -f adb.mk clobber;
	cd cgi; $(MAKE) -f cgi.mk clobber;
	cd dev; $(MAKE) -f dev.mk clobber;
	cd dos; $(MAKE) -f dos.mk clobber;
	cd lib2; $(MAKE) -f lib2.mk clobber;
	cd pgc; $(MAKE) -f pgc.mk clobber;
	cd opconfig; $(MAKE) -f opconfig.mk clobber;
	cd opsash; $(MAKE) -f opsash.mk clobber;
	cd stand; $(MAKE) -f stand.mk clobber;
	cd swap; $(MAKE) -f swap.mk clobber;
	cd udio; $(MAKE) -f udio.mk clobber;
	cd vdi; $(MAKE) -f vdi.mk clobber;
	cd vert; $(MAKE) -f vert.mk clobber;

FRC:
