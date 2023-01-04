INS = :
CFLAGS = -O -s
EXECS = ls du cat dd fsdb mkfs
CC = scc

all:	$(EXECS) od fsck fsorg

$(EXECS)::
	$(CC) $(CFLAGS) -o $@ ../../$@/$@.c
$(EXECS)::
	$(INS) -f $(ROOT)/stand $@

od::
	$(CC) $(CFLAGS) -o $@ ../../$@.c
od::
	$(INS) -f $(ROOT)/stand $@

fsck::
	if m88k; then \
		$(CC) $(CFLAGS) -Dpm400 -DFsTYPE=2 -o $@ ../../$@/$@.c; \
	elif ns32332; then \
		$(CC) $(CFLAGS) -Dpm200 -DFsTYPE=2 -o $@ ../../$@/$@.c; \
	elif ns32000; then \
		$(CC) $(CFLAGS) -Dpm100 -DFsTYPE=2 -o $@ ../../$@/$@.c; \
	elif clipper; then \
		$(CC) $(CFLAGS) -Dpm300 -DFsTYPE=2 -o $@ ../../$@/$@.c; \
	fi
fsck::
	$(INS) -f $(ROOT)/stand $@

fsorg::
	$(CC) $(CFLAGS) -o $@ ../$@.c
fsorg::
	$(INS) -f $(ROOT)/stand $@


install:
	make -f stand.mk INS=/etc/install

clean:
	rm -f *.o

clobber: clean
	rm -f $(EXECS) od fsorg fsck
