#ident "@(#) $Header: scnhdr.c 4.1 88/04/26 17:03:04 root Exp $ RISC Source Base"
#include <stdio.h>
#include <a.out.h>

/*
 *	On System V machines, <a.out.h> automatically includes the
 *	following files.  If this is a BSD host instead, then 
 *	explicitly include them and hope that our include path
 *	points somewhere where they will be found.
 */

#ifndef SCNHDR
#include "scnhdr.h"
#endif

#include "coffdump.h"

struct tran {
    CHAR *name;
    LONG value;
};

/*
 *	The following defines are new for System V Release 3, and may
 *	not be in all System V scnhdr.h files.
 */

#ifndef STYP_INFO
#define STYP_INFO 0x200
#endif

#ifndef STYP_OVER
#define STYP_OVER 0x400
#endif

#ifndef STYP_LIB
#define STYP_LIB 0x800
#endif

static struct tran tflags[] = {
    TRANSPAIR (STYP_REG),
    TRANSPAIR (STYP_DSECT),
    TRANSPAIR (STYP_NOLOAD),
    TRANSPAIR (STYP_GROUP),
    TRANSPAIR (STYP_PAD),
    TRANSPAIR (STYP_COPY),
#ifdef V68
    TRANSPAIR (STYP_TEXT),
    TRANSPAIR (STYP_DATA),
    TRANSPAIR (STYP_BSS),
#endif
    TRANSPAIR (STYP_INFO),
    TRANSPAIR (STYP_LIB),
    TRANSPAIR (STYP_OVER),
#ifdef u3b
    TRANSPAIR (S_SHRSEG),
#endif
    NULL, 0
};
    
static CHAR pbuf[128];


/*
 *  FUNCTION
 *
 *	TransSflags    translate flag bits to symbolics
 *
 *  SYNOPSIS
 *
 *	CHAR *TransSflags (value)
 *	LONG value;
 *
 *  DESCRIPTION
 *
 *	Given a numeric value <value> from the s_flags field in
 *	the section header structure, translates the numeric value to
 *	a symbolic value.  If the corresponding symbol(s) is(are) not
 *	found for any reason, a pointer to a static buffer containing
 *	a printable ascii string with the matching value of <value>
 *	is returned.
 *
 */
 
CHAR *TransSflags (value)
USHORT value;
{
    register struct tran *tp;
    register CHAR *name;
    register CHAR *in;
    register CHAR *out;
    
    DBUG_ENTER ("TransSflags");
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
	sprintf (pbuf, "STYP_REG");
    } else {
	*out = EOS;
    }
    DBUG_RETURN ((CHAR *)pbuf);
}
