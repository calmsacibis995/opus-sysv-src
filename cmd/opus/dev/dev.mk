
ROOT =
INSDIR = $(ROOT)/dev

all:	install

install: cgi cpt dos rmt sxt udio vdi graph ses

cgi:
	-rm $(INSDIR)/cgi
	mknod $(INSDIR)/cgi c 15 240
	chmod 666 $(INSDIR)/cgi
	chown root $(INSDIR)/cgi
	chgrp sys $(INSDIR)/cgi

cpt:
	-rm $(INSDIR)/cpt
	mknod $(INSDIR)/cpt c 23 0
	chmod 666 $(INSDIR)/cpt
	chown root $(INSDIR)/cpt
	chgrp sys $(INSDIR)/cpt

dos:
	-rm $(INSDIR)/dos
	mknod $(INSDIR)/dos c 13 0
	chmod 666 $(INSDIR)/dos
	chown root $(INSDIR)/dos
	chgrp sys $(INSDIR)/dos

rmt:
	-rm -rf $(INSDIR)/rmt
	mkdir $(INSDIR)/rmt
	chmod 775 $(INSDIR)/rmt
	chown root $(INSDIR)/rmt
	chgrp sys $(INSDIR)/rmt
	mknod $(INSDIR)/rmt/0m c 18 0
	chmod 666 $(INSDIR)/rmt/0m
	chown root $(INSDIR)/rmt/0m
	chgrp sys $(INSDIR)/rmt/0m
	mknod $(INSDIR)/rmt/0mn c 18 4
	chmod 666 $(INSDIR)/rmt/0mn
	chown root $(INSDIR)/rmt/0mn
	chgrp sys $(INSDIR)/rmt/0mn
	mknod $(INSDIR)/rmt/1m c 18 1
	chmod 666 $(INSDIR)/rmt/1m
	chown root $(INSDIR)/rmt/1m
	chgrp sys $(INSDIR)/rmt/1m
	mknod $(INSDIR)/rmt/1mn c 18 5
	chmod 666 $(INSDIR)/rmt/1mn
	chown root $(INSDIR)/rmt/1mn
	chgrp sys $(INSDIR)/rmt/1mn

sxt:
	-rm -rf $(INSDIR)/sxt
	mkdir $(INSDIR)/sxt
	chmod 775 $(INSDIR)/sxt
	chown root $(INSDIR)/sxt
	chgrp sys $(INSDIR)/sxt
	mknod $(INSDIR)/sxt/000 c 12 0
	chmod 622 $(INSDIR)/sxt/000
	chown root $(INSDIR)/sxt/000
	chgrp sys $(INSDIR)/sxt/000
	mknod $(INSDIR)/sxt/001 c 12 1
	chmod 622 $(INSDIR)/sxt/001
	chown root $(INSDIR)/sxt/001
	chgrp sys $(INSDIR)/sxt/001
	mknod $(INSDIR)/sxt/002 c 12 2
	chmod 622 $(INSDIR)/sxt/002
	chown root $(INSDIR)/sxt/002
	chgrp sys $(INSDIR)/sxt/002
	mknod $(INSDIR)/sxt/003 c 12 3
	chmod 622 $(INSDIR)/sxt/003
	chown root $(INSDIR)/sxt/003
	chgrp sys $(INSDIR)/sxt/003
	mknod $(INSDIR)/sxt/004 c 12 4
	chmod 622 $(INSDIR)/sxt/004
	chown root $(INSDIR)/sxt/004
	chgrp sys $(INSDIR)/sxt/004
	mknod $(INSDIR)/sxt/005 c 12 5
	chmod 622 $(INSDIR)/sxt/005
	chown root $(INSDIR)/sxt/005
	chgrp sys $(INSDIR)/sxt/005
	mknod $(INSDIR)/sxt/006 c 12 6
	chmod 622 $(INSDIR)/sxt/006
	chown root $(INSDIR)/sxt/006
	chgrp sys $(INSDIR)/sxt/006
	mknod $(INSDIR)/sxt/007 c 12 7
	chmod 622 $(INSDIR)/sxt/007
	chown root $(INSDIR)/sxt/007
	chgrp sys $(INSDIR)/sxt/007

udio:
	-rm $(INSDIR)/udio
	mknod $(INSDIR)/udio c 15 208
	chmod 666 $(INSDIR)/udio
	chown root $(INSDIR)/udio
	chgrp sys $(INSDIR)/udio

vdi:
	-rm $(INSDIR)/vdi
	mknod $(INSDIR)/vdi c 14 0
	chmod 666 $(INSDIR)/vdi
	chown root $(INSDIR)/vdi
	chgrp sys $(INSDIR)/vdi

graph:
	-rm $(INSDIR)/graph
	mknod $(INSDIR)/graph c 15 224
	chmod 666 $(INSDIR)/graph
	chown root $(INSDIR)/graph
	chgrp sys $(INSDIR)/graph

ses:
	-rm $(INSDIR)/con?
	mknod $(INSDIR)/con1 c 0 1
	mknod $(INSDIR)/con2 c 0 2
	mknod $(INSDIR)/con3 c 0 3
	chmod 666 $(INSDIR)/con?
	chown root $(INSDIR)/con?
	chgrp sys $(INSDIR)/con?

clobber:
clean:

