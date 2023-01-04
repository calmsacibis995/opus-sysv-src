#ifndef _LIMITS_H_
#define _LIMITS_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
        /usr/group Standards File limits.h for System V on M88000"
*/


#define	CHAR_BIT	8		/* # of bits in a "char" */
#define MB_LEN_MAX	5		/* */
#define SCHAR_MIN	(-128)		/* min value of a "signed char" */
#define SCHAR_MAX	127		/* max value of a "signed char" */
#define UCHAR_MAX	255		/* max value of a "unsigned char" */
#define	CHAR_MIN	(-128)		/* min value of a "char" */
#define	CHAR_MAX	127		/* max value of a "char" */
#define SHRT_MIN	(-32768)	/* min value of a "short" */
#define SHRT_MAX	32767		/* max value of a "short" */
#define USHRT_MAX	65535		/* max value of a "unsigned short" */
#define	INT_MIN		-2147483648	/* min value of an "int" */
#define	INT_MAX		2147483647	/* max value of an "int" */
#define	UINT_MAX	4294967295	/* max value of an "unsigned int" */
#define	LONG_MIN	-2147483648	/* min value of a "long" */
#define	LONG_MAX	2147483647	/* max value of a "long" */
#define	ULONG_MAX	4294967295	/* max value of a "unsigned long" */
#define CHAR_BIT	8		/* number of bits in a "char" */

#define _POSIX_ARG_MAX		4096
#define _POSIX_CHILD_MAX	6
#define _POSIX_LINK_MAX		8
#define _POSIX_MAX_CANON	255
#define _POSIX_MAX_INPUT	255
#define _POSIX_NAME_MAX		14
#define _POSIX_NGROUPS_MAX	0
#define _POSIX_OPEN_MAX		16
#define _POSIX_PATH_MAX		255
#define _POSIX_PIPE_BUF		512

#undef NGROUPS_MAX
#undef ARG_MAX
#undef CHILD_MAX
#undef OPEN_MAX
#undef LINK_MAX
#undef MAX_CANON
#undef MAX_INPUT
#undef NAME_MAX
#undef PATH_MAX
#undef PIPE_BUF

#ifndef __88000_OCS_DEFINED
#define	CLK_TCK		60		/* # of clock ticks per second */
#define	DBL_DIG		15		/* digits of precision of a "double" */
#define	FCHR_MAX	1048576		/* max size of a file in bytes */
#define	FLT_DIG		7		/* digits of precision of a "float" */
#define	PASS_MAX	8		/* max # of characters in a password */
#define	PID_MAX		30000		/* max value for a process ID */
#define	PIPE_MAX	5120		/* max # bytes written to a pipe in a write */
#define	STD_BLK		1024		/* # bytes in a physical I/O block */
#define	SYS_NMLN	256		/* # of chars in uname-returned strings */
#define	UID_MAX		60000		/* max value for a user or group ID */
#define	USI_MAX		4294967295	/* max decimal value of an "unsigned" */
#define	WORD_BIT	32		/* # of bits in a "word" or "int" */
#endif	/* ! __88000_OCS_DEFINED */

#endif	/* ! _LIMITS_H_ */
