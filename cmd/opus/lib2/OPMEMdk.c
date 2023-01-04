/*
 * Common driver for PC hard disk
 */

#include "stand.h"
#include "sys/cb.h"
#include "sys/iorb.h"
#include "sys/commpage.h"
#include "sys/cp.h"
#include "sys/dkcomm.h"

#define dev_t	short
#define daddr_t	long

#define DKGETNAME	3
#define DKSETNAME	4
#define DKPARTON	5
#define DKPARTOFF	6
#define DKSIZEINFO	7
#define DKTRUNCATE	8
#define DKTOTSIZE	9

#define D_DSKBLK	1
#define D_PARTYPE	2
#define D_CYLCNT	3
#define D_HEADCNT	4
#define D_SECTTRK	5
#define D_DRNAME	6
#define D_RDPART	7
#define D_WRTPART	8
#define D_DSKSIZE	9
#define D_TRUNCATE	10
#define D_TOTSIZE	11


#include "sys/sysconf.h"

int _diskerrno;

/*
 * System configuration information for standalone programs
 */

struct dkinfo {
	long spares;
	daddr_t sparetbl;
	long used;
	long entries;
	struct logvol logvol[NUMLOGVOL];
};

#define min(a,b) (a > b ? b : a)

int gd_boot = 0;

