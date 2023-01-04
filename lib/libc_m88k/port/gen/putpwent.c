/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/putpwent.c	1.4"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * format a password file entry
 */
#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

extern int _fprintf();
static	_ispasswd();

int
_putpwent(p, f)
register struct passwd *p;
register FILE *f;
{
	/*
	 * Added for OCS: return -1 if the file we are writing
	 * is not zero length && is not a valid passwd file.
	 */
	if (_ispasswd(f) == 0)
		return (_IOERR);
	(void) _fprintf(f, "%s:%s", p->pw_name, p->pw_passwd);
	if((*p->pw_age) != '\0')
		(void) _fprintf(f, ",%s", p->pw_age);
	(void) _fprintf(f, ":%u:%u:%s:%s:%s",
		p->pw_uid,
		p->pw_gid,
		p->pw_gecos,
		p->pw_dir,
		p->pw_shell);
	(void) putc('\n', f);
	return(ferror(f));
}

static
_ispasswd(f)
register FILE *f;
{
	register int savepos, fd, retval;
	register char *p;
	struct stat sb;
	char buf[BUFSIZ];
	/*
	 * Added for OCS: return -1 if the file we are writing
	 * is not zero length && is not a valid passwd file.
	 */
	fd = fileno(f);

	savepos = _fcntl(fd, F_GETFL, 0);
	if (savepos == -1)		/* not a valid filedes */
		return 0;
	savepos &= O_ACCMODE;
	if (savepos == O_RDONLY)	/* can't write read only files */
		return 0;		/* no */
	if (savepos == O_WRONLY)	/* can't verify write only files */
		return 1;		/* yes */

	_fstat(fd, &sb);
	if (sb.st_size) {		/* non zero length */
		retval = 0;
		savepos = _lseek(fd, 0, SEEK_CUR);
		_lseek(fd, 0L, SEEK_SET);
		_memset(buf, 0, sizeof(buf));
		_read(fd, buf, sizeof(buf));
		for ( p = buf; p < &buf[sizeof (buf)] ; p++) {
			if (*p == '\n' || *p == '\0')
				break;
			if (*p == ':')
				retval++;
		}
		_lseek(fd, savepos, SEEK_SET);
		/* a passwd entry should have at least 6 :'s */
		/* we test for 5 */
		return (retval >= 5);	/* XXX */
	}
	return 1;
}
