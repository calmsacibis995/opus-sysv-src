#ifndef __Ipoll
#define __Ipoll

struct pollfd {
	int fd;				/* file desc to poll */
	short events;			/* events of interest on fd */
	short revents;			/* events that occurred on fd */
};

#define POLLIN		01		/* fd is readable */
#define POLLPRI		02		/* priority info at fd */
#define	POLLOUT		04		/* fd is writeable (won't block) */

#define POLLERR		010		/* fd has error condition */
#define POLLHUP		020		/* fd has been hung up on */
#define POLLNVAL	040		/* invalid pollfd entry */

#define NPOLLFILE	20

#endif
