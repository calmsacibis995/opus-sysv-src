#ifndef _FS_PRFCNTL_H_
#define _FS_PRFCNTL_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved 	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*	prfcntl.h - fcntls for /proc */

#define PFC		('p'<<8)

#define PFCGETPR	(PFC|1)	/* read struct proc */
#define PFCOPENT	(PFC|2)	/* open text file for reading */
#define PFCEXCLU	(PFC|3)	/* mark text for exclusive use */

#define PFCSTOP		(PFC|4)	/* send STOP signal and... */
#define PFCWSTOP	(PFC|5)	/* wait for process to STOP */
#define PFCRUN		(PFC|6)	/* make process runnable */

#define PFCSMASK	(PFC|7)	/* set signal trace bit mask */
#define PFCCSIG		(PFC|8)	/* clear current signal */
#define PFCKILL		(PFC|9)	/* send signal */

#define PFCSEXEC	(PFC|10)	/* stop on exec */
#define PFCREXEC	(PFC|11)	/* run on exec */

#define PFCNICE		(PFC|12)	/* set nice priority */

#define PFCGMASK	(PFC|13)	/* get signal trace bit mask */

#endif	/* ! _FS_PRFCNTL_H_ */
