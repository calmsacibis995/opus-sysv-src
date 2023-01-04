#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)xenv:m88k/template.mk	1.35"
#
#	WE-32000 GLOBAL MAKEFILE
#
#
#	SGS indicates the prefix to be applied to the installed commands.
SGS=m88k
#
#	ARCH indicates the architecture of the host machine
#		AR16WR=11/70, AR32WR=vax, AR32W=ibm, 3b20, etc
ARCH=AR32W
#
#	The following macros define the various installation directories.
#	Note that if relative pathnames are used, they must allow for the
#	"cd" issued prior to each make.
#
#	BASE is the directory under which all the sgs source lives
BASE= ../..
#
#	BINDIR is the directory for installing executable ("bin") files.
#	It should correspond to the same directory as specified in AS and LD
#	in paths.h.
BINDIR=
#
#	LIBDIR is the directory for installing libraries and executable
#	files not intended for direct user execution (e.g., assembler pass1).
#	It should correspond to the same directory as specified in COMP,
#	OPTIM, AS1, and AS2 in paths.h.
LIBDIR=
#
#	INCDIR is the directory for installing include files used in
#	developing the SGS and which are needed by certain users of the
#	SGS (e.g., operating system developers).  It should correspond to
#	the same directory as specified in INCDIR in paths.h.
INCDIR=
#
#	Specify the byte order for this SGS instance.
#		FBO = Forward Byte Order (3B20, IBM)
#		RBO = Reverse Byte Order (DEC)
DBO=FBO
#
#	If host machine is running a pre-5.0 release of UNIX
#	then set Archive Format to "OLDAR".
#	Starting with the 5.0 UNIX (SVR1)  release the Archive Format
#	should be set to "PORTAR".
#	If building a 5.0 release on a host that is running
#	System V Rel. 2.0 (SVR2), then set ARFORMAT to PORT5AR.
#
ARFORMAT=PORTAR
#
#	Starting with the SVR2 release of UNIX,
#	if flexnames feature is desired, then set FLEX
#	to -DFLEXNAMES.
#	If host machine is running a pre-SVR2 release of UNIX
#	then set FLEX to null (ie. FLEX= ).
#
FLEX=-DFLEXNAMES
#
#	MAC parameter specifies the set of software workarounds
#	to be produced by the new sgs.
#	MAC=ABWORMAC produces sgs for WE32001 and WE32100 chips.
#	MAC=BMAC produces sgs for WE32100  o n l y.
#
MAC=
#	This is the machine ID field. The valid values are
#	u3b15, u3b5 or u3b2.
MACH=
#
#	The following parameter specifies the default include directory
#	for cpp. If not defined the value will be ROOT/usr/include.
#
USRINC=
NOUSRINC=NOUSRINC
#
#
OWN=bin
GRP=bin
#
#
MAKE=make
CC=cc
FFLAG=
ENV=
ROOT=
VERSION=
LDLIBS=
LIBLD=$(BASE)/libld/m88k/libld.a
#
#
ENVPARMS=MAKE="$(MAKE)" SGS="$(SGS)" ARCH="$(ARCH)" OWN="$(OWN)" GRP="$(GRP)" \
	DBO="$(DBO)" ARFORMAT="$(ARFORMAT)" FLEX="$(FLEX)" LDLIBS="$(LDLIBS)"

CPPARMS=CC="$(CC)" FFLAG="$(FFLAG)" ENV="$(ENV)" ROOT="$(ROOT)" \
	VERSION="$(VERSION)" USRINC="$(USRINC)"

CCPARMS=CC="$(CC)" FFLAG="$(FFLAG)" ENV="$(ENV)" ROOT="$(ROOT)" \
	VERSION="$(VERSION)" USRINC="$(NOUSRINC)"

DIRPARMS=BINDIR="$(ROOT)/$(BINDIR)" LIBDIR="$(ROOT)/$(LIBDIR)"
#
#
all:	libs
	cd $(BASE)/ar/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) 
	cd $(BASE)/as/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) 
	cd $(BASE)/comp/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) 
	cd $(BASE)/ld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	cd $(BASE)/reorder/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	cd $(BASE)/cpp/m88k; $(MAKE) $(ENVPARMS) $(CPPARMS) PD_SYS=D_unix \
		PD_MACH=D_newmach NEW_MACH="-DPD_MY_MACH=\\\"$(MACH)\\\""
	cd $(BASE)/cmd/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	cd $(BASE)/dump/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	cd $(BASE)/dis/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	cd $(BASE)/lorder/m88k; $(MAKE) $(ENVPARMS)
	cd $(BASE)/mkshlib/m88k; $(MAKE) $(ENVPARMS)
	cd $(BASE)/nm/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	#cd $(BASE)/sim/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	cd $(BASE)/size/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	#cd $(BASE)/sld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	cd $(BASE)/strip/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	cd $(BASE)/sifilter/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	cd $(BASE)/unix_conv/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
	cd $(BASE)/../make; /bin/make -f make.mk
#
libs:
	cd $(BASE)/libld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)
#
first:
	if [ ! -d $(BINDIR) ] ; then mkdir $(BINDIR) ; fi 
	if [ ! -d $(LIBDIR) ] ; then mkdir $(LIBDIR) ; fi 
	if [ ! -d $(INCDIR) ] ; then mkdir $(INCDIR) ; fi 
