/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident "$Header: expand1.c 2.2 88/04/08 14:01:28 root Exp $ RISC Source Base"

#include <stdio.h>
#include "systems.h"
#include "symbols.h"
#include "expand.h"
#include "gendefs.h"
#include "errors.h"
#include "dbug.h"

extern rangetag range[];
extern char idelta[];
extern char pcincr[];


#ifndef	MAXSS
#define MAXSS	(200)		/* maximum number of Selection set entries */
#endif

#ifndef	MAXSDI
#define MAXSDI	(4000)		/* maximum number of SDI's we can handle */
#endif

#ifndef	MAXLAB
#define MAXLAB	(2000)		/* max. number of labels whose address depends on SDI's */
#endif

char islongsdi[MAXSDI];

ssentry selset[MAXSS];
symbol *labset[MAXLAB];

unsigned short sdicnt = 0;
static short ssentctr = -1;
static short labctr = -1;

static unsigned short PCmax;

extern symbol *dot;
extern long newdot;

extern void as_error ();

void update (ssptr, sditype)
ssentry *ssptr;
short sditype;
{
    register ssentry *ptr;
    register short cntr;
    register symbol *lptr;
    register symbol **ptr2;
    register unsigned short delta;
    register unsigned short sdipos;
    long instaddr;

    DBUG_ENTER ("update");
    delta = idelta[ssptr -> itype];
    sdipos = ssptr -> sdicnt;
    instaddr = ssptr -> minaddr;
    PCmax -= delta;
    dot -> maxval -= delta;
    if (sditype) {		/* nonzero if short */
	instaddr += ssptr -> maxaddr;
	for (cntr = ssentctr, ptr = &selset[0]; cntr-- >= 0; ++ptr) {
	    if (ptr -> sdicnt > sdipos) {
		ptr -> maxaddr -= delta;
	    }
	}
	for (cntr = labctr, ptr2 = &labset[0]; cntr-- >= 0;) {
	    lptr = *ptr2;
	    if (lptr -> value + lptr -> maxval > instaddr) {
		lptr -> maxval -= delta;
		if (lptr -> maxval == 0) {
		    *ptr2 = labset[labctr--];
		    continue;
		}
	    }
	    ptr2++;
	}
    } else {			/* long */
	dot -> value += delta;
	newdot += delta;
	DBUG_PRINT ("newdot", ("newdot = %#lx", newdot));
	islongsdi[sdipos] = (char) delta;
	for (cntr = ssentctr, ptr = &selset[0]; cntr-- >= 0; ++ptr) {
	    if (ptr -> sdicnt > sdipos) {
		ptr -> minaddr += delta;
		ptr -> maxaddr -= delta;
	    }
	}
	for (cntr = labctr, ptr2 = &labset[0]; cntr-- >= 0;) {
	    lptr = *ptr2;
	    if (lptr -> value > instaddr) {
		lptr -> value += delta;
		lptr -> maxval -= delta;
		if (lptr -> maxval == 0) {
		    *ptr2 = labset[labctr--];
		    continue;
		}
	    }
	    ptr2++;
	}
    }
    DBUG_VOID_RETURN;
}

/*
 *	"notdone" is used to indicate when pass 1 has been completed.
 *	This helps to detect undefined externals.
 */

static short notdone = YES;

int sdiclass (sdiptr)
register ssentry *sdiptr;
{
    register symbol *lptr;
    register short itype;
    register short ltype;
    register long span;

    DBUG_ENTER ("sdiclass");
    lptr = sdiptr -> labptr;
    itype = sdiptr -> itype;
    if ((ltype = lptr -> styp & TYPE) != UNDEF) {
	if (ltype != TXT) {
	    DBUG_RETURN (L_SDI);
	}
	span = lptr -> value;
    } else {
	if (notdone == NO) {
	    DBUG_RETURN (L_SDI);
	}
	if (dot -> value != sdiptr -> minaddr) {
	    span = dot -> value;
	} else {
	    span = sdiptr -> minaddr + pcincr[itype];
	}
    }
    span += sdiptr -> constant - (sdiptr -> minaddr + pcincr[itype]);
    if ((span < range[itype].lbound) ||
	    (span > range[itype].ubound)) {	/* definitely long */
	DBUG_RETURN (L_SDI);
    } else {
	if (ltype != UNDEF) {
	    span += (int) (lptr -> maxval) -
		((lptr -> value > sdiptr -> minaddr) ?
		    (sdiptr -> maxaddr + idelta[itype]) : sdiptr -> maxaddr);
	    if ((span >= range[itype].lbound) &&
		    (span <= range[itype].ubound)) {	/* definitely short */
		DBUG_RETURN (S_SDI);
	    }
	}
    }
    DBUG_RETURN (U_SDI);
}

void expand ()
{
    register short change = YES;
    register short cntr;
    register ssentry *current;
    register short sditype;

    DBUG_ENTER ("expand");
    while (change) {
	change = NO;
	for (cntr = ssentctr, current = &selset[0]; cntr-- >= 0;) {
	    if ((sditype = sdiclass (current)) != U_SDI) {
		change = YES;
		update (current, sditype);
		*current = selset[ssentctr--];
	    } else {
		current++;
	    }
	}
    }
    DBUG_VOID_RETURN;
}

void punt ()
{
    register short cntr;
    register ssentry *current;
    register unsigned firstsdi;
    register ssentry *outptr;

    DBUG_ENTER ("punt");
    firstsdi = sdicnt;
    for (cntr = ssentctr, current = &selset[0]; cntr-- >= 0; ++current) {
	if (current -> sdicnt < firstsdi) {
	    firstsdi = current -> sdicnt;
	    outptr = current;
	}
    }
    update (outptr, L_SDI);
    *outptr = selset[ssentctr--];
    expand ();
    DBUG_VOID_RETURN;
}

void newlab (sym)
register symbol *sym;
{
    static short labwarn = YES;

    DBUG_ENTER ("newlab");
    if (++labctr == MAXLAB) {
	if (labwarn == YES) {
	    as_error (ERR_TBLOVFOP, "labels");
	    labwarn = NO;	/* don't warn again */
	}
	labctr--;		/* gone too far, back up */
	while (labctr == MAXLAB - 1) {
	    punt ();
	}			/* continue to punt until we free a label */
	labctr++;		/* now point to a free area */
    }
    labset[labctr] = sym;
    DBUG_VOID_RETURN;
}

void deflab (sym)
register symbol *sym;
{
    DBUG_ENTER ("deflab");
    sym -> maxval = PCmax;
    if (ssentctr >= 0) {
	newlab (sym);
	expand ();
    }
    DBUG_VOID_RETURN;
}

void fixsyms ()
{
    DBUG_ENTER ("fixsyms");
    notdone = NO;
    expand ();
    resolvealiases ();
    DBUG_VOID_RETURN;
}


