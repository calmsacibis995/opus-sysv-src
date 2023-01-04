h29166
s 00167/00000/00000
d D 1.1 90/03/06 12:39:00 root 1 0
c date and time created 90/03/06 12:39:00 by root
e
u
U
t
T
I 1

#include "param.h"
#include "systm.h"
#include "dir.h"
#include "user.h"
#include "proc.h"
#include "file.h"
#include "inode.h"
#include "buf.h"
#include "mbuf.h"
#include "un.h"
#include "domain.h"
#include "protosw.h"
#include "socket.h"
#include "socketvar.h"
#include "stat.h"
#include "ioctl.h"
#include "uio.h"
#include "../netinet/in.h"

extern wakeup();

kudp_socket(aso, sa)
struct socket **aso;
struct sockaddr_in *sa;
{
	int retval;
	struct mbuf *nam;

	retval = socreate(PF_INET, aso, SOCK_DGRAM, IPPROTO_UDP);
	if (retval) {
		printf("socreate return %d\n", retval);
		return(1);
	}
	if (sa) {
		if (sockargs(&nam, sa, sizeof(struct sockaddr_in), 
			MT_SONAME)) {
			soclose(*aso);
			return(1);
		}
		if (sobind(*aso, nam)) {
			soclose(*aso);
			m_freem(nam);
			return(1);
		}
		m_freem(nam);
	}
	return(0);
}

extern soreceive(), sosend();

kudp_recvfrom(so, addr, base, count)
struct socket *so;
struct sockaddr_in *addr;
caddr_t base;
{
	int cp;

	if (u.u_error = kudp_rw(so, addr, base, count, soreceive, &cp))
		return(0);
	else
		return(cp);
}

kudp_sendto(so, addr, base, count)
struct socket *so;
struct sockaddr_in *addr;
caddr_t base;
{
	int cp;

	return(kudp_rw(so, addr, base, count, sosend, &cp));
}

kudp_rw(so, addr, base, count, func, cp)
struct socket *so;
struct sockaddr_in *addr;
caddr_t base;
int (*func)();
int *cp;
{
	
	struct uio auio;
	struct iovec aiov;
	int err;
	struct mbuf *m;
	int s;

	if (so->so_ip->i_ftype != IFMPB) {
		printf("%x is not socket\n", so);
		return (EINVAL);
	}
	aiov.iov_base = base;
	aiov.iov_len = count;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_resid = count;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_offset = 0;

	s = spl7();
	if (func == sosend)  {
		m = m_get(M_DONTWAIT, MT_SONAME);
		if (m == NULL) {
			splx(s);
			return(ENOBUFS);
		}
		m->m_len = sizeof(*addr);
		bcopy((caddr_t)addr, mtod(m, caddr_t), sizeof(*addr));
		err = (*func)(so, m, &auio, 0, 0);
	} else {
		err = (*func)(so, &m, &auio, 0, 0);
		bcopy(mtod(m, caddr_t), (caddr_t) addr, sizeof(*addr));
	}
	if (m)
		m_freem(m);
	splx(s);

	*cp = count - auio.uio_resid;
	return(err);
}

kudp_wait(so, to)
struct socket *so;
{
	register struct sockbuf *sb;
	int s, id, i;

	s = spl7();

	u.u_error = 0;
	sb = &so->so_rcv;
	if (sb->sb_cc) {
		splx(s);
		return(0);
	}

	id = timeout(wakeup, (caddr_t)&sb->sb_cc, to);
	sb->sb_flags |= SB_WAIT;
	i = sleep((caddr_t)&sb->sb_cc,  (PZERO+1)|PCATCH);
	sb->sb_flags &= ~SB_WAIT;
	untimeout(id);

	if (i) {				/* interrupted */
		u.u_error = EINTR;
		splx(s);
		return(-1);
	}
	else if (sb->sb_cc == 0) {		/* timeout */
		u.u_error = ETIME;
		splx(s);
		return(-1);
	}
	splx(s);
	return(0);
}

#if 0
kudp_close(so)
struct socket *so;
{
	register i;
	i = soclose(so);
	return(i ? -1 : 0);
}
#endif
E 1
