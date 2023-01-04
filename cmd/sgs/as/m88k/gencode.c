/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident "$Header: gencode.c 2.2 88/04/08 14:05:02 root Exp $ RISC Source Base"

#include <stdio.h>
#include "systems.h"
#include "symbols.h"
#include "program.h"
#include "instab.h"
#include "gendefs.h"
#include "expand.h"
#include "expand2.h"
#include "errors.h"
#include "dbug.h"

extern void as_error ();

short pswopt = NO;

flags (flag)
char flag;
{
    char errmsg[28];

    DBUG_ENTER ("flags");
    switch (flag) {
	case 'G': 
	    pswopt = YES;
	    break;
	default: 
	    (void) sprintf (errmsg, "%c", flag);
	    as_error (ERR_UNKOPT, errmsg);
	    break;
    }
    DBUG_VOID_RETURN;
}
