/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/getstab.c	1.7" */

/*
 *
 *	"getstab.c"  is  a  file  that contains routines for post-pass1
 *	processing of the symbol table.   The  following  routines  are
 *	provided:
 *
 *	fixstab(addr,incr,type)	This  function  "fixes" elements of the
 *			symbol table by adding  "incr"  to  all  values
 *			of symbols of type "type" that are greater than
 *			"addr".   This  is necessary when the length of
 *			of an instruction is increased.
 *
 */

#include <stdio.h>
#include "systems.h"
#include "gendefs.h"
#include "symbols.h"
#include "dbug.h"		/* Special macro based C debugging package */

/*
 *
 *	"fixstab" is a function that "fixes" elements of the symbol
 *	table by adding "incr" to the value of all symbols of section number
 *	"sect" with a value greater than "addr".  This function stores
 *	its parameters into the global variables "fxaddr", "fxincr",
 *	and "fxsect".  It then calls "traverse" causing it to call
 *	"fix" to fix all elements in the table of ordinary symbols,
 *	and calls "ltraverse" causing it to call "lfix" to fix all
 *	elements in the table of compiler generated symbols.
 *
 */

static long fxaddr;
static long fxincr;
static short fxsect;

extern void traverse ();

void fix (ptr)
symbol *ptr;
{
    DBUG_ENTER ("fix");
    if ((ptr -> sectnum == fxsect) && (ptr -> value >= fxaddr)) {
	ptr -> value += fxincr;
    }
    DBUG_VOID_RETURN;
}

void fixstab (addr, incr, sect)
long addr;
long incr;
int sect;
{
    DBUG_ENTER ("fixstab");
    DBUG_PRINT ("fix", ("add %lx to all symbols in section %d with addr > %lx",
	    incr, sect, addr));
    fxaddr = addr;
    fxincr = incr;
    fxsect = sect;
    traverse ((int (*)()) fix);
    DBUG_VOID_RETURN;
}
