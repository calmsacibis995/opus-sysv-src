#ident	"@(#)sgs:sgs.mk.um88k	1.3"
#	3b15/3b5/3b2 Cross-SGS Global Makefile
#	PATHEDIT MUST BE RUN BEFORE THIS MAKEFILE IS USED!
#
#

SGS=

all:	install libs
libs:
	cd ../../lib; make -f lib.mk install

install:  sgs
	cd xenv/m88k; make BINDIR=$(ROOT)/bin LIBDIR=$(ROOT)/lib install

first:
	cd xenv/m88k; make first

sgs:	
	cd xenv/m88k; $(MAKE) all

uninstall:
	cd xenv/m88k; make uninstall

save:
	cd xenv/m88k; make save

shrink:	clobber
	if [ true ] ; \
	then \
		cd ../../lib; make -f lib.mk clobber ; \
	fi

clobber:
	cd xenv/m88k; make shrink
