#ident "@(#) $Header: utils.c 4.1 88/04/26 17:02:30 root Exp $ RISC Source Base"
#include <stdio.h>
#include "coffcmp.h"

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

