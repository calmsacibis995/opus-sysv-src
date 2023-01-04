#ifndef _TIME_H_
#define _TIME_H_


/*	Copyright (c) 1989 OPUS Systems	*/
/*	      All Rights Reserved 		*/


/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	3.0 SID #	1.2	*/

#ifndef _TIME_
#define	_TIME_

#ifndef _CLOCK_T_
#define	_CLOCK_T_
typedef long	clock_t;
#endif

#ifndef _TIME_T_
#define	_TIME_T_
typedef long	time_t;
#endif

#ifndef _SIZE_T_
#define	_SIZE_T_
typedef unsigned int	size_t;
#endif

struct	tm {	/* see ctime(3) */
	int	tm_sec;
	int	tm_min;
	int	tm_hour;
	int	tm_mday;
	int	tm_mon;
	int	tm_year;
	int	tm_wday;
	int	tm_yday;
	int	tm_isdst;
};

struct timeval {
	time_t	tv_sec;
	long	tv_usec;
};

struct itimerval {
	struct timeval	it_interval;
	struct timeval	it_value;
};

#define ITIMER_REAL	0
#define ITIMER_VIRTUAL	1
#define ITIMER_PROF	2

#define	CLOCKS_PER_SEC	1000000

#ifndef __88000_OCS_DEFINED

#define	CLK_TCK		60

#ifndef NULL
#define NULL		0
#endif /* !NULL */

struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};
#define	DST_NONE	0	/* not on dst */
#define	DST_USA		1	/* USA style dst */
#define	DST_AUST	2	/* Australian style dst */
#define	DST_WET		3	/* Western European dst */
#define	DST_MET		4	/* Middle European dst */
#define	DST_EET		5	/* Eastern European dst */
#define	DST_CAN		6	/* Canada */

/*
 * Operations on timevals.
 *
 * NB: timercmp does not work for >= or <=.
 */
#define	timerisset(tvp)		((tvp)->tv_sec || (tvp)->tv_usec)
#define	timercmp(tvp, uvp, cmp)	\
	((tvp)->tv_sec cmp (uvp)->tv_sec || \
	 (tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec)
#define	timerclear(tvp)		(tvp)->tv_sec = (tvp)->tv_usec = 0

#ifndef INKERNEL
extern struct tm *gmtime(), *localtime();
extern char *ctime(), *asctime();
int  cftime(),  ascftime();
extern void tzset();
extern long timezone, altzone;
extern int daylight;
extern char *tzname[];
extern time_t mktime();
#endif
#endif	/* ! __88000_OCS_DEFINED */
#endif  /* _TIME_ */

#endif	/* ! _TIME_H_ */
