#ifndef _SYS_WAIT_H_
#define _SYS_WAIT_H_

/*	Copyright (c) 1989 OPUS Systems	*/
/*	      All Rights Reserved 		*/

#define WSTOPPED	0177

#define WNOHANG		0x40
#define WUNTRACED	0x04

#define	WTERMSIG(s)	((s) & 0x7f)
#define	WEXITSTATUS(s)	(((s) & 0xff00) >> 8)
#define	WIFEXITED(s)	(WTERMSIG(s) == 0)
#define	WIFSTOPPED(s)	((WTERMSIG(s) == 0x7f) && (((s) & 0x80) == 0))
#define	WIFSIGNALED(s)	(!WIFEXITED(s) && !WIFSTOPPED(s))
#define	WSTOPSIG(s)	(WIFSTOPPED(s) ? WEXITSTATUS(s) : 0)

#endif /* ! _SYS_WAIT_H_ */
