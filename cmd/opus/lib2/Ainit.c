static char	sccsid[] = "@(#)Ainit.c	1.6";

#if pm100 || pm200 || pm300 || pm400
#ifndef NUMLOGVOL
#define NUMLOGVOL 8
#endif
# include "stand.h"
#include "compat.h"
#include "sys/iorb.h"
#include "sys/commpage.h"
#include "sys/cp.h"
#include "sys/dkcomm.h"
#include "sys/cb.h"
#include "sys/sysconf.h"

static char	dsk[] = "/dev/dsk/0s0\000";
static char	ndsk[] = "/dev/dsk/c1d0s0\000";
static char	rdsk[] = "/dev/rdsk/0s0\000";
static char	nrdsk[] = "/dev/rdsk/c1d0s0\000";
static char	flp[] = "/dev/flpa";
static char	rflp[] = "/dev/rflpa";
static char	swap[] = "/dev/swap";
struct dkinfo *_block0[DKMAX];
struct spare *_spareblks[DKMAX];
long _numspares[DKMAX];
struct io_hdisk *_hdisk_cb[DKMAX];
#else
extern gd_boot;

static char	rp[] = "/dev/rp00";
static char	dsk[] = "/dev/dsk/0s0";
static char	mt[] = "/dev/mt/0ln";
static char	omt[] = "/dev/mt0";
#ifdef vax
static char	ts[] = "/dev/ts0";
#endif
#endif

_init ()
{
#if pm100 || pm200 || pm300 || pm400
	register i,j;
	register unsigned start;
	register minor;

	/* New floppy drive names */
	if (MKNOD (flp, 0, (DKMAX-2)*NUMLOGVOL, (daddr_t) 0) < 0)
		perror (flp);
	if (mount (flp, "/a") < 0)
		perror(flp);

	if (MKNOD (rflp, 0, (DKMAX-2)*NUMLOGVOL, (daddr_t) 0) < 0)
		perror (rflp);

	flp[8] = 'b';
	if (MKNOD (flp, 0, (DKMAX-3)*NUMLOGVOL, (daddr_t) 0) < 0)
		perror (rflp);
	if (mount (flp, "/b") < 0)
		perror(flp);

	rflp[9] = 'b';
	if (MKNOD (rflp, 0, (DKMAX-3)*NUMLOGVOL, (daddr_t) 0) < 0)
		perror (rflp);
#ifndef SASH

	flp[8] = 'A';
	if (MKNOD (flp, 0, (DKMAX-4)*NUMLOGVOL, (daddr_t) 0) < 0)
		perror (rflp);
	if (mount (flp, "/A") < 0)
		perror(flp);

	rflp[9] = 'A';
	if (MKNOD (rflp, 0, (DKMAX-4)*NUMLOGVOL, (daddr_t) 0) < 0)
		perror (rflp);

	flp[8] = 'B';
	if (MKNOD (flp, 0, (DKMAX-5)*NUMLOGVOL, (daddr_t) 0) < 0)
		perror (rflp);
	if (mount ( flp, "/B") < 0)
		perror(flp);

	rflp[9] = 'B';
	if (MKNOD (rflp, 0, (DKMAX-5)*NUMLOGVOL, (daddr_t) 0) < 0)
		perror (rflp);
#endif
	if (MKNOD (swap, 0, 0, (daddr_t) 0) < 0)
		perror (swap);

    	for (i=0; i<DKMAX; i++) {
		if (_hdisk_cb[i] == 0 ||_hdisk_cb[i]->io_index == 0)
			continue;
#ifdef SASH
		if (i > 0)
			break;
#endif

		for (j = 0; j < NUMLOGVOL; j += 1) {

			if (i>10 && i <15 && j!=0) /* for flpB, flpA, flpb, */
				break; 		/* and flpa only do c1dns0 */

			minor = i * NUMLOGVOL + j;

			if (i < 10) {
				nrdsk[13]=ndsk[12]=rdsk[10]=dsk[9]= i + '0';
				nrdsk[14]=ndsk[13]=rdsk[11]=dsk[10]= 's';
				nrdsk[15]=ndsk[14]=rdsk[12]=dsk[11]= j + '0';
				nrdsk[16]=ndsk[15]=rdsk[13]=dsk[12]= 0;
			} else {
				nrdsk[13]=ndsk[12]=rdsk[10]=dsk[9]= '1';
				nrdsk[14]=ndsk[13]=rdsk[11]=dsk[10]= i-10 + '0';
				nrdsk[15]=ndsk[14]=rdsk[12]=dsk[11]= 's';
				nrdsk[16]=ndsk[15]=rdsk[13]=dsk[12]= j + '0';
			}
					
			if ((MKNOD (dsk, 0, minor, (daddr_t) 0) < 0)
				|| (MKNOD (ndsk, 0, minor, (daddr_t) 0) < 0))
				perror(dsk);

			if (i == 0 && j == 0)
				if (mount (dsk, "") < 0)
		   			perror (dsk);

			if ((MKNOD (rdsk, 0, minor, (daddr_t) 0) < 0)
				|| (MKNOD (nrdsk, 0, minor, (daddr_t) 0) < 0))
				perror(rdsk);
		}
	}
#endif
#ifdef vax
	if ( MKNOD( ts, 2, 0, (daddr_t) 0) < 0)
		perror( ts);

	ts[7] = '4';
	if ( MKNOD( ts, 2, 4, (daddr_t) 0) < 0)
		perror( ts);
#endif
}
