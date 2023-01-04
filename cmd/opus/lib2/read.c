/*	@(#)read.c	1.2	*/

# include "stand.h"
#ifdef opus
char _dosdesc[NFILES];
#define min(a,b) ((a)<(b)?(a):(b))
#endif

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

#ifdef opus
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
#endif

read (fdesc, buf, count)
int fdesc;
char *buf;
int count;
{
	register i, j;
	register struct iob *io;

	if (fdesc >= 0 && fdesc <= 2)
		return (_ttread (buf, count));

	fdesc -= 3;

#ifdef DEBUG
	dumpiob(fdesc);
#endif
	if (fdesc < 0
	    || fdesc >= NFILES
	    || ((io = &_iobuf[fdesc])->i_flgs&F_ALLOC) == 0
	    || (io->i_flgs&F_READ) == 0) {
		errno = EBADF;
		return (-1);
	}
#ifdef opus
	if (_dosdesc[fdesc] > 0) {
		int addr;
		if ((addr = _mapin(buf,count)) < 0) {
			errno = ENXIO;
			return (-1);
		}
		return(_dread(_dosdesc[fdesc],addr,count));
	}
#endif
	if ((io->i_flgs&F_FILE) == 0) {
		io->i_cc = count;
		io->i_ma = buf;
		io->i_bn = (io->i_offset >> 9) + io->i_dp->dt_boff;
		if ((i = _devread (io)) > 0)
			io->i_offset += i;
		return (i);
	}
	if (io->i_offset+count > io->i_ino.i_size){
		count = io->i_ino.i_size - io->i_offset;
		}
	if ((i = count) <= 0)
		return (0);
#ifdef opus
	if ((io->i_offset & 511) == 0)
		if(_readb (io,count,buf) == count)
			return(count);
#endif
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
dumpiob(fdes)
int fdes;
{
#define IOBF _iobuf[fdes]
	_PRINTF("\niobuf:\n");
	_PRINTF("\ni_bn : %ld (%lo)",IOBF.i_bn,IOBF.i_bn);
	_PRINTF("\ni_offset : %ld (%lo)",IOBF.i_offset,IOBF.i_offset);
	_PRINTF("\ni_ma : %d (%o)",IOBF.i_ma,IOBF.i_ma);
	_PRINTF("\ni_cc : %d (%o)",IOBF.i_cc,IOBF.i_cc);
	_PRINTF("\ni_flgs : %d (%o)",IOBF.i_flgs,IOBF.i_flgs);
}
