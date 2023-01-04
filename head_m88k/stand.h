#ifndef _STAND_H_
#define _STAND_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

/*
 * Header file for standalone package
 */

#define	FsTYPE	2

#ifndef ASSEM
#include "errno.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/inode.h"

/*
 * I/O block flags
 */

#define F_READ	01
#define F_WRITE	02
#define F_ALLOC	04
#define F_FILE	010

/*
 * Request codes -- must be
 * the same as an F_XXX above
 */

#define	READ	1
#define	WRITE	2

/*
 * Buffer sizes
 */

#if FsTYPE == 2
#define BLKSIZ	1024
#else
#define BLKSIZ	512
#endif
#define NAMSIZ	60

/*
 * devsw table --
 * initialized in conf.c
 */

struct devsw {
	int	(*dv_strategy)();
	int	(*dv_open)();
	int	(*dv_close)();
};
extern struct devsw _devsw[];

/*
 * dtab table -- entries
 * are created by MKNOD
 */

#if SASH
#define NDEV 128
#else
#define NDEV 1024
#endif

struct dtab {
	char		*dt_name;
	struct devsw	*dt_devp;
	int		dt_unit;
	daddr_t		dt_boff;
};
extern struct dtab _dtab[];

/*
 * mtab table -- entries
 * are created by mount
 */

#define NMOUNT	8

struct mtab {
	char		*mt_name;
	struct dtab	*mt_dp;
};
extern struct mtab _mtab[];

/*
 * I/O block: includes an inode,
 * cells for the use of seek, etc,
 * and a buffer.
 */

#define NFILES	6

#ifndef NADDR
#define	NADDR 13
#endif

struct iob {
	char		i_flgs;
	struct inode	i_ino;
	time_t		i_atime;
	time_t		i_mtime;
	time_t		i_ctime;
	struct dtab	*i_dp;
	off_t		i_offset;
	daddr_t		i_bn;		/* disk block number (physical) */
	char		*i_ma;
	int		i_cc;		/* character count */
	char		i_buf[BLKSIZ];
	daddr_t		i_addr[NADDR];	/* for converted disk block address */
};
extern struct iob _iobuf[];

/*
 * Set to the error type of the routine that
 * last returned an error -- may be read by perror.
 */

extern int errno;
#define	RAW	040
#define	LCASE	04
#define	XTABS	02
#define	ECHO	010
#define	CRMOD	020
#define B300	7
#define B9600	015
struct sgttyb {
	char	sg_ispeed;		/* input speed */
	char	sg_ospeed;		/* output speed */
	char	sg_erase;		/* erase character */
	char	sg_kill;		/* kill character */
	int	sg_flags;		/* mode flags */
};
#endif /* !ASSEM */

#ifndef ASSEM
#define BPTOP		0xf2
#define NUMLOGVOL	8
#define LOGVOLSIZE	4096

#define MAXERRCNT	10
struct er_block {
	unsigned short	er_stat;	/* last condition tested */
	unsigned short	er_unused;
	long		er_dma;		/* number of dma errors */
	long		er_par;		/* number of parity errors */
	long		er_parbitmap;	/* bit map of banks with bad parity */
	long		er_errcnt;	/* number of memory test errors */
	long		er_bitmap[32];	/* bit error map for each bank */
	struct {
		long	er_addr;
		long	er_val;
		long	er_val1;
		long	er_val2;
	} er_memerrors[MAXERRCNT];
};
#endif /* !ASSEM */

#endif	/* ! _STAND_H_ */
