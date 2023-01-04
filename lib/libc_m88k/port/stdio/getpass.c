/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/getpass.c	1.11"
/*	3.0 SID #	1.4	*/
/*LINTLIBRARY*/
#include <stdio.h>
#include <signal.h>
#include <termio.h>

extern void _setbuf();
extern FILE *_fopen();
extern int _fclose(), fprintf(), findiop();
extern int _kill(), _ioctl(), _getpid();
static int intrupt;
extern	void(*_signal())();

#define	MAXPASSWD	8	/* max significant characters in password */

static char dev_tty[] = "/dev/tty";


char *
_getpass(prompt)
char	*prompt;
{
	struct termio ttyb;
	unsigned short flags;
	register char *p;
	register int c;
	FILE	*fi;
	static char pbuf[ MAXPASSWD + 1 ];
	int	 catch();
	void     (*sig)();

	if((fi = _fopen(dev_tty,"r")) == NULL)
		return((char*)NULL);
	_setbuf(fi, (char*)NULL);
	sig = _signal(SIGINT, catch);
	intrupt = 0;
	(void) _ioctl(fileno(fi), TCGETA, &ttyb);
	flags = ttyb.c_lflag;
	ttyb.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
	(void) _ioctl(fileno(fi), TCSETAF, &ttyb);
	(void) _fputs(prompt, stderr);
	p = pbuf;
	while( !intrupt  &&
		(c = getc(fi)) != '\n'  &&  c != '\r'  &&  c != EOF ) {
		if(p < &pbuf[ MAXPASSWD ])
			*p++ = c;
	}
	*p = '\0';
	ttyb.c_lflag = flags;
	(void) _ioctl(fileno(fi), TCSETAW, &ttyb);
	(void) putc('\n', stderr);
	(void) _signal(SIGINT, sig);
	if(fi != stdin)
		(void) _fclose(fi);
	if(intrupt)
		(void) _kill(_getpid(), SIGINT);
	return(pbuf);
}

static int
catch()
{
	++intrupt;
}