struct io_hdisk *_hdisk_cb[DKMAX];
struct dkinfo *_block0[DKMAX];
struct spare *_spareblks[DKMAX];
struct dkinfo dblk0 = {
	0,0,0,0,
	{ {0,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {0,-1} }
};

_dkinit(io)
register struct iob *io;
{
	register long addr;
	register long blkno;
	register long sparecnt;
	register long i;
	register tblent;
	char *memaddr;
	int unit;
	int volume;
	int entries;
	char diskbuf[512];
	struct dkinfo *blk0;

	unit = io->i_dp->dt_unit / NUMLOGVOL;
	volume = io->i_dp->dt_unit % NUMLOGVOL;
#ifdef DEBUG
	_PRINTF("dkinit(%d)\n",unit);
#endif

	if (_hdisk_cb[unit] == 0 ||_hdisk_cb[unit]->io_index == 0)
		return(-1);
	if (_block0[unit] == 0)
		_block0[unit] = (struct dkinfo *)sbrk(sizeof(struct dkinfo));
	blk0 = _block0[unit];
	if (_block0[unit] == (struct dkinfo *)-1) {
		_hdisk_cb[unit]->io_index == 0;
		_PRINTF("UNABLE TO INITIALIZE DISK DRIVE %d (OUT OF MEM)\n",
			unit);
		return(-1);
	}
	_hdisk_cb[unit]->io_cmd = PC_READ;
	_hdisk_cb[unit]->io_error = 0;
	addr = _mapin(diskbuf,sizeof(struct confinfo));
	if (addr == -1) {
		errno = ENXIO;
		return(-1);
	}
	_hdisk_cb[unit]->io_memaddr = addr;
	_hdisk_cb[unit]->io_blknum = 0;
	_hdisk_cb[unit]->io_hblknum = 0;
	_hdisk_cb[unit]->io_bcount = sizeof(struct confinfo);

	if (hio_request(_hdisk_cb[unit]) > 2) {
		/*_PRINTF("UNABLE TO READ BLOCK 0 of DISK DRIVE %d\n",unit);*/
		errno = ENXIO;
		return(-1);
	}
	if (((struct confinfo *)diskbuf)->magic != 'opus' ||
			checksum(diskbuf) != BLKCHKSUM) {
		_block0[unit] = &dblk0;
		return(-1);
	}

	entries = blk0->entries;
	blk0->spares = ((struct confinfo *)diskbuf)->spares;
	blk0->sparetbl = ((struct confinfo *)diskbuf)->sparetbl;
	blk0->used = ((struct confinfo *)diskbuf)->used;
	blk0->entries = ((struct confinfo *)diskbuf)->entries;
	for (i=0; i<NUMLOGVOL; i++) {
		blk0->logvol[i] = ((struct confinfo *)diskbuf)->logvol[i];
	}

				/* we've been here before, so don't sbrk */
	if ((_spareblks[unit] == 0) || (entries != blk0->entries)) {
#if ns32000 && SASH
		memaddr = (char *)(0x100000 + KPTRTAB - blk0->entries * sizeof(struct spare));
#else
		memaddr = (char *)sbrk(blk0->entries * sizeof(struct spare));
#endif
		if (memaddr < 0) {
	    		_PRINTF("UNABLE TO INITIALIZE DISK DRIVE %d (TOO MANY SPARES -- %d)\n", unit,blk0->entries);
	    		return(-2);
		}
		_spareblks[unit] = (struct spare *)memaddr;
	}
	blkno = blk0->sparetbl;
	sparecnt = blk0->entries;

	for (i=0; i<sparecnt; i++) {
		tblent = i % NUMSPARE;
		if (tblent == 0) {
			if (blkno == 0)
				break;
			_hdisk_cb[unit]->io_memaddr =
				_mapin(diskbuf,sizeof(struct spareinfo));
			_hdisk_cb[unit]->io_blknum = blkno;
			_hdisk_cb[unit]->io_hblknum = blkno >> 16;
			_hdisk_cb[unit]->io_bcount = sizeof(struct spareinfo);
			if (hio_request(_hdisk_cb[unit]) > 2) {
				errno = EIO;
				break;
			}
			if (checksum(diskbuf) != BLKCHKSUM)
				break;
			blkno = ((struct spareinfo *)diskbuf)->link;
		}
		_spareblks[unit][i] =
			((struct spareinfo *)diskbuf)->spare[tblent%NUMSPARE];
	}
	if (i != sparecnt) {
		_PRINTF("BAD SPARE TABLE ON DRIVE %d\n",unit);
		_hdisk_cb[unit]->io_index == 0;
		return(-1);
	}
	return(0);
}
	
_dkopen (io)
register struct iob *io;
{
	_dkinit(io);
}

_dkstrategy (io, func)
register struct iob *io;
{
	register i, unit;
	int errcnt = 0;
	daddr_t bn;
	daddr_t firstbn;
	daddr_t lastbn;
	long addr;
	int blkatatime = 0;
	int bytesread;
	int badsect;
	int volume;
	int newbn;
	int bcount;
	int retval;

	unit = io->i_dp->dt_unit / NUMLOGVOL;
	volume = io->i_dp->dt_unit % NUMLOGVOL;
	firstbn = io->i_bn;
	lastbn = io->i_bn + (io->i_cc + 511) / 512;

	if (_block0[unit] != (struct dkinfo *)-1) {
		firstbn += _block0[unit]->logvol[volume].start - io->i_dp->dt_boff;
		lastbn += _block0[unit]->logvol[volume].start -
			io->i_dp->dt_boff;
		if (_block0[unit]->logvol[volume].len != -1) {
			if (lastbn > _block0[unit]->logvol[volume].start +
					_block0[unit]->logvol[volume].len) {
				/*_PRINTF("DISK BLOCK OUT OF REACH\n");*/
				errno = ENXIO;
				return(-1);
			}
		}
	}

	if (volume != (NUMLOGVOL - 1)) {
		for (i=0; i<_block0[unit]->entries; i++) {
			badsect = _spareblks[unit][i].badblk;
			if (lastbn < badsect)
				break;
			if (firstbn > badsect)
				continue;
			blkatatime = 1;
		}
	}

#ifdef DEBUG
	_PRINTF("\n\nDriver-unit %d iobuf:", unit);
	_PRINTF("\ni_bn : %ld (%lo)",io->i_bn,io->i_bn);
	_PRINTF("\ni_offset : %ld (%lo)",io->i_offset,io->i_offset);
	_PRINTF("\ni_ma : %d (%o)",io->i_ma,io->i_ma);
	_PRINTF("\ni_cc : %d (%o)",io->i_cc,io->i_cc);
	_PRINTF("\ni_flgs : %d (%o)",io->i_flgs,io->i_flgs);
#endif

	if (_hdisk_cb[unit] == 0 ||_hdisk_cb[unit]->io_index == 0) {
		_PRINTF("NO DEVICE--UNIT NOT INITIALIZED\n");
		errno = ENXIO;
		return(-1);
	}

	_hdisk_cb[unit]->io_cmd = (func == READ) ? PC_READ : PC_WRITE;
	_hdisk_cb[unit]->io_error = 0;
	addr = _mapin(io->i_ma,io->i_cc);
	if (addr == -1) {
		errno = ENXIO;
		return(-1);
	}
	bytesread = 0;
	bn = firstbn;
nextblk:
	errcnt = 0;
	newbn = blktoread(bn,unit,volume);
	do {
		_hdisk_cb[unit]->io_memaddr = addr;
		_hdisk_cb[unit]->io_blknum = newbn;
		_hdisk_cb[unit]->io_hblknum = newbn >> 16;
		_hdisk_cb[unit]->io_bcount = blkatatime ? 
			min(512,io->i_cc-bytesread):io->i_cc-bytesread;
		if ((retval = hio_request(_hdisk_cb[unit])) <= E_CE) {
			errno = 0;
			break;
		}
		if (retval != E_DE) {
			errcnt = 2;
			errno = ENXIO;
		} else
			errno = EIO;
	} while (++errcnt < 2);
	
	if (errcnt < 2) {
		bcount = _hdisk_cb[unit]->io_bcount;
		if (bcount > 0) {
			bytesread += bcount;
			if (bytesread < io->i_cc) {
				addr += bcount;
				bn = firstbn + (bytesread + 511)/512;
				goto nextblk;
			}
		}
		return(bytesread);
	}

	return (-1);
}

blktoread(blkno,unit,volume) {
	register i;
	register badsect;

	if (volume < NUMLOGVOL-1) {
		for (i=0; i<_block0[unit]->entries; i++) {
			badsect = _spareblks[unit][i].badblk;
			if (badsect > blkno)
				return(blkno);
			if (badsect == blkno) {
#ifdef DEBUG
				_PRINTF("{%d->%d}\n",badsect,
					_spareblks[unit][i].spareblk);
#endif
				return(_spareblks[unit][i].spareblk);
			}
		}
	}
	return(blkno);
}

_dkclose(io)
register struct iob *io;
{
	return;
}

#ifndef SASH
/* special ioctl for Opus opconfig program */
_dkioctl(drive,cmd,buf)
long *buf;
{
	errno = 0;
	if (drive > DKMAX || _hdisk_cb[drive] == 0) {
		errno = ENXIO;
		return(-1);
	}
	_hdisk_cb[drive]->io_cmd = PC_IOCTL;
	_hdisk_cb[drive]->io_error = 0;
	switch(cmd) {
	case D_TRUNCATE:
		_hdisk_cb[drive]->io_memaddr = DKTRUNCATE;
		hio_request(_hdisk_cb[drive]);
		break;
	case D_TOTSIZE:
		_hdisk_cb[drive]->io_memaddr = DKTOTSIZE;
		hio_request(_hdisk_cb[drive]);
		*buf = _hdisk_cb[drive]->io_blknum |
			 _hdisk_cb[drive]->io_hblknum<<16;
		break;
	case D_DSKBLK:
		_hdisk_cb[drive]->io_memaddr = DKSIZEINFO;
		hio_request(_hdisk_cb[drive]);
		*buf = _hdisk_cb[drive]->io_blknum |
			 _hdisk_cb[drive]->io_hblknum<<16;
		break;
	case D_PARTYPE:
		_hdisk_cb[drive]->io_memaddr = DKSIZEINFO;
		hio_request(_hdisk_cb[drive]);
		*buf = _hdisk_cb[drive]->io_devstat;
		if (_hdisk_cb[drive]->io_error == E_CMD) {
			*buf = -2;
			return(0);
		}
		break;
	case D_DSKSIZE:
		_hdisk_cb[drive]->io_memaddr = DKSIZEINFO;
		hio_request(_hdisk_cb[drive]);
		*buf = _hdisk_cb[drive]->io_blknum |
			 _hdisk_cb[drive]->io_hblknum<<16;
		if (_hdisk_cb[drive]->io_error == E_CMD) {
			*buf = -2;
			return(0);
		}
		break;
	case D_CYLCNT:
		_hdisk_cb[drive]->io_memaddr = DKSIZEINFO;
		hio_request(_hdisk_cb[drive]);
		*buf = _hdisk_cb[drive]->io_bcount;
		break;
	case D_HEADCNT:
		_hdisk_cb[drive]->io_memaddr = DKSIZEINFO;
		hio_request(_hdisk_cb[drive]);
		*buf = _hdisk_cb[drive]->io_memaddr >> 8;
		break;
	case D_SECTTRK:
		_hdisk_cb[drive]->io_memaddr = DKSIZEINFO;
		hio_request(_hdisk_cb[drive]);
		*buf = _hdisk_cb[drive]->io_memaddr & 0xff;
		break;
	case D_DRNAME:
		_hdisk_cb[drive]->io_memaddr = DKGETNAME;
		_hdisk_cb[drive]->io_bcount = _mapin(buf,NAMESIZE);
		hio_request(_hdisk_cb[drive]);
		break;
	case D_RDPART:
	case D_WRTPART:
		_hdisk_cb[drive]->io_memaddr = DKPARTON;
		if (hio_request(_hdisk_cb[drive]) > 2)
			break;
		_hdisk_cb[drive]->io_cmd = (cmd==D_WRTPART)?PC_WRITE:PC_READ;
		_hdisk_cb[drive]->io_blknum = 0;
		_hdisk_cb[drive]->io_hblknum = 0;
		_hdisk_cb[drive]->io_bcount = 512;
		_hdisk_cb[drive]->io_error = 0;
		_hdisk_cb[drive]->io_memaddr = _mapin(buf,512);
		if (hio_request(_hdisk_cb[drive]) > 2)
			break;
		_hdisk_cb[drive]->io_cmd = PC_IOCTL;
		_hdisk_cb[drive]->io_memaddr = DKPARTOFF;
		_hdisk_cb[drive]->io_error = 0;
		hio_request(_hdisk_cb[drive]);
		break;
	default:
		*buf = -1;
		errno = EINVAL;
		return(-1);
	}
	if (_hdisk_cb[drive]->io_error > 2) {
		*buf = -1;
		errno = EIO;
		return(-1);
	}
	return(0);
}
#endif

hio_request(cb_addr)
struct io_gen *cb_addr;
{
	struct io_gen *io_req;

	cb_addr->io_status = S_GO;
	PCCMD->pc_rqblk = mapcomm(cb_addr);
	PCCMD->pc_rqflg = S_GO;
	IRQPC();
	while (cb_addr->io_status != S_DONE) {
		PCCMD->pc_cpflg = S_IDLE;
	}
	PCCMD->pc_cpflg = S_IDLE;
	cb_addr->io_status = S_IDLE;
	return(_diskerrno = cb_addr->io_error);
}

checksum(buf)
struct confinfo *buf;
{
	int i;
	long *lptr;
	long sum = 0;

	lptr = (long *)buf;
	for (i=0; i<512; i+=4)
		sum += *lptr++;
	return(sum);
}

