#ifndef __fcntl
#define __fcntl

/* Flags used by open and fcntl */
#define	O_RDONLY	00000	/* read		*/
#define	O_WRONLY	00001	/* write	*/
#define	O_RDWR		00002	/* read/write	*/
#define O_ACCMODE	00003	/* rw-mask	*/
#define	O_NDELAY	00004	/* no buffering	*/
#define	O_APPEND	00010	/* at end	*/
#define O_SYNC		00020	/* sync writes	*/
#define O_NONBLOCK	00100
#define	O_CREAT		00400	/* open create	*/
#define	O_TRUNC		01000	/* open trunc	*/
#define	O_EXCL		02000	/* open exclus.	*/
#define O_NOCTTY	04000

/* fcntl flags */
#define	F_DUPFD	 0	/* dup fd	*/
#define	F_GETFD	 1	/* get fd flags	*/
#define	F_SETFD	 2	/* set fd flags */
#define	F_GETFL	 3	/* get file flg */
#define	F_SETFL	 4	/* set file flg */
#define	F_GETLK	 5	/* get lock	*/
#define	F_SETLK	 6	/* set lock	*/
#define	F_SETLKW 7	/* set and wait	*/
#define F_CHKFL	 8	/* check flags	*/
#define F_FREESP 11

/* file locking structure */
typedef struct flock {
	short	l_type;
	short	l_whence;
	long	l_start;
	long	l_len;
	int	l_pid;
} flock_t;

/* l_type */
#define	F_RDLCK	1	/* read lock	*/
#define	F_WRLCK	2	/* write lock	*/
#define	F_UNLCK	3	/* remove lock	*/

#define F_CLOEXEC 1

#endif
