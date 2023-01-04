/*	@(#)close.c	1.1	*/

# include <stand.h>
#ifdef opus
char _dosdesc[NFILES];
#endif

close(fdesc)
register fdesc; {
	register struct iob *io;

	fdesc -= 3;
	if (fdesc < 0
	    || fdesc >= NFILES
	    || ((io = &_iobuf[fdesc])->i_flgs&F_ALLOC) == 0) {
		errno = EBADF;
		return (-1);
	}
	if ((io->i_flgs&F_FILE) == 0)
		_devclose(io);
	io->i_flgs = 0;
#ifdef opus
	if (fdesc >= 0)
		if (_dosdesc[fdesc] > 0)
			_dclose(_dosdesc[fdesc]);
	_dosdesc[fdesc] = 0;
#endif
	return (0);
}
