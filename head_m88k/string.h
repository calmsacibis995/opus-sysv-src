#ifndef _STRING_H_
#define _STRING_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef _SIZE_T_
#define	_SIZE_T_
typedef unsigned int	size_t;
#endif

#ifndef __88000_OCS_DEFINED
extern char
	*strcpy(),
	*strncpy(),
	*strcat(),
	*strncat(),
	*strchr(),
	*strrchr(),
	*strpbrk(),
	*strtok(),
	*strdup(),
	*strstr();
extern int
	strcmp(),
	strncmp(),
	strlen(),
	strspn(),
	strcspn();
extern char
	*memccpy(),
	*memchr(),
	*memcpy(),
	*memset();
extern int memcmp();
extern char
	*index(),
	*rindex();
#endif

#endif	/* ! _STRING_H_ */
