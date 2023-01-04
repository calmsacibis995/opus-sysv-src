
===== The following commands/libraries failed =====
Some that we don't care about have been filtered out

Mail
net.admin
sh
tail

======== Mail
cd Mail
**** Build of Mail failed (no makefile found in directory)

======== net.admin
cd net.admin
**** Build of net.admin failed (no makefile found in directory)

======== sh
cd sh
make -b -f sh.mk ARGS="" LDLIBS='' IFLAG="" FFLAG="" CPU="pm400" install
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c main.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c args.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c blok.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c cmd.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c defs.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c error.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c fault.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c hash.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c hashserv.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c io.c
	if [ "" ];\
		 then \
			 `if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c msg.c; \
		 else \
			 CC=`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` ; export CC; \
			 sh ./:fix msg; \
		 fi
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c name.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c print.c
/usr/include/sys/param.h: 157: MAX redefined
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -I/usr/src/cmd/acct -c service.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c stak.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c string.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c word.c
	set +e; if u370;\
		 then\
		 	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -b1,0 -c xec.c;\
		 else\
			`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c xec.c;\
		 fi
	if [ "" ];\
		 then \
			 `if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c ctype.c; \
		 else \
			 CC=`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` ; export CC; \
			 sh ./:fix ctype; \
		 fi
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c echo.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c expand.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c func.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c macro.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c pwd.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c setbrk.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c test.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c main.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi`  -o sh.bck  main.o args.o blok.o cmd.o defs.o error.o \
	fault.o hash.o hashserv.o io.o msg.o name.o print.o service.o stak.o \
	string.o word.o xec.o ctype.o echo.o expand.o func.o macro.o pwd.o \
	setbrk.o test.o   -s -lgen
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -DNOPASSIDS -c main.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi`  -o sh.sec  main.o args.o blok.o cmd.o defs.o error.o \
	fault.o hash.o hashserv.o io.o msg.o name.o print.o service.o stak.o \
	string.o word.o xec.o ctype.o echo.o expand.o func.o macro.o pwd.o \
	setbrk.o test.o   -s -lgen
	install -f /usr/adm -m 444 sh.bck
sh.bck installed as /usr/adm/sh.bck
	install -f /usr/adm -m 444 sh.sec
sh.sec installed as /usr/adm/sh.sec
	cp ./sh.sec ./sh
	install -o -m 555 -u bin -g root -f /bin sh
new owner is bin
mv: cannot unlink /bin/OLDsh
mv: Text file busy
cp: cannot create /bin/sh
cp: Text file busy
install: cp /bin/OLDsh /bin/sh failed
*** Error code 2

Stop.
make -b -f sh.mk LDLIBS='' IFLAG="" FFLAG="" CPU="pm400"  clobber
	rm -f args.o blok.o cmd.o ctype.o defs.o echo.o error.o expand.o  fault.o func.o hash.o hashserv.o io.o macro.o main.o msg.o name.o  print.o profile.o pwd.o service.o setbrk.o stak.o string.o test.o  word.o xec.o sh.bck sh.sec
**** Build of sh failed (make)

======== tail
cd tail
make -b -f tail.mk ARGS="" LDLIBS='' IFLAG="" FFLAG="" CPU="pm400" install
	cc -O -I/usr/include -c tail.c
	cc -O -I/usr/include -o tail tail.o -s 
	cpset tail /bin
cpset: Couldn't unlink '/bin/tail'
*** Error code 1

Stop.
make -b -f tail.mk LDLIBS='' IFLAG="" FFLAG="" CPU="pm400"  clobber
	rm -f tail.o tail
**** Build of tail failed (make)

======== Mail
cd Mail
**** Build of Mail failed (no makefile found in directory)

======== net.admin
cd net.admin
**** Build of net.admin failed (no makefile found in directory)

======== sh
cd sh
make -b -f sh.mk ARGS="" LDLIBS='' IFLAG="" FFLAG="" CPU="pm400" install
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c main.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c args.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c blok.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c cmd.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c defs.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c error.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c fault.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c hash.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c hashserv.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c io.c
	if [ "" ];\
		 then \
			 `if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c msg.c; \
		 else \
			 CC=`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` ; export CC; \
			 sh ./:fix msg; \
		 fi
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c name.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c print.c
/usr/include/sys/param.h: 157: MAX redefined
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -I/usr/src/cmd/acct -c service.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c stak.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c string.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c word.c
	set +e; if u370;\
		 then\
		 	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -b1,0 -c xec.c;\
		 else\
			`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c xec.c;\
		 fi
	if [ "" ];\
		 then \
			 `if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c ctype.c; \
		 else \
			 CC=`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` ; export CC; \
			 sh ./:fix ctype; \
		 fi
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c echo.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c expand.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c func.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c macro.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c pwd.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c setbrk.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c test.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -c main.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi`  -o sh.bck  main.o args.o blok.o cmd.o defs.o error.o \
	fault.o hash.o hashserv.o io.o msg.o name.o print.o service.o stak.o \
	string.o word.o xec.o ctype.o echo.o expand.o func.o macro.o pwd.o \
	setbrk.o test.o   -s -lgen
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi` -DNOPASSIDS -c main.c
	`if m88k; then echo '/usr/bin/cc' ; else echo cc ; fi` -O -I/usr/include -DNICE -DACCT -DNICEVAL=4 `if m88k;then echo '-W0,-P';fi`  -o sh.sec  main.o args.o blok.o cmd.o defs.o error.o \
	fault.o hash.o hashserv.o io.o msg.o name.o print.o service.o stak.o \
	string.o word.o xec.o ctype.o echo.o expand.o func.o macro.o pwd.o \
	setbrk.o test.o   -s -lgen
	install -f /usr/adm -m 444 sh.bck
sh.bck installed as /usr/adm/sh.bck
	install -f /usr/adm -m 444 sh.sec
sh.sec installed as /usr/adm/sh.sec
	cp ./sh.sec ./sh
	install -o -m 555 -u bin -g root -f /bin sh
new owner is bin
mv: cannot unlink /bin/OLDsh
mv: Text file busy
cp: cannot create /bin/sh
cp: Text file busy
install: cp /bin/OLDsh /bin/sh failed
*** Error code 2

Stop.
make -b -f sh.mk LDLIBS='' IFLAG="" FFLAG="" CPU="pm400"  clobber
	rm -f args.o blok.o cmd.o ctype.o defs.o echo.o error.o expand.o  fault.o func.o hash.o hashserv.o io.o macro.o main.o msg.o name.o  print.o profile.o pwd.o service.o setbrk.o stak.o string.o test.o  word.o xec.o sh.bck sh.sec
**** Build of sh failed (make)

======== tail
cd tail
make -b -f tail.mk ARGS="" LDLIBS='' IFLAG="" FFLAG="" CPU="pm400" install
	cc -O -I/usr/include -c tail.c
	cc -O -I/usr/include -o tail tail.o -s 
	cpset tail /bin
cpset: Couldn't unlink '/bin/tail'
*** Error code 1

Stop.
make -b -f tail.mk LDLIBS='' IFLAG="" FFLAG="" CPU="pm400"  clobber
	rm -f tail.o tail
**** Build of tail failed (make)

