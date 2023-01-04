#ifndef _GRP_H_
#define _GRP_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

struct	group {	/* see getgrent(3) */
	char	*gr_name;
	char	*gr_passwd;
	int	gr_gid;
	char	**gr_mem;
};

#ifndef _GID_T_
#define _GID_T_
typedef	unsigned long	gid_t;
#endif

#ifndef __88000_OCS_DEFINED
struct group *getgrent(), *getgrgid(), *getgrnam();
#endif
#endif	/* ! _GRP_H_ */
