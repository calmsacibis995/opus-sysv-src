#ifndef __Istropts
#define __Istropts

#include <sys/ioctl.h>

#define RNORM 		0
#define RMSGD		1
#define RMSGN		2

#define S_INPUT		001
#define S_HIPRI		002
#define S_OUTPUT	004
#define S_MSG		010

#define RS_HIPRI	1

#define MORECTL		1
#define MOREDATA	2

#define	STR		('S'<<8)
#define I_NREAD		_IOR_('S',1,long)
#define I_PUSH		_IO_('S',2)
#define I_POP		_IO_('S',3)
#define I_LOOK		_IO_('S',4)
#define I_FLUSH		_IO_('S',5)
#define I_SRDOPT	_IO_('S',6)
#define I_GRDOPT	_IOR_('S',7,int)
#define I_STR		_IOWR_('S',8,struct strioctl)
#define I_SETSIG	_IO_('S',9)
#define I_GETSIG	_IOR_('S',10,long)
#define I_FIND		_IO_('S',11)
#define I_LINK		_IO_('S',12)
#define I_UNLINK	_IO_('S',13)
#define I_PEEK		_IOWR_('S',15,struct strpeek)
#define I_FDINSERT	_IOW_('S',16,struct strfdinsert)
#define I_SENDFD	_IO_('S',17)
#define I_RECVFD	_IOR_('S',18,struct strrecvfd)

struct strioctl {
	int 	ic_cmd;
	int	ic_timout;
	int	ic_len;
	char	*ic_dp;
};

struct strbuf {
	int	maxlen;
	int	len;
	char	*buf;
};

struct strpeek {
	struct strbuf ctlbuf;
	struct strbuf databuf;
	long	      flags;
};

struct strfdinsert {
	struct strbuf ctlbuf;
	struct strbuf databuf;
	long	      flags;
	int	      fildes;
	int	      offset;
};

struct strrecvfd {
	int fd;
	unsigned long uid;
	unsigned long gid;
	char fill[8];
};

#define FLUSHR 1			/* flush read queue */
#define FLUSHW 2			/* flush write queue */
#define FLUSHRW 3			/* flush both queues */

#define INFTIM		-1

#endif
