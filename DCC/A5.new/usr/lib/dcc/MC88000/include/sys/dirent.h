#ifndef __Isdirent
#define __Isdirent

struct dirent {				/* directory entries	*/
	long		d_ino;		/* inode number		*/
	off_t		d_off;		/* offset of entry	*/
	unsigned short	d_reclen;	/* length of entry	*/
	char		d_name[1];	/* filename		*/
};

#define DIRENTBASESIZ	offsetof(struct dirent,d_name)
#define DIRENTSIZ(len)	((DIRENTBASESIZ + sizeof(long) + (len))/sizeof(long)*sizeof(long))

#define	MAXNAMLEN	512		/* max filename length	*/

#ifndef NAME_MAX
#define	NAME_MAX	(MAXNAMLEN - 1)
#endif

#endif
