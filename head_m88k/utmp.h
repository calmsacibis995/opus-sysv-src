#ifndef _UTMP_H_
#define _UTMP_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	<sys/types.h> must be included.					*/

struct utmp
  {
	char ut_user[8] ;		/* User login name */
	char ut_id[4] ; 		/* /etc/lines id(usually line #) */
	char ut_line[12] ;		/* device name (console, lnxx) */
	pid_t ut_pid ;			/* process id */
	short ut_type ; 		/* type of entry */
	char ut_pad1[2] ;
	struct exit_status
	  {
	    short e_termination ;	/* Process termination status */
	    short e_exit ;		/* Process exit status */
	  }
	ut_exit ;			/* The exit status of a process
					 * marked as DEAD_PROCESS.
					 */
	time_t ut_time ;		/* time entry was made */
	char ut_pad2[24] ;
  } ;

/*	Definitions for ut_type						*/

#define	EMPTY		0
#define	RUN_LVL		1
#define	BOOT_TIME	2
#define	OLD_TIME	3
#define	NEW_TIME	4
#define	INIT_PROCESS	5	/* Process spawned by "init" */
#define	LOGIN_PROCESS	6	/* A "getty" process waiting for login */
#define	USER_PROCESS	7	/* A user process */
#define	DEAD_PROCESS	8
#define	ACCOUNTING	9
#define FTP		128
#define REMOTE_LOGIN	129
#define REMOTE_PROCESS	130

#ifndef __88000_OCS_DEFINED
#define	UTMP_FILE	"/etc/utmp"
#define	WTMP_FILE	"/etc/wtmp"
#define	ut_name	ut_user

#define	UTMAXTYPE	REMOTE_PROCESS	/* Largest legal value of ut_type */

/*	Special strings or formats used in the "ut_line" field when	*/
/*	accounting for something other than a process.			*/
/*	No string for the ut_line field can be more than 11 chars +	*/
/*	a NULL in length.						*/

#define	RUNLVL_MSG	"run-level %c"
#define	BOOT_MSG	"system boot"
#define	OTIME_MSG	"old time"
#define	NTIME_MSG	"new time"
#endif	/* !__88000_OCS_DEFINED */

#endif	/* ! _UTMP_H_ */
