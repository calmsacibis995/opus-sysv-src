#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * stat structure, used by stat(2) and fstat(2)
 */
struct	stat {
	dev_t		st_dev;
	ino_t		st_ino;
	mode_t 		st_mode;
	nlink_t  	st_nlink;
	uid_t 		st_uid;
	gid_t 		st_gid;
	dev_t		st_rdev;
	off_t		st_size;
	time_t		st_atime;
	unsigned long	st_ausec;
	time_t		st_mtime;
	unsigned long	st_musec;
	time_t		st_ctime;
	unsigned long	st_cusec;
	char		st_padding[456];
};

#define		S_IFMT		0170000	/* type of file */
#define		S_IFDIR		0040000	/* directory */
#define		S_IFCHR		0020000	/* character special */
#define		S_IFBLK		0060000	/* block special */
#define		S_IFREG		0100000	/* regular */
#define		S_IFIFO		0010000	/* fifo */
#define		S_IFLNK		0120000	/* symbolic links */
#define		S_IFSOCK	0140000	/* sockets */
#define	S_ISUID	04000		/* set user id on execution */
#define	S_ISGID	02000		/* set group id on execution */
#define	S_ISVTX	01000		/* save swapped text even after use */
#define	S_IREAD	00400		/* read permission, owner */
#define	S_IWRITE	00200	/* write permission, owner */
#define	S_IEXEC	00100		/* execute/search permission, owner */
#define	S_ENFMT	S_ISGID		/* record locking enforcement flag */
#define	S_IRWXU	00700		/* read, write, execute: owner */
#define	S_IRUSR	00400		/* read permission: owner */
#define	S_IWUSR	00200		/* write permission: owner */
#define	S_IXUSR	00100		/* execute permission: owner */
#define	S_IRWXG	00070		/* read, write, execute: group */
#define	S_IRGRP	00040		/* read permission: group */
#define	S_IWGRP	00020		/* write permission: group */
#define	S_IXGRP	00010		/* execute permission: group */
#define	S_IRWXO	00007		/* read, write, execute: other */
#define	S_IROTH	00004		/* read permission: other */
#define	S_IWOTH	00002		/* write permission: other */
#define	S_IXOTH	00001		/* execute permission: other */

#ifndef __88000_OCS_DEFINED

#define	S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define	S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define	S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define	S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define	S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)

/*
 * stat structure, used by old stat(2) and old fstat(2)
 */
/* DELETE LATER. MMV */
struct	ostat {
	short	st_dev;
	ushort	st_ino;
	ushort 	st_mode;
	short  	st_nlink;
	ushort 	st_uid;
	ushort 	st_gid;
	short	st_rdev;
	off_t	st_size;
	time_t	st_atime;
	time_t	st_mtime;
	time_t	st_ctime;
};
#endif	/* ! __88000_OCS_DEFINED */

#endif	/* ! _SYS_STAT_H_ */
