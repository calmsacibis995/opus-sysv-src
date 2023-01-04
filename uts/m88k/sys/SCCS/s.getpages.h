h62344
s 00063/00000/00000
d D 1.1 90/03/06 12:29:44 root 1 0
c date and time created 90/03/06 12:29:44 by root
e
u
U
t
T
I 1
#ifndef _SYS_GETPAGES_H_
#define _SYS_GETPAGES_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*	The following list is used to keep track of regions locked
**	by getpages.
*/

typedef struct {
	reg_t	*gpr_rgptr;	/* Pointer to the region.	*/
	int	gpr_count;	/* Number of pages from this	*/
				/* region in spglist and 	*/
				/* fpglist.			*/
} gprgl_t;

extern gprgl_t	gprglst[];
extern int	gprgndx;


/*	The following structure is used to maintain a list of
 *	pages to be stolen by getpages.
 */

typedef struct {
	pde_t	*gp_ptptr;	/* Ptr to page table entry.	*/
	gprgl_t	*gp_rlptr;	/* Ptr to region list entry.	*/
} pglst_t;

/*	The following list is used to accumulate pages which we
**	wish to steal but which must be written to swap first.
*/

extern pglst_t	spglst[];
extern int	spglndx;

/*	The following flags are used to lock the getpages tables
**	described above so that vhand and sched can't both
**	use them at the same time.
*/

#define	PL_LOCK		0x1	/* Lock for pglstlock			*/
#define PL_WANTED	0x2	/* Wanted flag for pglstlock		*/

extern int	pglstlk();
extern int	pglstunlk();
extern int	tstpglstlk();

#endif	/* ! _SYS_GETPAGES_H_ */
E 1
