#ifndef __Istat
#define __Istat

struct	stat {
	dev_t	st_dev;
	ino_t	st_ino;
	mode_t 	st_mode;
	nlink_t	st_nlink;
	uid_t 	st_uid;
	gid_t 	st_gid;
	dev_t	st_rdev;
	off_t	st_size;
#ifdef m88k
	time_t		st_atime;
	unsigned long	st_ausec;
	time_t		st_mtime;
	unsigned long	st_musec;
	time_t		st_ctime;
	unsigned long	st_cusec;
	char		st_padding[456];
#else
	time_t	st_atime;
	time_t	st_mtime;
	time_t	st_ctime;
#endif
};

#define	S_IFMT	 0170000	/* type mask	*/
#define	S_IFREG	 0100000	/* regular	*/
#define	S_IFBLK	 0060000	/* block	*/
#define	S_IFDIR	 0040000	/* directory	*/
#define	S_IFCHR	 0020000	/* character	*/
#define	S_IFIFO	 0010000	/* fifo		*/
#define S_IFLNK	 0120000	/* symbolic link*/
#define S_IFSOCK 0140000	/* sockets	*/
#define	S_ISUID  0004000	/* s-bit	*/
#define	S_ISGID	 0002000	/* g-bit	*/
#define	S_ISVTX	 0001000	/* save text	*/
#define	S_IREAD	 0000400	/* read perm.	*/
#define	S_IWRITE 0000200	/* write perm.	*/
#define	S_IEXEC	 0000100	/* exec perm.	*/
#define	S_ENFMT	 S_ISGID	/* record locking enforcement flag	*/
#define	S_IRWXU	 0000700	/* read, write, execute: owner		*/
#define	S_IRUSR	 0000400	/* read permission: owner		*/
#define	S_IWUSR	 0000200	/* write permission: owner		*/
#define	S_IXUSR	 0000100	/* execute permission: owner		*/
#define	S_IRWXG	 0000070	/* read, write, execute: group		*/
#define	S_IRGRP	 0000040	/* read permission: group		*/
#define	S_IWGRP	 0000020	/* write permission: group		*/
#define	S_IXGRP	 0000010	/* execute permission: group		*/
#define	S_IRWXO	 0000007	/* read, write, execute: other		*/
#define	S_IROTH	 0000004	/* read permission: other		*/
#define	S_IWOTH	 0000002	/* write permission: other		*/
#define	S_IXOTH	 0000001	/* execute permission: other		*/

#define S_ISDIR(mode)	(((mode) & S_IFMT) == S_IFDIR)
#define S_ISCHR(mode)	(((mode) & S_IFMT) == S_IFCHR)
#define S_ISBLK(mode)	(((mode) & S_IFMT) == S_IFBLK)
#define S_ISREG(mode)	(((mode) & S_IFMT) == S_IFREG)
#define S_ISFIFO(mode)	(((mode) & S_IFMT) == S_IFIFO)

#endif
