#ifndef _SYS_RAM_H_
#define _SYS_RAM_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*		Copyright (c) 1985 AT&T		*/
/*		All Rights Reserved		*/
/*		@(#)ram.h	8.2			*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/* ram configuration structure */

struct ram {
	unsigned 	ram_lo;	/* low ram range (clicks) */
	unsigned 	ram_hi;	/* high ram range ( + 1 ) (clicks) */
	unsigned 	ram_sz;	/* ram size (clicks) */
	unsigned	ram_flg;
};

/* ram_flg values */

#define	RAM_INIT	0x0001	/* ram must be written to be reset */
#define	RAM_HUNT	0x0002	/* ram boards may be located within range */
#define	RAM_PRIV	0x0004	/* ram is private (not general purpose) */
#define	RAM_CI		0x0008	/* cache inhibited in this range */
#define	RAM_FOUND	0x8000	/* ram initializer found memory in this range */

/* initial probe structure */

struct probe {
	char	*probe_addr;	/* address to be probed */
	short	probe_byte;	/* byte to write into probe_addr */
};

/* (probe_addr is only read from if read if probe_byte < 0) */

#endif	/* ! _SYS_RAM_H_ */
