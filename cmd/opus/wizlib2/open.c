/*	@(#)open.c	1.2	*/

# include "stand.h"
# include <fcntl.h>

ino_t	_find();
char	*strchr(), *strrchr();
char nbuf[1024];

open(strx, how)
char *strx; {
	register struct iob *io;
	int fdesc;
	ino_t n;

	if (strx == NULL || how < 0) {
		errno = EINVAL;
		return (-1);
	}

	if (how != 0) {
		io->i_flgs = 0;
		errno = EACCES;
		return (-1);
	}

	_cond(strx, nbuf);


	for (fdesc = 0; fdesc < NFILES; fdesc++)
		if (_iobuf[fdesc].i_flgs == 0)
			goto gotfile;
	errno = EMFILE;
	return (-1);
gotfile:
	(io = &_iobuf[fdesc])->i_flgs |= F_ALLOC;

	_devopen(io);

	if ((n = _find(&nbuf[1], io)) == 0) {
		io->i_flgs = 0;
		return (-1);
	}

	_openi(n, io);
	io->i_offset = 0;
	io->i_cc = 0;
	io->i_flgs |= F_FILE | (how+1);
	return (fdesc);
}