#
install: libinstall
	cd $(BASE)/ar/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	cd $(BASE)/as/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	cd $(BASE)/comp/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	cd $(BASE)/ld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	#cd $(BASE)/reorder/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	cd $(BASE)/cpp/m88k; $(MAKE) $(ENVPARMS) $(CPPARMS) PD_SYS=D_unix \
		PD_MACH=D_newmach NEW_MACH="-DPD_MY_MACH=\\\"$(MACH)\\\"" \
		$(DIRPARMS) install
	cd $(BASE)/cmd/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	cd $(BASE)/dump/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	cd $(BASE)/dis/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	cd $(BASE)/lorder/m88k; $(MAKE) $(ENVPARMS) $(DIRPARMS) install
	cd $(BASE)/mkshlib/m88k; $(MAKE) $(ENVPARMS) $(DIRPARMS) install
	cd $(BASE)/nm/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	#cd $(BASE)/sim/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	cd $(BASE)/size/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	#cd $(BASE)/sld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	cd $(BASE)/strip/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	cd $(BASE)/sifilter/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
	cd $(BASE)/unix_conv/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) install
	if [ "$(SGS)" != "" ] ; then \
		cp $(BASE)/../make/make $(BINDIR)/m88kmake ; \
		cp $(BASE)/xenv/m88k/env-m88k $(BINDIR)/$(SGS)env ; \
		mv $(BINDIR)/make $(BINDIR)/make.old ; \
		cp $(BASE)/xenv/m88k/make.tmp $(BINDIR)/make ; \
	fi
	cp $(BASE)/xenv/m88k/vax $(BINDIR)
	cp $(BASE)/xenv/m88k/u3b $(BINDIR)
	cp $(BASE)/xenv/m88k/u370 $(BINDIR)
	cp $(BASE)/xenv/m88k/u3b15 $(BINDIR)
	cp $(BASE)/xenv/m88k/u3b5 $(BINDIR)
	cp $(BASE)/xenv/m88k/u3b2 $(BINDIR)
#
libinstall:
	cd $(BASE)/libld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) $(DIRPARMS) install
#
save:
	cd $(BASE)/ar/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)  save
	cd $(BASE)/as/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)  save
	cd $(BASE)/comp/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)  save
	cd $(BASE)/ld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	cd $(BASE)/reorder/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	cd $(BASE)/cpp/m88k; $(MAKE) $(ENVPARMS) $(CPPARMS) PD_SYS=D_unix \
		PD_MACH=D_newmach NEW_MACH="-DPD_MY_MACH=\\\"$(MACH)\\\"" save
	cd $(BASE)/cmd/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	cd $(BASE)/dump/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	cd $(BASE)/dis/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	cd $(BASE)/lorder/m88k; $(MAKE) $(ENVPARMS) save
	cd $(BASE)/mkshlib/m88k; $(MAKE) $(ENVPARMS) save
	cd $(BASE)/nm/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	#cd $(BASE)/sim/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	cd $(BASE)/size/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	#cd $(BASE)/sld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	cd $(BASE)/strip/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	cd $(BASE)/sifilter/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	cd $(BASE)/unix_conv/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) save
	cd $(BASE)/../make; /bin/make -f make.mk save
#
uninstall:
	cd $(BASE)/ar/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)  uninstall
	cd $(BASE)/as/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)  uninstall
	cd $(BASE)/comp/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)  uninstall
	cd $(BASE)/ld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	cd $(BASE)/reorder/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	cd $(BASE)/cpp/m88k; $(MAKE) $(ENVPARMS) $(CPPARMS) PD_SYS=D_unix \
		PD_MACH=D_newmach NEW_MACH="-DPD_MY_MACH=\\\"$(MACH)\\\"" uninstall
	cd $(BASE)/cmd/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	cd $(BASE)/dump/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	cd $(BASE)/dis/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	cd $(BASE)/lorder/m88k; $(MAKE) $(ENVPARMS) uninstall
	cd $(BASE)/mkshlib/m88k; $(MAKE) $(ENVPARMS) uninstall
	cd $(BASE)/nm/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	#cd $(BASE)/sim/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	cd $(BASE)/size/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	#cd $(BASE)/sld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	cd $(BASE)/strip/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	cd $(BASE)/sifilter/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	cd $(BASE)/unix_conv/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) uninstall
	cd $(BASE)/../make; /bin/make -f make.mk uninstall
#
shrink: libshrink
	cd $(BASE)/ar/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)  clobber
	cd $(BASE)/as/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)  clobber
	cd $(BASE)/comp/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS)  clobber
	cd $(BASE)/ld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) clobber
	cd $(BASE)/reorder/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) clobber
	cd $(BASE)/cpp/m88k; $(MAKE) $(ENVPARMS) $(CPPARMS) PD_SYS=D_unix \
		PD_MACH=D_newmach NEW_MACH="-DPD_MY_MACH=\\\"$(MACH)\\\"" shrink 
	cd $(BASE)/cmd/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) shrink
	cd $(BASE)/dump/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) shrink
	cd $(BASE)/dis/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) clobber
	cd $(BASE)/lorder/m88k; $(MAKE) $(ENVPARMS) shrink
	cd $(BASE)/mkshlib/m88k; $(MAKE) $(ENVPARMS) shrink
	cd $(BASE)/nm/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) shrink
	#cd $(BASE)/sim/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) clobber
	cd $(BASE)/size/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) shrink
	#cd $(BASE)/sld/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) clobber
	cd $(BASE)/strip/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) shrink
	cd $(BASE)/sifilter/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) clobber
	cd $(BASE)/unix_conv/m88k; $(MAKE) $(ENVPARMS) $(CCPARMS) shrink
	cd $(BASE)/../make; /bin/make -f make.mk clobber
#
libshrink:
	cd $(BASE)/libld/m88k; $(MAKE) $(ENVPARMS) shrink
