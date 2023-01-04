#ident "@(#) $Header: reloc.c 4.1 88/04/26 17:02:59 root Exp $ RISC Source Base"
#include <stdio.h>
#include <a.out.h>

/*
 *	On System V machines, <a.out.h> automatically includes the
 *	following files.  If this is a BSD host instead, then 
 *	explicitly include them and hope that our include path
 *	points somewhere where they will be found.
 */

#ifndef RELOC
#include "reloc.h"
#endif

#include "coffdump.h"

struct relflg {
    CHAR *relname;
    SHORT rtype;
};

struct relflg relflgs[] = {
    TRANSPAIR (R_ABS),
    TRANSPAIR (R_OFF8),
    TRANSPAIR (R_OFF16),
    TRANSPAIR (R_SEG12),
    TRANSPAIR (R_AUX),
    TRANSPAIR (R_DIR16),
    TRANSPAIR (R_REL16),
    TRANSPAIR (R_IND16),
    TRANSPAIR (R_DIR24),
    TRANSPAIR (R_REL24),
    TRANSPAIR (R_OPT16),
    TRANSPAIR (R_IND24),
    TRANSPAIR (R_IND32),
    TRANSPAIR (R_DIR32),
    TRANSPAIR (R_DIR32S),
    TRANSPAIR (R_RELBYTE),
    TRANSPAIR (R_RELWORD),
    TRANSPAIR (R_RELLONG),
    TRANSPAIR (R_PCRBYTE),
    TRANSPAIR (R_PCRWORD),
    TRANSPAIR (R_PCRLONG),
#ifdef R_PCR26
    TRANSPAIR (R_PCR26),
#endif
#ifdef R_HPCRWORD
    TRANSPAIR (R_HPCRWORD),
#endif
#ifdef R_HRELWORD
    TRANSPAIR (R_HRELWORD),
#endif
    NULL, 0
};

static CHAR nbuf [12];


/*
 *  FUNCTION
 *
 *	TransReloc    translate relocation type to symbolic name
 *
 *  SYNOPSIS
 *
 *	CHAR *TransReloc (type)
 *	SHORT type;
 *
 *  DESCRIPTION
 *
 *	Given a numeric relocation type <type> from the r_type in
 *	the relocation structure, translates the numeric type to
 *	a symbolic type.  If the corresponding symbol is not
 *	found for any reason, a pointer to a static buffer containing
 *	a printable ascii string with the matching value of <type>
 *	is returned.
 *
 */
 
CHAR *TransReloc (type)
SHORT type;
{
    register struct relflg *fptr;
    register CHAR *name;
    
    DBUG_ENTER ("TransReloc");
    name = NULL;
    for (fptr = relflgs; fptr -> relname != NULL; fptr++) {
	if (fptr -> rtype == type) {
	    name = fptr -> relname;
	    break;
	}
    }
    if (name == NULL) {
	sprintf (nbuf, "%#8lo", type);
	name = nbuf;
    }
    DBUG_RETURN (name);
}
