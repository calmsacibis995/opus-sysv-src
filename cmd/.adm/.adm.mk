#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)adm:.adm.mk	1.18.1.2"

ROOT =
LIB = $(ROOT)/usr/lib
CRONTABS = $(ROOT)/usr/spool/cron/crontabs
LIBCRON = $(LIB)/cron
INSDIR = $(ROOT)/etc
TOUCH=/bin/touch

CRON_ENT= adm root sys sysadm

CRON_LIB= .proto at.allow cron.allow queuedefs

ETC_SCRIPTS= checkall filesave gettydefs \
	group ioctl.syscon master motd passwd powerfail \
	rc shutdown tapesave \
	shadow

all:	etc_scripts crontab cronlib

crontab: $(CRON_ENT)

cronlib: $(CRON_LIB)

etc_scripts: $(ETC_SCRIPTS)

clean:

clobber: clean

install:
	make -f .adm.mk $(ARGS)

adm::
	cp adm $(CRONTABS)/adm
	$(CH)chmod 644 $(CRONTABS)/adm
	$(CH)chgrp sys $(CRONTABS)/adm
	$(TOUCH) 0101000070 $(CRONTABS)/adm
	$(CH)chown root $(CRONTABS)/adm

root::
	cp root $(CRONTABS)/root
	$(CH)chmod 644 $(CRONTABS)/root
	$(CH)chgrp sys $(CRONTABS)/root
	$(TOUCH) 0101000070 $(CRONTABS)/root
	$(CH)chown root $(CRONTABS)/root

sys::
	cp sys $(CRONTABS)/sys
	$(CH)chmod 644 $(CRONTABS)/sys
	$(CH)chgrp sys $(CRONTABS)/sys
	$(TOUCH) 0101000070 $(CRONTABS)/sys
	$(CH)chown root $(CRONTABS)/sys

sysadm::
	cp sysadm $(CRONTABS)/sysadm
	$(CH)chmod 644 $(CRONTABS)/sysadm
	$(CH)chgrp sys $(CRONTABS)/sysadm
	$(TOUCH) 0101000070 $(CRONTABS)/sysadm
	$(CH)chown root $(CRONTABS)/sysadm

.proto::
	cp .proto $(LIBCRON)/.proto
	$(CH)chmod 744 $(LIBCRON)/.proto
	$(CH)chgrp sys $(LIBCRON)/.proto
	$(TOUCH) 0101000070 $(LIBCRON)/.proto
	$(CH)chown root $(LIBCRON)/.proto

at.allow::
	cp at.allow $(LIBCRON)/at.allow
	$(CH)chmod 644 $(LIBCRON)/at.allow
	$(CH)chgrp sys $(LIBCRON)/at.allow
	$(TOUCH) 0101000070 $(LIBCRON)/at.allow
	$(CH)chown root $(LIBCRON)/at.allow

cron.allow::
	cp cron.allow $(LIBCRON)/cron.allow
	$(CH)chmod 644 $(LIBCRON)/cron.allow
	$(CH)chgrp sys $(LIBCRON)/cron.allow
	$(TOUCH) 0101000070 $(LIBCRON)/cron.allow
	$(CH)chown root $(LIBCRON)/cron.allow

queuedefs::
	cp queuedefs $(LIBCRON)/queuedefs
	$(CH)chmod 644 $(LIBCRON)/queuedefs
	$(CH)chgrp sys $(LIBCRON)/queuedefs
	$(TOUCH) 0101000070 $(LIBCRON)/queuedefs
	$(CH)chown root $(LIBCRON)/queuedefs


bcheckrc::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	fi;\
	cp bcheckrc.sh $(INSDIR)/bcheckrc;\
	$(CH)chmod 744 $(INSDIR)/bcheckrc;\
	$(CH)chgrp sys $(INSDIR)/bcheckrc;\
	$(TOUCH) 0101000070 $(INSDIR)/bcheckrc;\
	$(CH)chown root $(INSDIR)/bcheckrc

brc::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	fi;\
	cp brc.sh $(INSDIR)/brc;\
	$(CH)chmod 744 $(INSDIR)/brc;\
	$(CH)chgrp sys $(INSDIR)/brc;\
	$(TOUCH) 0101000070 $(INSDIR)/brc;\
	$(CH)chown root $(INSDIR)/brc

checkall::
	-if vax || pdp11 || u3b5 || u3b || clipper || ns32000 || m88k;\
	then\
		cp checkall.sh $(INSDIR)/checkall;\
		$(CH)chmod 744 $(INSDIR)/checkall;\
		$(CH)chgrp bin $(INSDIR)/checkall;\
		$(TOUCH) 0101000070 $(INSDIR)/checkall;\
		$(CH)chown bin $(INSDIR)/checkall;\
	fi

