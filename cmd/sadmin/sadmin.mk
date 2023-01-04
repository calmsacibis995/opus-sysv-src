#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)sadmin:sadmin.mk	2.4"
#	Overall Makefile shipped administrative pieces

ROOT =	/
ADMINHOME =	/usr/admin
BIN =	/usr/bin
LBIN =	/usr/lbin
DIRS =	$(ROOT) $(ROOT)/usr $(ROOT)/install
DEVS =	$(ROOT)/dev/SA/diskette1 $(ROOT)/dev/rSA/diskette1
ETC =	/etc

PARTS =	tools shell include csub cmain check etc admin

FILES = README sadmin.mk

ALL: \
		$(PARTS)
	cd tools;  $(MAKE)
	cd shell;  $(MAKE)
	cd include;  $(MAKE)
	cd csub;  $(MAKE)
	cd cmain;  $(MAKE)
	cd check;  $(MAKE)
	cd etc;  $(MAKE)
	cd admin;  $(MAKE)

install: \
		ALL $(DIRS) $(DEVS)
	rootdir=`cd $(ROOT); pwd`;  cd shell;  make DIR=$${rootdir}$(LBIN) $@
	rootdir=`cd $(ROOT); pwd`;  cd etc;  make DIR=$${rootdir}$(ETC) $@
	rootdir=`cd $(ROOT); pwd`;  cd cmain;  make DIR=$${rootdir}$(LBIN) $@
	rootdir=`cd $(ROOT); pwd`;  cd check;  make DIR=$${rootdir}$(LBIN) $@
	rootdir=`cd $(ROOT); pwd`;  cd admin;  make BIN=$${rootdir}$(BIN) ADMINHOME=$${rootdir}$(ADMINHOME) $@

product productdir:
	@rootdir=`cd $(ROOT); pwd`; \
	cd tools;    $(MAKE) DIR=$${rootdir}$(LBIN) $@  && \
	cd ../shell;  $(MAKE) DIR=$${rootdir}$(LBIN) $@  && \
	cd ../cmain;  $(MAKE) DIR=$${rootdir}$(LBIN) $@  && \
	cd ../check;  $(MAKE) DIR=$${rootdir}$(LBIN) $@  && \
	cd ../etc;    $(MAKE) DIR=$${rootdir}$(ETC) $@  && \
	cd ../admin;  $(MAKE) BIN=$${rootdir}$(BIN) ADMINHOME=$${rootdir}$(ADMINHOME) $@

clean clobber newmakefile remove:
	rootdir=`cd $(ROOT); pwd`; cd tools;  $(MAKE) DIR=$${rootdir}$(LBIN) $@
	rootdir=`cd $(ROOT); pwd`; cd shell;  $(MAKE) DIR=$${rootdir}$(LBIN) $@
	rootdir=`cd $(ROOT); pwd`; cd include;  $(MAKE) DIR=$${rootdir}$(LBIN) $@
	rootdir=`cd $(ROOT); pwd`; cd csub;   $(MAKE) DIR=$${rootdir}$(LBIN) $@
	rootdir=`cd $(ROOT); pwd`; cd cmain;  $(MAKE) DIR=$${rootdir}$(LBIN) $@
	rootdir=`cd $(ROOT); pwd`; cd check;  $(MAKE) DIR=$${rootdir}$(LBIN) $@
	rootdir=`cd $(ROOT); pwd`; cd etc;    $(MAKE) DIR=$${rootdir}$(ETC) $@
	rootdir=`cd $(ROOT); pwd`; cd admin;  $(MAKE) BIN=$${rootdir}$(BIN) ADMINHOME=$${rootdir}$(ADMINHOME) $@

$(DIRS):
	mkdir $@;  chmod 775 $@

$(DEVS):
	@echo The following device needs to be created: $@
	@echo It is created by linking the largest removable disk slice to $@ .

partslist:	$(PARTS)
	@echo $(FILES)  |  tr ' ' '\012'
	@rootdir=`cd $(ROOT); pwd`; \
	cd tools;    $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;tools/;'  && \
	cd ../shell;    $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;shell/;'  && \
	cd ../include;  $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;include/;'  && \
	cd ../csub;  $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;csub/;'  && \
	cd ../cmain;  $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;cmain/;'  && \
	cd ../check;  $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;check/;'  && \
	cd ../etc;    $(MAKE) DIR=$${rootdir}$(ETC) $@  |  sed 's;^;etc/;'  && \
	cd ../admin;  $(MAKE) BIN=$${rootdir}$(BIN) ADMINHOME=$${rootdir}$(ADMINHOME) $@  |  sed 's;^;admin/;'  && \

srcaudit:
	@ls -d $(PARTS) $(FILES) 2>&1 >/dev/null
	@rootdir=`cd $(ROOT); pwd`; \
	cd tools;    $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;tools/;'  && \
	cd ../shell;    $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;shell/;'  && \
	cd ../include;  $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;include/;'  && \
	cd ../csub;  $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;csub/;'  && \
	cd ../cmain;  $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;cmain/;'  && \
	cd ../check;  $(MAKE) DIR=$${rootdir}$(LBIN) $@  |  sed 's;^;check/;'  && \
	cd ../etc;    $(MAKE) DIR=$${rootdir}$(ETC) $@  |  sed 's;^;etc/;'  && \
	cd ../admin;  $(MAKE) BIN=$${rootdir}$(BIN) ADMINHOME=$${rootdir}$(ADMINHOME) $@  |  sed 's;^;admin/;'  && \

proto:
	$(MAKE) -f sadmin.mk ROOT=$(ROOT) productdir product  |  \
		tools/mkproto -u0 -g3 >PROTO

cmdsused:
	rootdir=`cd $(ROOT); pwd`; \
	( cd shell;    $(MAKE) partslist  |  sed 's;^;shell/;'  && \
	cd ../etc;    $(MAKE) partslist  |  sed 's;^;etc/;'  && \
	cd ../admin;  $(MAKE) partslist |  sed 's;^;admin/;' )  | \
	xargs cat  |  tools/cmdsused
