#	@(#)lib2B.mk	1.1
all:	Bconf.o Binit.o
	$(PFX)ar rvc lib2B.a Bconf.o Binit.o

install: all
	/etc/install -f $(ROOT)/usr/lib lib2B.a

clean:
	rm -f Bconf.o Binit.o

clobber: clean
	rm -f lib2B.a
