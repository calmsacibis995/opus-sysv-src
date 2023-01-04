
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-m88k:gen/setchrclass.c"

#include "lib_ctype.h"
#include <stdio.h>


static char first_call = 1;

extern char *_getenv(), *_strcpy(), *_memcpy();
extern unsigned char __ctype[];

int
_setchrclass(ccname)
char *ccname;
{
	char pathname[128];
	char my_ctype[SZ];		/* local copy */
	register int fd, ret = -1;

       /*
	* If no character class name supplied, use the CHRCLASS env. variable.
	*/
	if (ccname == 0 && ((ccname = _getenv("CHRCLASS")) == 0 || ccname[0] == '\0'))
	{
		/*
		* If this is the first call to _setchrclass() then there is
		* nothing else to do here.  If this is a subsequent call
		* to _setchrclass() and if _ctype[] was set to anything but
		* the default, the pathname[] array would have been changed.
		* Thus, in this case, actually do the reset to "ascii".
		*/
		if (first_call)
			return (0);
		ccname = DEFL_CCLASS;
	}
	first_call = 0;
	(void)_strcpy(pathname, CCLASS_ROOT);
	(void)_strcpy(&pathname[LEN_CC_ROOT], ccname);
	if ((fd = _open(pathname, O_RDONLY)) >= 0)
	{
		if (_read(fd, my_ctype, SZ) == SZ)
		{
			/*
			* Update _ctype[] only after everything works.
			*/
			(void)_memcpy(__ctype, my_ctype, SZ);
			ret = 0;
		}
		(void)_close(fd);
	}
	return (ret);
}
