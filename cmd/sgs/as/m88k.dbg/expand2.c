/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident "$Header: expand2.c 2.2 88/04/08 14:02:27 root Exp $ RISC Source Base"

#include "systems.h"
#include "symbols.h"
#include "expand.h"
#include "expand2.h"
#include "dbug.h"

/*
 *	This array contains the upper and lower bounds of the span
 *	permitted for a short form of a span dependent instruction
 *	(sdi).  This array must be initialized in the same units
 *	as dot,	 e.g. bytes. Also don't forget that the structure
 *	expects long values.
 */

rangetag range[NITYPES] = {
    {
	-130L, 125L
    },				/* UBRB	 */
    {
	-130L, 125L
    },				/* CBRB	 */
    {
	-130L, 125L
    },				/* BSBB	 */
#ifdef	CALLPCREL
    {
	-130L, 125L
    },				/* PCBRELCALL */
    {
	-32771L, 32764L
    },				/* PCHRELCALL */
#endif
    {
	-32771L, 32764L
    },				/* UBRH	 */
    {
	-32771L, 32764L
    },				/* CBRH	 */
    {
	-32771L, 32764L
    },				/* BSBH	 */
};

/*
 *	This array contains the size of the short form of a sdi.
 *	The size must be in the same units that are used by dot,
 *	e.g. bytes.
 */

char pcincr[NITYPES] = {
    2,
    2,
    3,
#ifdef	CALLPCREL
    2,
    3,
#endif
    3,
    3,
    4
};

/*
 *	This array contains the difference between the size of
 *	the long form of a sdi and the short form. Again the difference
 *	must be in the same units as dot.
 */

char idelta[NITYPES] = {
    4,
    6,
    4,
#ifdef	CALLPCREL
    3,
    2,
#endif
    3,
    5,
    3
};
