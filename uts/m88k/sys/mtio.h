#ifndef _SYS_MTIO_H_
#define _SYS_MTIO_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

struct mtop	{
	short mt_op;
	char mt_pad[2];
	daddr_t mt_count;
};

#define MTWEOF	0
#define MTFSF	1
#define MTBSF	2
#define MTFSR	3
#define	MTBSR	4
#define MTREW	5
#define MTOFFL	6
#define MTNOP	7
#define MTCACHE	8
#define MTNOCACHE	9
#define MTFSS	10
#define MTBSS	11
#define MTERA	12
#define MTEND	13
#define MTTEN	14

#include <sys/ioctl.h>

#define MTIOCTOP	_IOW(m,1,struct mtop)


#endif	/* ! _SYS_MTIO_H_ */
