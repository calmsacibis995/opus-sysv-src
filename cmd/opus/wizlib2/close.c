/*	@(#)close.c	1.1	*/

# include <stand.h>

close(fdesc)
register fdesc; {
	register struct iob *io;

	if (fdesc < 0
	    || fdesc >= NFILES
	    || ((io = &_iobuf[fdesc])->i_flgs&F_ALLOC) == 0) {
		errno = EBADF;
		return (-1);
	}
	io->i_flgs = 0;
	return (0);
}
