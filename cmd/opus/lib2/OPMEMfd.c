

/*
 * Common driver for PC floppy disk
 */

#include "stand.h"
#include "sys/cb.h"
#include "sys/iorb.h"
#include "sys/commpage.h"
#include "sys/cp.h"

#define COUNTMAX 100000

struct io_fdisk *_fdisk_cb[DKMAX];

_fdstrategy (io, func)
register struct iob *io;
{
	register i, unit;
	int errcnt = 0;
	daddr_t bn;
	long addr;

	unit = io->i_dp->dt_unit;
	bn = io->i_bn;

#ifdef DEBUG
	_PRINTF("\n\nDriver-unit %d iobuf:", unit);
	_PRINTF("\ni_bn : %ld (%lo)",io->i_bn,io->i_bn);
	_PRINTF("\ni_offset : %ld (%lo)",io->i_offset,io->i_offset);
	_PRINTF("\ni_ma : %d (%o)",io->i_ma,io->i_ma);
	_PRINTF("\ni_cc : %d (%o)",io->i_cc,io->i_cc);
	_PRINTF("\ni_flgs : %d (%o)",io->i_flgs,io->i_flgs);
#endif

	if (_fdisk_cb[unit]->io_index == 0) {
		_prs("NO DEVICE--UNIT NOT INITIALIZED\n");
		errno = ENXIO;
		return(-1);
	}

	_fdisk_cb[unit]->io_cmd = (func == READ) ? PC_READ : PC_WRITE;
	_fdisk_cb[unit]->io_error = 0;
	addr = _mapin(io->i_ma,io->i_cc);
	if (addr == -1) {
		errno = ENXIO;
		return(-1);
	}
	_fdisk_cb[unit]->io_memaddr = addr;
	_fdisk_cb[unit]->io_blknum = bn;
	_fdisk_cb[unit]->io_hblknum = bn>>16;
	_fdisk_cb[unit]->io_bcount = io->i_cc;

	do {
		if (!hio_request(_fdisk_cb[unit]))
			return(io->i_cc);

	} while (++errcnt < 10);

	errno = EIO;
	return (-1);
}

_fdclose(io)
register struct iob *io;
{
	return;
}