filesave::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	fi;\
	cp filesave.sh $(INSDIR)/filesave;\
	$(CH)chmod 744 $(INSDIR)/filesave;\
	$(CH)chgrp sys $(INSDIR)/filesave;\
	$(TOUCH) 0101000070 $(INSDIR)/filesave;\
	$(CH)chown root $(INSDIR)/filesave

gettydefs::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	else cd u3b2;\
	fi;\
	cp gettydefs $(INSDIR)/gettydefs;\
	$(CH)chmod 644 $(INSDIR)/gettydefs;\
	$(CH)chgrp sys $(INSDIR)/gettydefs;\
	$(TOUCH) 0101000070 $(INSDIR)/gettydefs;\
	$(CH)chown root $(INSDIR)/gettydefs

group::
	cp group $(INSDIR)/group
	$(CH)chmod 644 $(INSDIR)/group
	$(CH)chgrp sys $(INSDIR)/group
	$(TOUCH) 0101000070 $(INSDIR)/group
	$(CH)chown root $(INSDIR)/group

ioctl.syscon::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	else cd u3b2;\
	fi;\
	cp ioctl.syscon $(INSDIR)/ioctl.syscon;\
	$(CH)chmod 644 $(INSDIR)/ioctl.syscon;\
	$(CH)chgrp sys $(INSDIR)/ioctl.syscon;\
	$(TOUCH) 0101000070 $(INSDIR)/ioctl.syscon;\
	$(CH)chown root $(INSDIR)/ioctl.syscon

master::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	fi;\
	cp master $(INSDIR)/master;\
	$(CH)chmod 644 $(INSDIR)/master;\
	$(CH)chgrp sys $(INSDIR)/master;\
	$(CH)chown root $(INSDIR)/master

motd::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	else cd u3b2;\
	fi;\
	sed 1d motd > $(INSDIR)/motd;\
	$(CH)chmod 644 $(INSDIR)/motd;\
	$(CH)chgrp sys $(INSDIR)/motd;\
	$(CH)chown root $(INSDIR)/motd

passwd::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	else cd u3b2;\
	fi;\
	cp passwd $(INSDIR)/passwd;\
	$(CH)chmod 644 $(INSDIR)/passwd;\
	$(CH)chgrp sys $(INSDIR)/passwd;\
	$(TOUCH) 0101000070 $(INSDIR)/passwd;\
	$(CH)chown root $(INSDIR)/passwd

shadow::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	else cd u3b2;\
	fi;\
	cp shadow $(INSDIR)/shadow;\
	$(CH)chmod 400 $(INSDIR)/shadow;\
	$(CH)chgrp sys $(INSDIR)/shadow;\
	$(TOUCH) 0101000070 $(INSDIR)/shadow;\
	$(CH)chown root $(INSDIR)/shadow

powerfail::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	fi;\
	cp powerfail.sh $(INSDIR)/powerfail;\
	$(CH)chmod 744 $(INSDIR)/powerfail;\
	$(CH)chgrp sys $(INSDIR)/powerfail;\
	$(TOUCH) 0101000070 $(INSDIR)/powerfail;\
	$(CH)chown root $(INSDIR)/powerfail

rc::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	fi;\
	cp rc.sh $(INSDIR)/rc;\
	$(CH)chmod 744 $(INSDIR)/rc;\
	$(CH)chgrp sys $(INSDIR)/rc;\
	$(TOUCH) 0101000070 $(INSDIR)/rc;\
	$(CH)chown root $(INSDIR)/rc

shutdown::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	fi;\
	cp shutdown.sh $(INSDIR)/shutdown;\
	$(CH)chmod 744 $(INSDIR)/shutdown;\
	$(CH)chgrp sys $(INSDIR)/shutdown;\
	$(TOUCH) 0101000070 $(INSDIR)/shutdown;\
	$(CH)chown root $(INSDIR)/shutdown

tapesave::
	-if vax;\
	then cd vax;\
	elif pdp11;\
	then cd pdp11;\
	elif u3b5;\
	then cd u3b5;\
	elif u3b;\
	then cd u3b;\
	elif ns32000 || clipper || m88k;\
	then cd opus;\
	fi;\
	cp tapesave.sh $(INSDIR)/tapesave;\
	$(CH)chmod 744 $(INSDIR)/tapesave;\
	$(CH)chgrp sys $(INSDIR)/tapesave;\
	$(TOUCH) 0101000070 $(INSDIR)/tapesave;\
	$(CH)chown root $(INSDIR)/tapesave
