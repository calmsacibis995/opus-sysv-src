/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)uadmin:uadmin.c	1.2"
#include <sys/uadmin.h>

main(argc, argv)
char *argv[];
{
	register cmd, fcn;

/*
** Following lines have been changed to conform to 531 Release from 
** Austin - Change made by PEG 1/30/88
**
**	cmd = atoi(argv[1]);
**	fcn = atoi(argv[2]);
**	uadmin(cmd, fcn, 0);
*/

	if (argc == 3)	/* NULL POINTER FIX 12/10/86 e.j.r */
	{
		cmd = atoi(argv[1]);
		fcn = atoi(argv[2]);
		uadmin(cmd, fcn, 0);
	}
	perror("uadmin");
}
