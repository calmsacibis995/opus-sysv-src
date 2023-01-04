#ident	"@(#)sgs:sgs.mk.um32	1.3"
#	3b15/3b5/3b2 Cross-SGS Global Makefile
#	PATHEDIT MUST BE RUN BEFORE THIS MAKEFILE IS USED!
#
#

BINDIR=
SGS=

all:	install libs
libs:
	cd ../../lib; $(BINDIR)/$(SGS)env make -f lib.mk install
	cd xenv/clipper; \
		$(BINDIR)/$(SGS)env make libshrink; \
		$(BINDIR)/$(SGS)env make libinstall; \
		$(BINDIR)/$(SGS)env make libshrink;

install:  sgs
	cd xenv/clipper; make install

first:
	cd xenv/clipper; make first

sgs:	
	cd xenv/clipper; $(MAKE) all

uninstall:
	cd xenv/clipper; make uninstall

save:
	cd xenv/clipper; make save

shrink:	clobber
	if [ -f "$(BINDIR)/$(SGS)env" ] ; \
	then \
		cd ../../lib; $(BINDIR)/$(SGS)env make -f lib.mk clobber ; \
	fi

clobber:
	cd xenv/clipper; make shrink
