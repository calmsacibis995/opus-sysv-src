/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)mkshlib:m32/t_btassem.c	1.2"

#include <stdio.h>
#include "filehdr.h"
#include "ldfcn.h"
#include "shlib.h"
#include "syms.h"
#include "trg.h"


/* This function prints the leading lines of assemble code for the branch table */
void
prlbtassem(assembt)
FILE	*assembt;
{
	/* Print leading lines of assembly file.
	 * The beginning of the assemble file should look like:
	 *
	 *	.file	"<filename>"
	 *	.ident	"<ident string>"  #if ident string exists
	 *	.text
	 */
	/*
	 *(void)fprintf(assembt,".file\t\"branchtab\"\n");
	 *if (idstr != 0)
	 *	(void)fprintf(assembt,".ident\t\"%s\"\n",idstr);
	 *(void)fprintf(assembt,".text\n");
	 */

	(void)fprintf(assembt, "\tfile\t\"branchtab\"\n");
	if (idstr != 0)
		(void)fprintf(assembt,"\tident\t\"%s\"\n",idstr);
	(void)fprintf(assembt, "\ttext\n");
}


/* This function generates the assembly code for a label in the branch table */
/* MOTOROLA mwc 8/28/86 Changed for Motorola assembler */
void
labelassem(symname, assembt)
char	*symname;
FILE	*assembt;
{
	(void)fprintf(assembt,"\tglobal\t%s\n%s:\n",symname,symname);
}

/* This function generates the assembly code for the branch table */
/* MOTOROLA mwc 8/28/86 OK for Motorola assembler */
void
genbtassem(label, assembt)
char	*label;
FILE	*assembt;
{
	/* Each entry in the branch table will have the following format:
	 *
	 *		jmp	<label name>
	 */
/* Comment out 68K code.  kss  9/14/88 */
/*
	(void)fprintf(assembt,"\tjmp\t%s\n",label);
*/

/* Insert 88K code.  kss  9/14/88 */
	(void)fprintf(assembt,"\tor.u\tr10,r0,hi16(%s)\n",label);
	(void)fprintf(assembt,"\tor\tr10,r10,hi16(%s)\n",label);
	(void)fprintf(assembt,"\tjmp\tr10\n");
}


/* This function generates the assembly code for the .lib section */
/* MOTOROLA mwc 8/28/86 Changed for Motorola assembler */
void
genlibassem(assembt)
FILE	*assembt;
{
	int	size;	/* length of target pathname */
	int	i;
	char	*p;
	int	slong,
		len;

	/* The .lib contents will have the following format:
	 *
	 *	.section	.lib,"l"
	 *	.word	num,2	# where num is the size of the current entry
	 *			# i.e. length of the target pathname + other info.
	 *	.byte	<target pathname in hex>
	 */

	/* MOTOROLA format, data is changed to .lib by post processor
	 *	data
	 *	long	num,2
	 *	byte
	 */

	(void)fprintf(assembt,"\tsection\t.lib,\"l\"\n");

	len= strlen(trgpath) + 1;
	slong= sizeof(long);
	size= (len+(slong - 1))/slong + 2; 	/* the last term represents the two
						   leading words- the 1st describes
						   the total length and the 2nd
						   describes the offset of the
						   non-optional information */
	/* Account for section padding */
	while ((size % 8) != 0)
		size ++;
/* Comment out 68K code.  kss */
/*	(void)fprintf(assembt,"\tlong\t%d,2\n",size);
	(void)fprintf(assembt,"\tbyte\t");
	for (p=trgpath;*p!='\0';p++) {
		(void)fprintf(assembt,"0x%x,",*p);
	}
	(void)putc('0',assembt);
	(void)putc('\n',assembt);
*/

/* Insert 88K code.  kss */
	(void)fprintf(assembt,"\tword\t%d,2\n",size);
	(void)fprintf(assembt,"\tstring\t\"%s\"\n",trgpath);
}
