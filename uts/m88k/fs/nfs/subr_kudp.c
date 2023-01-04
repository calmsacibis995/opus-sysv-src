
/* @(#)subr_kudp.c	1.2  */

/* nfs to bsd interface module */

#include "sys/machine.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/systm.h"
#include "sys/signal.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/sysmacros.h"
#include "sys/immu.h"
#if clipper || m88k
#include "sys/region.h"
#endif
#include "sys/proc.h"
#include "sys/var.h"
#include "sys/file.h"
#include "sys/inode.h"
#include "sys/errno.h"

extern int ec_inited;

/*
 * Create a udp socket and pass the resulting socket minor number
 * back through aso.
 */
udpsocket(aso, sa)
int *aso;
struct sockaddr_in *sa;
{
	if (ec_inited)
		return(kudp_socket(aso, sa));
	return(1);
}

int
ku_recvfrom(so, from, buf, len)
ulong so;
struct sockaddr *from;
caddr_t buf;
{
	return(kudp_recvfrom(so, from, buf, len));
}

int Sendtries = 0;
int Sendok = 0;

/*
 * Kernel sendto.
 */
int
ku_sendto(so, addr, buf, len)
ulong so;
caddr_t buf;
struct sockaddr_in *addr;
{
	int error;

	Sendtries++;
	if ((error = kudp_sendto(so, addr, buf, len)) == 0)
		Sendok++;
	return(error);
}

#if 0
/*
 * Bind the address in the mbuf m to the port
 * represented by the socket structure so.  The
 * address may be NULL.
 */
sobind(so, m)
struct socket *so;
{
	return(ENXIO);
}

soclose(so)
int so;
{
	return (kudp_close(so));
}
#endif

int
nfssbwait(dev, to)			/* ints off */
int to;
{
	return(kudp_wait(dev, to));
}

int
kudp_create(fp, flags)
register struct file *fp;
{
	return((int) fp->f_inode->i_fsptr);
}	
