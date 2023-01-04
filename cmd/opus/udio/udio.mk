# @(#)udio.mk	2.1 

ROOT = 
udiodaemon : udio.c
	cc -O -o udiodaemon udio.c

install : udiodaemon
	cp udiodaemon $(ROOT)/opus/bin
	chown root $(ROOT)/opus/bin/udiodaemon
	chgrp sys $(ROOT)/opus/bin/udiodaemon
	chmod 4755 $(ROOT)/opus/bin/udiodaemon

clobber:
clean:
	rm -f udiodaemon
