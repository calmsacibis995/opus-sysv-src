/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)mkshlib:m32/h_assem.c	1.2"

#include <stdio.h>
#include "filehdr.h"
#include "ldfcn.h"
#include "reloc.h"
#include "syms.h"
#include "scnhdr.h"
#include "shlib.h"
#include "hst.h"

/* prlal() prints the leading lines of an assembly file */
void
prlal(assemf, curname)
FILE	*assemf;
char	*curname;
{
	/* Print leading lines of assembly file.
	 * The beginning of the assembly file should look like:
	 *
	 *      .file   "<filename>"
	 *      .ident  "<ident string>"
	 *      .section	.init,"x"
	 */

	
	 /*(void)fprintf(assemf,".file   \"%s\"\n",curname);
	  *if (idstr != 0)
	  *	(void)fprintf(assemf,".ident  \"%s\"\n",idstr);
	  *(void)fputs(".section\t.init,\"x\"\n",assemf);
	  */ 

	(void)fprintf(assemf, "\tfile\t\"%s\"\n", curname);
	if (idstr != 0)
	 	(void)fprintf(assemf,"\tident\t\"%s\"\n",idstr);
	(void)fputs("\tsection\t.init,\"x\"\n",assemf);
}


/* Print initialization code */
void
initpr(assemf, import, pimport)
FILE	*assemf;
char	*import;
char	*pimport;
{
	/* Generate initialization code. MOTOROLA mwc 8/28/86
	* Each initialization specification line should result in
	* the generation of the following code:
	*
	*      mov.l    &<import> <pimport>
	*/
/* Comment out 68K code.  kss  9/14/88 */
/*	(void)fprintf(assemf,"mov.l\t&%s,%s\n",import,pimport);
*/
/* Insert 88K code.  kss  9/14/88 */
	(void)fprintf(assemf,"\tor.u\tr10,r0,hi16(%s)\n",import);
	(void)fprintf(assemf,"\tor.u\tr10,r0,lo16(%s)\n",import);
	(void)fprintf(assemf,"\tor.u\tr11,r0,hi16(%s)\n",pimport);
	(void)fprintf(assemf,"\tst\tr11,r0,lo16(%s)\n",pimport);
}
