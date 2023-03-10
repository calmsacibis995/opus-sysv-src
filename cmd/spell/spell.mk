#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)spell:spell.mk	1.11"
#	spell make file

#	Note:  In using the -f flag it is assumed that either
#	both the host and the target machines need the -f, or
#	neither needs it.  If one needs it and the other does
#	not, it is assumed that the machine that does not need
#	it will treat it appropriately.

# _SH_ is used by 3B5 View-path environment

ROOT =
OL = $(ROOT)/
SL = $(ROOT)/usr/src/cmd
RDIR = ${SL}/spell
INS = :
REL = current
CSID = -r`gsid spellcode ${REL}`
DSID = -r`gsid spelldata ${REL}`
SHSID = -r`gsid spell.sh ${REL}`
CMPRSID = -r`gsid compress.sh ${REL}`
MKSID = -r`gsid spell.mk ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
CINSDIR = ${OL}usr/bin
PINSDIR = ${OL}usr/lib/spell
#IFLAG = -i
IFLAG =
SMFLAG =
CFLAGS = -O $(FFLAGS)
SFILES = spellprog.c spellin.c
DFILES = american british local list extra stop
MAKE = make

compile all: spell hlista hlistb hstop spellin spellin1 spellprog hashmake \
	hashmk1 hashcheck compress
	:

spell:	spellprog spell.sh
	cp spell.sh spell
	$(INS) $(CINSDIR) spell

spelldir:
	-mkdir ${PINSDIR}

compress:  spelldir compress.sh
	cp compress.sh compress
	${INS} ${PINSDIR} compress

spellprog: spelldir spellprog.c hash.c hashlook.c huff.c malloc.c
	$(CC) $(IFLAG) ${SMFLAG} $(CFLAGS) -s spellprog.c hash.c hashlook.c huff.c malloc.c -o spellprog
	$(INS) $(PINSDIR) spellprog

spellin1: spelldir spellin.c huff.c
	$(CC) $(IFLAG) ${SMFLAG} $(CFLAGS) -s spellin.c huff.c -o spellin1

spellin: spelldir spellin.c huff.c
	$(CC) $(IFLAG) ${SMFLAG} $(CFLAGS) -s spellin.c huff.c -o spellin
	${INS} ${PINSDIR} spellin

hashcheck: spelldir hashcheck.c hash.c huff.c
	$(CC) $(IFLAG) ${SMFLAG} $(CFLAGS) -s hashcheck.c hash.c huff.c -o hashcheck
	${INS} ${PINSDIR} hashcheck

hashmk1: spelldir hashmake.c hash.c
	$(CC) $(IFLAG) ${SMFLAG} $(CFLAGS) -s hashmake.c hash.c -o hashmk1

hashmake: spelldir hashmake.c hash.c
	$(CC) $(IFLAG) ${SMFLAG} $(CFLAGS) -s hashmake.c hash.c -o hashmake
	${INS} ${PINSDIR} hashmake

alldata: hlista hlistb hstop
	rm htemp1

htemp1:	list local extra hashmk1
	if [ "${ROOT}" != "" ]; then \
		cat list local extra | $(_SH_) /usr/bin/hashmk1 >htemp1; \
	else \
		cat list local extra | $(_SH_) ./hashmk1 >htemp1; \
	fi

hlista: spelldir american hashmake hashmk1 spellin spellin1 htemp1
	if [ "${ROOT}" != "" ]; then \
		$(_SH_) /usr/bin/hashmk1 <american |sort -u - htemp1 >htemp2; \
		$(_SH_) /usr/bin/spellin1 `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hlista; \
	else \
		$(_SH_) ./hashmk1 <american |sort -u - htemp1 >htemp2; \
		$(_SH_) ./spellin1 `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hlista; \
	fi
	$(INS) $(PINSDIR) -m 644 hlista
	rm htemp2

hlistb: spelldir british hashmk1 spellin1 htemp1
	if [ "${ROOT}" != "" ]; then \
		$(_SH_) /usr/bin/hashmk1 <british |sort -u - htemp1 >htemp2; \
		$(_SH_) /usr/bin/spellin1 `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hlistb; \
	else \
		$(_SH_) ./hashmk1 <british |sort -u - htemp1 >htemp2; \
		$(_SH_) ./spellin1 `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hlistb; \
	fi
	$(INS) $(PINSDIR) -m 644 hlistb
	rm htemp2


hstop:	spelldir stop spellin1 hashmk1
	if [ "${ROOT}" != "" ]; then \
		$(_SH_) /usr/bin/hashmk1 <stop | sort -u >htemp2; \
		$(_SH_) /usr/bin/spellin1 `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hstop; \
	else \
		$(_SH_) ./hashmk1 <stop | sort -u >htemp2; \
		$(_SH_) ./spellin1 `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hstop; \
	fi
	$(INS) $(PINSDIR) -m 644 hstop
	rm htemp2

install:  ;  ${MAKE} -f spell.mk INS="install -f" OL=${OL} all
inssh:    ;  ${MAKE} -f spell.mk INS="install -f" OL=${OL} spell
inscomp:  ;  ${MAKE} -f spell.mk INS="install -f" OL=${OL} compress
inscode:  ;  ${MAKE} -f spell.mk INS="install -f" spell OL=${OL}
insdata:  ;  ${MAKE} -f spell.mk INS="install -f" alldata OL=${OL}

listing:  ;  pr spell.mk spell.sh compress.sh ${SFILES} ${DFILES} | ${LIST}
listmk:   ;  pr spell.mk | ${LIST}
listsh:	  ;  pr spell.sh | ${LIST}
listcomp: ;  pr compress.sh | ${LIST}
listcode: ;  pr ${SFILES} | ${LIST}
listdata: ;  pr ${DFILES} | ${LIST}

build:  bldmk bldsh bldcomp bldcode blddata
	:
bldcode:  ;  get -p ${CSID} s.spell.src ${REWIRE} | ntar -d ${RDIR} -g
blddata:  ;  get -p ${DSID} s.spell.data | ntar -d ${RDIR} -g
bldsh:	  ;  get -p ${SHSID} s.spell.sh ${REWIRE} > ${RDIR}/spell.sh
bldcomp:  ;  get -p ${CMPRSID} s.compress.sh ${REWIRE} > ${RDIR}/compress.sh
bldmk:    ;  get -p ${MKSID} s.spell.mk > ${RDIR}/spell.mk

edit:	sedit dedit mkedit shedit compedit
	:
sedit:	;  get -p -e s.spell.src | ntar -g
dedit:	;  get -p -e s.spell.data | ntar -g
shedit:	;  get -e s.spell.sh
compedit: ; get -e s.compress.sh

delta:	sdelta ddelta mkdelta shdelta compdelta
	:
sdelta:
	ntar -p ${SFILES} > spell.src
	delta s.spell.src
	rm -f ${SFILES}
ddelta:
	ntar -p ${DFILES} > spell.data
	delta s.spell.data
	rm -f ${DFILES}
shdelta:
	delta s.spell.sh
compdelta: ; delta s.compress.sh

mkedit:	;  get -e s.spell.mk
mkdelta: ; delta s.spell.mk

clean:
	rm -f *.o

clobber: clean shclobber compclobber
	rm -f spell spellprog spellin hashmake hlist* hstop spellin1 hashmk1
	-rm -f htemp1 htemp2 hashcheck
shclobber: ; rm -f spell
compclobber: ; rm -f compress

delete:	clobber shdelete compdelete
	rm -f ${SFILES} ${DFILES}
shdelete: shclobber
	rm -f spell.sh
compdelete: compclobber
	rm -f compress.sh
