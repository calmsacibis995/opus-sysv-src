/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/codeout.c	1.9" */

#include <stdio.h>
#include "systems.h"
#include "symbols.h"
#include "codeout.h"
#include "gendefs.h"
#include "temppack.h"
#include "section.h"
#include "errors.h"
#include "dbug.h"		/* Special macro based C debugging package */

extern void as_error ();

extern FILE *fdsect;		/* file written to by codgen */

extern symbol *dot;
extern symbol symtab[];		/* internal symbol table */

extern struct scninfo secdat[];	/* section info table */

long newdot;			/* up-to-date value of dot */

FILE *fdcode;		/* temp file containing intermediate language */

static short poscnt = 0;	/* current bit position in outword buffer */
				/* 0 <= poscnt < BITSPOW	 */
static short bitpos = 0;	/* bit position within a byte in outword */
				/* buffer; spans from [0 - BITSPBY]	 */

static OUTWTYPE outword;	/* output buffer - see gendefs.h */

static int mask[] = {
    0, 1, 3, 7,
    017, 037, 077,
    0177, 0377, 0777,
    01777, 03777, 07777,
    017777, 037777, 077777,
    0177777
};

extern int (*(modes[])) ();		/* array of action routine functions */
extern upsymins *lookup ();
extern short sttop;

void codgen (nbits, val)
register short nbits;
long val;
{
    register short size;	/* space remaining in outword buffer */
    register long value;	/* working buffer for val */

    DBUG_ENTER ("codgen");
    DBUG_PRINT ("codgen1", ("output %d bits from %#x", nbits, val));
    if (nbits) {
start:
 	value = val;
	/* generate as much of nbits as possible */
	if ((size = BITSPOW - poscnt) > nbits) {
	    size = nbits;
	}
	value >>= (nbits - size);	/* align value to get correct bits */
	value &= mask[size];
	value <<= BITSPOW - poscnt - size;
	outword |= (OUTWTYPE) value;	/* store field in the buffer */
	poscnt += size;
	bitpos += size;
	newdot += bitpos / BITSPBY;
	DBUG_PRINT ("newdot", ("newdot = %#lx", newdot));
	bitpos %= BITSPBY;
	if (poscnt == BITSPOW) {		/* is buffer full? */
	    DBUG_PRINT ("out", ("output %#x", outword));
	    (void) OUT (outword, fdsect);		/* see gendefs.h */
	    poscnt = 0;
	    outword = 0;
	    if (nbits > size) {			/* more bits? */
		nbits -= size;
		goto start;
	    }
	}
    }
    DBUG_VOID_RETURN;
}

void codout (file, start, sect)
char *file;
long start;
int sect;
{
    codebuf code;
    register symbol *sym;

    DBUG_ENTER ("codout");
    DBUG_PRINT ("cdout1",
	    ("generate output to section %d from '%s', starting at %#lx",
	    sect, file, start));
    dot = (*lookup (".", INSTALL, USRNAME)).stp;
    dot -> value = newdot = start;
    DBUG_PRINT ("newdot", ("newdot = %#lx", newdot));
    dot -> styp = secdat[sect].s_typ;
    if ((fdcode = fopen (file, "r")) == NULL) {
	as_error (ERR_COTR, file);
    }
    while (1) {
	if (!getcode (&code)) {
	    break;
	}
	if (code.caction != 0) {
	    if (code.caction > NACTION) {
		as_error (ERR_IAR);
	    } else {
		if (code.cindex) {
		    sym = symtab + (code.cindex - 1);
		} else {
		    sym = (symbol *) NULL;
		}
		/* call to appropriate action routine */
		(*(modes[code.caction])) (sym, &code);
	    }
	}
	/* do NOACTION case or handle any end-of-action processing */
	codgen ((short) (code.cnbits), code.cvalue);
    }
    if (sttop > 0) {
	as_error (ERR_USTE2);
    }
    (void) fclose (3, fdcode);
    DBUG_VOID_RETURN;
}


/* 
 *	get a codebuf structure from the packed temp file
 */

int getcode (codptr)
codebuf *codptr;
{
    auto long pckbuf;
    register long pckword;
    register int rtnval = 0;

    DBUG_ENTER ("getcode");
    if (fread ((char *) &pckbuf, sizeof (long), 1, fdcode) == 1) {
	pckword = pckbuf;
	codptr -> caction = ACTNUM (pckword);
	codptr -> cnbits = NUMBITS (pckword);
	DBUG_PRINT ("codebuf2", ("caction = %u, cnbits = %u", codptr -> caction,
		codptr -> cnbits));
	if (pckword & SYMINDEX) {
	    codptr -> cindex = SYMORVAL (pckword);
	} else {
	    codptr -> cindex = 0;
	}
	if (pckword & VAL0) {
	    codptr -> cvalue = 0;
	} else if (pckword & VAL16) {
	    codptr -> cvalue = SYMORVAL (pckword);
	} else if (fread ((char *) &(codptr -> cvalue), sizeof (long), 1, fdcode) != 1) {
	    as_error (ERR_BTFF);
	}
	DBUG_PRINT ("codebuf3", ("cindex = %u, cvalue = %#lx", codptr -> cindex,
		codptr -> cvalue));
	rtnval = 1;
    }
    DBUG_RETURN (rtnval);
}
