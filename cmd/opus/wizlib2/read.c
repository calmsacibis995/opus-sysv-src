/*	@(#)read.c	1.2	*/

# include "stand.h"
#define min(a,b) ((a)<(b)?(a):(b))

daddr_t	_sbmap ();

static
_readc (io)
register struct iob *io; {
	register char *p;
	register c;
	int off;

	p = io->i_ma;
	if (io->i_cc <= 0) {
		io->i_bn = io->i_offset / BLKSIZ;
		if ((io->i_bn = _sbmap (io, io->i_bn)) < 0){
			return (-1);
		}
		io->i_bn = io->i_bn * (BLKSIZ/512) + io->i_dp->dt_boff;
		io->i_ma = io->i_buf;
		io->i_cc = BLKSIZ;
		_devread (io);
		off = io->i_offset & BMASK;
		if (io->i_offset + (BLKSIZ-off) >= io->i_ino.i_size)
			io->i_cc = io->i_ino.i_size - io->i_offset + off;
		io->i_cc -= off;
		if (io->i_cc <= 0){
			return (-2);
		}
		p = &io->i_buf[off];
	}
	io->i_cc--;
	io->i_offset++;
	c = (int) *p++;
	io->i_ma = p;
	return (c & 0377);
}

static
_readb (io,count,buf)
register struct iob *io;
char *buf; {
	register char *p;
	register c;
	int off;

	if ((io->i_offset & 511) != 0)
		return(0);
	p = io->i_ma;
	io->i_bn = io->i_offset / BLKSIZ;
	if ((io->i_bn = _sbmap (io, io->i_bn)) < 0){
		return (-1);
	}
	io->i_bn = io->i_bn * (BLKSIZ/512) + io->i_dp->dt_boff +
		((io->i_offset & 512) / 512);
	io->i_ma = buf;
	io->i_cc = min(BLKSIZ,count);
	_devread (io);
	io->i_offset += io->i_cc;
	io->i_cc = 0;
	io->i_ma = p;
	return (min(BLKSIZ,count));
}

read (fdesc, buf, count)
int fdesc;
char *buf;
int count;
{
	register i, j;
	register struct iob *io;

	io = &_iobuf[fdesc];
	if (io->i_offset+count > io->i_ino.i_size){
		count = io->i_ino.i_size - io->i_offset;
		}
	if ((i = count) <= 0)
		return (0);
	if ((io->i_offset & 511) == 0)
		if(_readb (io,count,buf) == count)
			return(count);
	do {
		switch (j = _readc (io)) {
		case -1:
			return (-1);
		case -2:
			count -= i;
			i = 1;
			break;
		}
		*buf++ = (char)j;
	} while (--i);
	return (count);
}

dumpiob(io)
struct iob *io;
{
	register fdes;


	fdes = io - _iobuf;

#define IOBF _iobuf[fdes]
	printf("\niobuf:\n");
	printf("\ni_bn : %ld (%lx)",IOBF.i_bn,IOBF.i_bn);
	printf("\ni_offset : %ld (%lx)",IOBF.i_offset,IOBF.i_offset);
	printf("\ni_ma : %d (%x)",IOBF.i_ma,IOBF.i_ma);
	printf("\ni_cc : %d (%x)",IOBF.i_cc,IOBF.i_cc);
	printf("\ni_flgs : %d (%x)",IOBF.i_flgs,IOBF.i_flgs);
}
