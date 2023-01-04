/*	@(#)write.c	1.1	*/

# include <stand.h>
#ifdef opus
char _dosdesc[NFILES];
#endif

write(fdesc, buf, count)
int fdesc;
char *buf;
int count; {
	register i;
	register struct iob *io;

	if (fdesc >= 0 && fdesc <= 2)
		return (_ttwrite(buf, count));

	fdesc -= 3;
	if (fdesc < 0
	    || fdesc >= NFILES
	    || ((io = &_iobuf[fdesc])->i_flgs&F_ALLOC) == 0
	    || (io->i_flgs&F_WRITE) == 0) {
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
		return(_dwrite(_dosdesc[fdesc],addr,count));
	}
#endif
	io->i_cc = count;
	io->i_ma = buf;
	io->i_bn = (io->i_offset >> 9) + io->i_dp->dt_boff;
	if ((i = _devwrite(io)) > 0)
		io->i_offset += i;
	return (i);
}
