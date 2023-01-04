#ifndef _PWD_H_
#define _PWD_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

struct passwd {
	char	*pw_name;
	char	*pw_passwd;
	int	pw_uid;
	int	pw_gid;
	char	*pw_age;
	char	*pw_comment;
	char	*pw_gecos;
	char	*pw_dir;
	char	*pw_shell;
};

#ifndef _UID_T_
#define	_UID_T_
typedef unsigned long uid_t;
#endif

#ifndef __88000_OCS_DEFINED	
struct comment {
	char	*c_dept;
	char	*c_name;
	char	*c_acct;
	char	*c_bin;
};
struct	passwd	*getpwent(), *getpwuid(), *getpwnam();
#endif	/* ! __88000_OCS_DEFINED */

#endif	/* ! _PWD_H_ */
