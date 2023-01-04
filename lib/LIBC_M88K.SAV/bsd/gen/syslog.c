/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)syslog.c	5.9 (Berkeley) 5/7/86";
#endif /* LIBC_SCCS and not lint */

/*
 * SYSLOG -- print message on log file
 *
 * This routine looks a lot like printf, except that it
 * outputs to the log file instead of the standard output.
 * Also:
 *	adds a timestamp,
 *	prints the module name in front of the message,
 *	has some other formatting types (or will sometime),
 *	adds a newline on the end of the message.
 *
 * The output of this routine is intended to be read by /etc/syslogd.
 *
 * Author: Eric Allman
 * Modified to use UNIX domain IPC by Ralph Campbell
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <sys/syslog.h>
#include <netdb.h>
#include <string.h>
#include <sys/fcntl.h>

#define	MAXLINE	1024			/* max message size */
#define NULL	0			/* manifest */

#define PRIMASK(p)	(1 << ((p) & LOG_PRIMASK))
#define PRIFAC(p)	(((p) & LOG_FACMASK) >> 3)
#define IMPORTANT 	LOG_ERR

static char	logname[] = "/dev/log";
static char	ctty[] = "/dev/console";

static int	LogFile = -1;		/* fd for log */
static int	LogStat	= 0;		/* status bits, set by openlog() */
static char	*LogTag = "syslog";	/* string to tag the entry with */
static int	LogMask = 0xff;		/* mask of priorities to be logged */
static int	LogFacility = LOG_USER;	/* default facility code */

static struct sockaddr SyslogAddr;	/* AF_UNIX address of local logger */

extern	int errno, sys_nerr;
extern	char *sys_errlist[];

_syslog(pri, fmt, p0, p1, p2, p3, p4)
	int pri;
	char *fmt;
{
	char buf[MAXLINE + 1], outline[MAXLINE + 1];
	register char *b, *f, *o;
	register int c;
	long now;
	int pid, olderrno = errno;

	/* see if we should just throw out this message */
	if (pri <= 0 || PRIFAC(pri) >= LOG_NFACILITIES || (PRIMASK(pri) & LogMask) == 0)
		return;
	if (LogFile < 0)
		_openlog(LogTag, LogStat | LOG_NDELAY, 0);

	/* set default facility if none specified */
	if ((pri & LOG_FACMASK) == 0)
		pri |= LogFacility;

	/* build the message */
	o = outline;
	_sprintf(o, "<%d>", pri);
	o += _strlen(o);
	_time(&now);
	_sprintf(o, "%.15s ", _ctime(&now) + 4);
	o += _strlen(o);
	if (LogTag) {
		_strcpy(o, LogTag);
		o += _strlen(o);
	}
	if (LogStat & LOG_PID) {
		_sprintf(o, "[%d]", _getpid());
		o += _strlen(o);
	}
	if (LogTag) {
		_strcpy(o, ": ");
		o += 2;
	}

	b = buf;
	f = fmt;
	while ((c = *f++) != '\0' && c != '\n' && b < &buf[MAXLINE]) {
		if (c != '%') {
			*b++ = c;
			continue;
		}
		if ((c = *f++) != 'm') {
			*b++ = '%';
			*b++ = c;
			continue;
		}
		if ((unsigned)olderrno > sys_nerr)
			_sprintf(b, "error %d", olderrno);
		else
			_strcpy(b, sys_errlist[olderrno]);
		b += _strlen(b);
	}
	*b++ = '\n';
	*b = '\0';
	_sprintf(o, buf, p0, p1, p2, p3, p4);
	c = _strlen(outline);
	if (c > MAXLINE)
		c = MAXLINE;

	/* output the message to the local logger */
	if (_sendto(LogFile, outline, c, 0, &SyslogAddr, sizeof SyslogAddr) >= 0)
		return;
	if (!(LogStat & LOG_CONS))
		return;

	/* output the message to the console */
	pid = _fork();
	if (pid == -1)
		return;
	if (pid == 0) {
		int fd;
		sigset_t set;

		_signal(SIGALRM, SIG_DFL);
		/*
		sigsetmask(sigblock(0) & ~sigmask(SIGALRM));
		*/
		_sigprocmask(0, (sigset_t *) 0, &set);
		_sigdelset(&set, SIGALRM);
		_sigprocmask(SIG_SETMASK, &set, (sigset_t *) 0);
		_alarm(5);
		fd = _open(ctty, O_WRONLY);
		_alarm(0);
		_strcat(o, "\r");
		o = (char *) _index(outline, '>') + 1;
		_write(fd, o, c + 1 - (o - outline));
		_close(fd);
		_exit(0);
	}
	if (!(LogStat & LOG_NOWAIT))
		while ((c = _wait((int *)0)) > 0 && c != pid)
			;
}

/*
 * OPENLOG -- open system log
 */

_openlog(ident, logstat, logfac)
	char *ident;
	int logstat, logfac;
{
	if (ident != NULL)
		LogTag = ident;
	LogStat = logstat;
	if (logfac != 0)
		LogFacility = logfac & LOG_FACMASK;
	if (LogFile >= 0)
		return;
	SyslogAddr.sa_family = AF_UNIX;
	_strncpy(SyslogAddr.sa_data, logname, sizeof SyslogAddr.sa_data);
	if (LogStat & LOG_NDELAY) {
		LogFile = _socket(AF_UNIX, SOCK_DGRAM, 0);
		_fcntl(LogFile, F_SETFD, 1);
	}
}

/*
 * CLOSELOG -- close the system log
 */

_closelog()
{

	(void) _close(LogFile);
	LogFile = -1;
}

/*
 * SETLOGMASK -- set the log mask level
 */
_setlogmask(pmask)
	int pmask;
{
	int omask;

	omask = LogMask;
	if (pmask != 0)
		LogMask = pmask;
	return (omask);
}
