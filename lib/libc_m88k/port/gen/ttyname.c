/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/ttyname.c	1.8"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * _ttyname(f): return "/dev/ttyXX" which the the name of the
 * tty belonging to file f.
 *
 * This program works in two passes: the first pass tries to
 * find the device by matching device and inode numbers; if
 * that doesn't work, it tries a second time, this time doing a
 * stat on every file in /dev and trying to match device numbers
 * only. If that fails too, NULL is returned.
 */

#define	NULL	0
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

extern int _stat(), _fstat(), _isatty();
extern char *_strcpy(), *_strcat();
struct dirent *_readdir();
DIR *_opendir();

static char rbuf[32], dev[]="/dev/";

char *
_ttyname(f)
int	f;
{
	struct stat fsb, tsb;
	register DIR *dirp;
	register struct dirent *dentry;
	register int pass1;

	if(_isatty(f) == 0)
		return(NULL);
	if(_fstat(f, &fsb) < 0)
		return(NULL);
	if((fsb.st_mode & S_IFMT) != S_IFCHR)
		return(NULL);
	if((dirp = _opendir(dev)) == NULL)
		return(NULL);
	pass1 = 1;
	do {
		while(dentry = _readdir(dirp)) {
			if(dentry->d_ino == 0)
				continue;
			if(pass1 && dentry->d_ino != fsb.st_ino)
				continue;
			(void) _strcpy(rbuf, dev);
			(void) _strcat(rbuf, dentry->d_name);
			if(_stat(rbuf, &tsb) < 0)
				continue;
			if(tsb.st_rdev == fsb.st_rdev &&
				(tsb.st_mode&S_IFMT) == S_IFCHR &&
				(!pass1 || tsb.st_ino == fsb.st_ino)) {
				(void) _closedir(dirp);
				return(rbuf);
			}
		}
		(void) _seekdir(dirp, 0L);
	} while(pass1--);
	(void) _closedir(dirp);
	return(NULL);
}
