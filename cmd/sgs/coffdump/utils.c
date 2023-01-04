#ident "@(#) $Header: utils.c 4.1 88/04/26 17:03:21 root Exp $ RISC Source Base"
#include <stdio.h>
#include "coffdump.h"

static char pbuf[128];


VOID vecprintf (fp, vector)
FILE *fp;
register char **vector;
{
    DBUG_ENTER ("vecprintf");
    while (*vector != NULL) {
	fprintf (fp, "%s\n", *vector++);
    }
    DBUG_VOID_RETURN;
}


CHAR *LongTuple (value)
LONG value;
{
    DBUG_ENTER ("LongTuple");
    sprintf (pbuf, "%12ld %#12lo %#12lx", value, value, value);
    DBUG_RETURN ((CHAR *)pbuf);
}


CHAR *IntTuple (value)
INT value;
{
    DBUG_ENTER ("IntTuple");
    sprintf (pbuf, "%12d %#12o %#12x", value, value, value);
    DBUG_RETURN ((CHAR *)pbuf);
}


CHAR *UIntTuple (value)
UINT value;
{
    DBUG_ENTER ("UIntTuple");
    sprintf (pbuf, "%12u %#12o %#12x", value, value, value);
    DBUG_RETURN ((CHAR *)pbuf);
}
