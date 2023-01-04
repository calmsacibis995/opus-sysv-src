#ifndef __Itypes
#define __Itypes

#define MAXSUSE		255		/* maximum share count on swap */

typedef	unsigned short	ushort;
typedef	unsigned int	uint;

typedef unsigned char	use_t;
typedef char		swck_t;

typedef ushort		iord_t;
typedef ushort		pgadr_t;
typedef short		cnt_t;

typedef uint		size_t;

typedef	long		time_t;
typedef	long		off_t;
typedef	long		paddr_t;
typedef	long		key_t;
typedef long		clock_t;

typedef	char *		caddr_t;
typedef	char *		laddr_t;

typedef	struct { int r[1]; } *	physadr;

#ifdef m88k
typedef	unsigned long	ino_t;
typedef	unsigned long	mode_t;
typedef long		pid_t;
typedef	unsigned long	dev_t;
typedef	unsigned long	uid_t;
typedef	unsigned long	gid_t;
typedef unsigned long	nlink_t;
typedef	long		daddr_t;
#else
typedef	ushort		ino_t;
typedef	ushort		mode_t;
typedef short		pid_t;
typedef	short		dev_t;
typedef	ushort		uid_t;
typedef	ushort		gid_t;
typedef short		nlink_t;
typedef	unsigned long	daddr_t;
#endif

#endif
