#ident "@(#) $Header: filehdr.c 4.1 88/04/26 17:03:26 root Exp $ RISC Source Base"
#include <stdio.h>
#include <a.out.h>

/*
 *	On System V machines, <a.out.h> automatically includes the
 *	following files.  If this is a BSD host instead, then 
 *	explicitly include them and hope that our include path
 *	points somewhere where they will be found.
 */

#ifndef FILHDR
#include "filehdr.h"
#endif

#include <ldfcn.h>

#include "coffdump.h"

struct tran {
    CHAR *name;
    USHORT value;
};

static struct tran magics[] = {
    TRANSPAIR (B16MAGIC),
    TRANSPAIR (BTVMAGIC),
    TRANSPAIR (X86MAGIC),
    TRANSPAIR (XTVMAGIC),
    TRANSPAIR (N3BMAGIC),
    TRANSPAIR (NTVMAGIC),
#ifdef XLMAGIC
    TRANSPAIR (XLMAGIC),
#endif
    TRANSPAIR (FBOMAGIC),
    TRANSPAIR (RBOMAGIC),
#ifdef CT	/* Convergent Technologies */
    TRANSPAIR (M80MAGIC),
#endif
    TRANSPAIR (MTVMAGIC),
    TRANSPAIR (VAXWRMAGIC),
    TRANSPAIR (VAXROMAGIC),
#ifdef CT
    TRANSPAIR (MC68KWRMAGIC),
    TRANSPAIR (MC68KROMAGIC),
    TRANSPAIR (MC68KPGMAGIC),
    TRANSPAIR (ARTYPE),
#endif
#ifdef V68		/* Microsystems generic port */
    TRANSPAIR (MC68MAGIC),
    TRANSPAIR (MC68TVMAGIC),
    TRANSPAIR (MC68MAGIC),
    TRANSPAIR (M68MAGIC),
    TRANSPAIR (M68TVMAGIC),
    TRANSPAIR (U370WRMAGIC),
    TRANSPAIR (U370ROMAGIC),
    TRANSPAIR (ARTYPE),
#endif
#ifdef MC88MAGIC
    TRANSPAIR (MC88MAGIC),
#endif
    NULL, 0
};

static struct tran tflags[] = {
    TRANSPAIR (F_RELFLG),
    TRANSPAIR (F_EXEC),
    TRANSPAIR (F_LNNO),
    TRANSPAIR (F_LSYMS),
    TRANSPAIR (F_MINMAL),
    TRANSPAIR (F_UPDATE),
    TRANSPAIR (F_SWABD),
    TRANSPAIR (F_AR16WR),
    TRANSPAIR (F_AR32WR),
    TRANSPAIR (F_AR32W),
    TRANSPAIR (F_PATCH),
#ifdef V68
    TRANSPAIR (F_NODF),
#endif
    NULL
};
    
static CHAR pbuf [128];


/*
 *  FUNCTION
 *
 *	TransMagic    translate magic number to symbolic name
 *
 *  SYNOPSIS
 *
 *	CHAR *TransMagic (value)
 *	USHORT value;
 *
 *  DESCRIPTION
 *
 *	Given a numeric value <value> from the f_magic field in
 *	the file header structure, translates the numeric value to
 *	a symbolic value.  If the corresponding symbol is not
 *	found for any reason, a pointer to a static buffer containing
 *	a printable ascii string with the matching value of <value>
 *	is returned.
 *
 */
 
CHAR *TransMagic (value)
USHORT value;
{
    register struct tran *tp;
    register CHAR *name;
    
    DBUG_ENTER ("TransMagic");
    name = NULL;
    for (tp = magics; tp -> name != NULL; tp++) {
	if (tp -> value == value) {
	    name = tp -> name;
	    break;
	}
    }
    if (name == NULL) {
	sprintf (pbuf, "%#lo", value);
	name = pbuf;
    }
    DBUG_RETURN (name);
}


/*
 *  FUNCTION
 *
 *	TransFlags    translate flag bits to symbolics
 *
 *  SYNOPSIS
 *
 *	CHAR *TransFlags (value)
 *	USHORT value;
 *
 *  DESCRIPTION
 *
 *	Given a numeric value <value> from the f_flags field in
 *	the file header structure, translates the numeric value to
 *	a symbolic value.  If the corresponding symbol(s) is(are) not
 *	found for any reason, a pointer to a static buffer containing
 *	a printable ascii string with the matching value of <value>
 *	is returned.
 *
 */
 
CHAR *TransFlags (value)
USHORT value;
{
    register struct tran *tp;
    register CHAR *name;
    register CHAR *in;
    register CHAR *out;
    
    DBUG_ENTER ("TransFlags");
    DBUG_3 ("flags", "f_flags word is %o", value);
    out = pbuf;
    for (tp = tflags; tp -> name != NULL; tp++) {
	if (tp -> value & value) {
	    if (out != pbuf) {
		*out++ = ' ';
		*out++ = '|';
		*out++ = ' ';
	    }
	    in = tp -> name;
	    while (*in != EOS) {
		*out++ = *in++;
	    }
	}
    }
    if (out == pbuf) {
	sprintf (pbuf, "%#lo", value);
    } else {
	*out = EOS;
    }
    DBUG_RETURN ((CHAR *)pbuf);
}


BOOLEAN GoodMagic (magic)
USHORT magic;
{
    register struct tran *tp;
    register BOOLEAN result;

    DBUG_ENTER ("GoodMagic");
    result = FALSE;
    for (tp = magics; tp -> name != NULL; tp++) {
	if (tp -> value == magic) {
	    result = TRUE;
	    break;
	}
    }
    DBUG_RETURN (result);
}
