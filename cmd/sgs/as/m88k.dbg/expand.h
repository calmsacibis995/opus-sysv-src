#ident "@(#) $Header: expand.h 4.1 88/04/26 16:58:41 root Exp $ RISC Source Base"
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


#define L_SDI	(0)		/* a long sdi */
#define S_SDI	(1)		/* a short sdi */
#define U_SDI	(2)		/* an sdi of unknown size */

/* structure for a sdi selection set entry */

typedef struct {
    unsigned short sdicnt;	/* this is the Nth sdi */
    short itype;		/* type of this sdi */
    long minaddr;		/* minimum address for this sdi */
    short maxaddr;		/* difference between max addr and min addr */
    symbol *labptr;		/* ptr to label appearing in the operand */
    long constant;		/* value of constant appearing in operand */
} ssentry;

/* structure that contains the span range for each sdi */

typedef struct {
    long lbound;		/* lower bound of sdi range */
    long ubound;		/* upper bound of sdi range */
} rangetag;
