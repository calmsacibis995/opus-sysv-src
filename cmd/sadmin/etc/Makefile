#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)sadmin:etc/Makefile	2.5"
#	Makefile for etc
#	Generated Thu Aug 22 10:25:13 EDT 1985 from skeleton makefile:
#  sadmin:etc/SKELMakefile  2.1  /sccs/src/cmd/sadmin/etc/s.SKELMakefile

ROOT = /

DIR = $(ROOT)/etc

CMDS = savecpio 

DIRS = save.d

PARTS = $(CMDS) TIMEZONE disketteparm profile rc.d/0_firstcheck rc.d/setup \
save.d/except stdprofile 

ALL:		$(PARTS)

install:	$(PARTS) $(DIR) remove
	umask 022;  ls $(PARTS)  |  cpio -pdu $(DIR)
	-if m88k ; then \
		cp savecpio.m88k $(DIR)/savecpio; \
	fi
	cd $(DIR);  chmod go-w,u+w $(PARTS)
	cd $(DIR);  chmod +x $(CMDS)

$(DIR):
	mkdir $(DIR);  chmod 755 $(DIR)

remove:
	cd $(DIR);  rm -f $(PARTS)
	-cd $(DIR);  if test -d $(DIRS); then if test `ls $(DIRS) | wc -l` -eq 0; then rmdir $(DIRS); fi; fi

partslist:
	@echo Makefile SKELMakefile $(PARTS)  |  tr ' ' '\012'  |  sort
	
product:
	@echo $(PARTS)  |  tr ' ' '\012'  |  sed "s;^;$(DIR)/;"

productdir:
	@echo $(DIR)
	@echo $(DIRS)  |  tr ' ' '\012'  |  sed "s;^;$(DIR)/;"

clean:

clobber:

srcaudit:
	@(	find * -print;  \
		ls -d $(DIRS) $(PARTS) SKELMakefile Makefile  \
	)  |  sort  |  uniq -u  |  sed 's/$$/	unexpected/'

newmakefile:	../tools/genmakefile SKELMakefile
	cp Makefile OMakefile
	../tools/genmakefile SKELMakefile *Makefile >Makefile
	rm -f OMakefile
