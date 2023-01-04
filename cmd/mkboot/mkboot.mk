#ident	"@(#)mkboot-:mkboot.mk	1.6"
INC = $(ROOT)/usr/include
ROOT =
INS = install

mkboot:
	cp mkboot.sh mkboot

install:mkboot
	$(INS) -f $(ROOT)/etc mkboot

clobber:clean
	rm -f mkboot

clean:
	rm -f mkboot

