h22943
s 00653/00000/00000
d D 1.1 90/03/06 12:32:52 root 1 0
c date and time created 90/03/06 12:32:52 by root
e
u
U
t
T
I 1
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)if_ec.c	7.1 (Berkeley) 6/5/86
 */

#if !pm100
#define	NEC	1
#if NEC > 0

/*
 * 3Com Ethernet Controller style interface
 */

#include "param.h"
#include "systm.h"
#include "mbuf.h"
#include "buf.h"
#include "protosw.h"
#include "socket.h"
#include "syslog.h"
#include "ioctl.h"
#include "errno.h"

#include "../net/if.h"
#include "../net/netisr.h"
#include "../net/route.h"

#ifdef INET
#include "../netinet/in.h"
#include "../netinet/in_systm.h"
#include "../netinet/in_var.h"
#include "../netinet/ip.h"
#include "../netinet/if_ether.h"
#endif

#ifdef NS
#include "../netns/ns.h"
#include "../netns/ns_if.h"
#endif

#include "if_ecreg.h"

#define	ECREAD	0
#define	ECWRITE	1
#define	NUMEC	(ECRHBF+1)
#define	RCVBUFSZ 2048

int	ecubamem(), ecprobe(), ecattach(), ecrint(), ecxint(), eccollide();
int	ec_inited;

int	ecinit(),ecioctl(),ecoutput(),ecreset();
struct	mbuf *ecget();

extern struct ifnet loif;
struct in_addr myinetid;

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * es_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, its address, ...
 * We also have, for each interface, a UBA interface structure, which
 * contains information about the UNIBUS resources held by the interface:
 * map registers, buffered data paths, etc.  Information is cached in this
 * structure for use by the if_uba.c routines in running the interface
 * efficiently.
 */
struct	ec_softc {
	struct	arpcom es_ac;		/* common Ethernet structures */
#define	es_if	es_ac.ac_if		/* network-visible interface */
#define	es_addr	es_ac.ac_enaddr		/* hardware Ethernet address */
	short	es_mask;		/* mask for current output delay */
	short	es_oactive;		/* is output active? */
	u_char	*es_buf[NUMEC];		/* virtual addresses of buffers */
	u_short	es_len[NUMEC];		/* len of above buffers */
} ec_softc[NEC];

/*
 * Interface exists: make available by filling in network interface
 * record.  System will initialize the interface when it is ready
 * to accept packets.
 */
ecattach()
{
	struct ec_softc *es = &ec_softc[0];
	register struct ifnet *ifp = &es->es_if;

	if (ecmeminit(es, NUMEC, RCVBUFSZ))
		return;
	ifp->if_unit = 0;
	ifp->if_name = "ec";
	ifp->if_mtu = ETHERMTU;

	ifp->if_init = ecinit;
	ifp->if_ioctl = ecioctl;
	ifp->if_output = ecoutput;
	ifp->if_reset = ecreset;
	ifp->if_flags = IFF_BROADCAST;
	if_attach(ifp);
}

/*
 * Reset of interface after UNIBUS reset.
 * If interface is on specified uba, reset its state.
 */
ecreset()
{

	ec_softc[0].es_if.if_flags &= ~IFF_RUNNING;
	ecinit(0);
}

/*
 * Initialization of interface; clear recorded pending
 * operations, and reinitialize UNIBUS usage.
 */
ecinit(unit)
	int unit;
{
	struct ec_softc *es = &ec_softc[unit];
	register struct ifnet *ifp = &es->es_if;
	register int i, s;

	/* not yet, if address still unknown */
	if (ifp->if_addrlist == (struct ifaddr *)0)
		return;

	if (!ec_inited)
		return;

	if (ecstat((char *) es->es_addr))
		return;

	/*
	 * Hang receive buffers and start any pending writes.
	 * Writing into the rcr also makes sure the memory
	 * is turned on.
	 */
	if ((ifp->if_flags & IFF_RUNNING) == 0) {
		s = splimp();
		/*
		 * Arm the receiver
		 */
		for (i = ECRHBF; i >= ECRLBF; i--) {
			ecrwio(unit, i, ECREAD, es->es_buf[i], es->es_len[i]);
		}
		es->es_oactive = 0;
		es->es_mask = ~0;
		es->es_if.if_flags |= IFF_RUNNING;
		if (es->es_if.if_snd.ifq_head)
			ecstart(unit);
		splx(s);
	}
}

/*
 * Start output on interface.  Get another datagram to send
 * off of the interface queue, and copy it to the interface
 * before starting the output.
 */
ecstart(unit)
{
	register struct ec_softc *es = &ec_softc[unit];
	struct mbuf *m;

	if ((es->es_if.if_flags & IFF_RUNNING) == 0)
		return;
	IF_DEQUEUE(&es->es_if.if_snd, m);
	if (m == 0)
		return;
	es->es_len[ECTBF] = ecput(es->es_buf[ECTBF], m);
	es->es_oactive = 1;
	ecrwio(unit, ECTBF, ECWRITE, es->es_buf[ECTBF], es->es_len[ECTBF]);
}

/*
 * Ethernet interface transmitter interrupt.
 * Start another output if more data to send.
 */
ecxint(unit)
	int unit;
{
	register struct ec_softc *es = &ec_softc[unit];

	if (es->es_oactive == 0)
		return;
	es->es_if.if_opackets++;
	es->es_oactive = 0;
	es->es_mask = ~0;
	if (es->es_if.if_snd.ifq_head)
		ecstart(unit);
}

/*
 * On ethernet interface receiver interrupt.
 * If input error just drop packet.
 * Otherwise examine 
 * packet to determine type.  If can't determine length
 * from type, then have to drop packet.  Othewise decapsulate
 * packet based on type and pass to type specific higher-level
 * input routine.
 */
ecread(unit, rbuf, pktlen)
	int unit;
	int rbuf;
	int pktlen;
{
	register struct ec_softc *es = &ec_softc[unit];
	register struct ether_header *ec;
    	struct mbuf *m;
	int len, off, resid, ecoff;
	register struct ifqueue *inq;
	u_char *ecbuf;

	es->es_if.if_ipackets++;
	if (rbuf < ECRLBF || rbuf > ECRHBF)
		panic("ecrint");
	ecbuf = es->es_buf[rbuf];

	/*
	 * Get input data length.
	 * Get pointer to ethernet header (in input buffer).
	 * Deal with trailer protocol: if type is trailer type
	 * get true type from first 16-bit word past data.
	 * Remember that type was trailer by setting off.
	 */
	len = pktlen;
	ec = (struct ether_header *)(ecbuf);
	ec->ether_type = ntohs((u_short)ec->ether_type);
#define	ecdataaddr(ec, off, type)	((type)(((caddr_t)((ec)+1)+(off))))
	if (ec->ether_type >= ETHERTYPE_TRAIL &&
	    ec->ether_type < ETHERTYPE_TRAIL+ETHERTYPE_NTRAILER) {
		off = (ec->ether_type - ETHERTYPE_TRAIL) * 512;
		if (off >= ETHERMTU)
			goto setup;		/* sanity */
		ec->ether_type = ntohs(*ecdataaddr(ec, off, u_short *));
		resid = ntohs(*(ecdataaddr(ec, off+2, u_short *)));
		if (off + resid > len)
			goto setup;		/* sanity */
		len = off + resid;
	} else
		off = 0;
	if (len == 0)
		goto setup;

	/*
	 * Pull packet off interface.  Off is nonzero if packet
	 * has trailing header; ecget will then force this header
	 * information to be at the front, but we still have to drop
	 * the type and length which are at the front of any trailer data.
	 */
	m = ecget(ecbuf, len, off, &es->es_if);
	if (m == 0)
		goto setup;
	if (off) {
		struct ifnet *ifp;

		ifp = *(mtod(m, struct ifnet **));
		m->m_off += 2 * sizeof (u_short);
		m->m_len -= 2 * sizeof (u_short);
		*(mtod(m, struct ifnet **)) = ifp;
	}
	switch (ec->ether_type) {

#ifdef INET
	case ETHERTYPE_IP:
		schednetisr(NETISR_IP);
		inq = &ipintrq;
		break;

	case ETHERTYPE_ARP:
		arpinput(&es->es_ac, m);
		goto setup;
#endif
#ifdef NS
	case ETHERTYPE_NS:
		schednetisr(NETISR_NS);
		inq = &nsintrq;
		break;

#endif
	default:
		m_freem(m);
		goto setup;
	}

	if (IF_QFULL(inq)) {
		IF_DROP(inq);
		m_freem(m);
		goto setup;
	}
	IF_ENQUEUE(inq, m);

setup:
	/*
	 * Reset for next packet.
	 */
	ecrwio(unit, rbuf, ECREAD, es->es_buf[rbuf], es->es_len[rbuf]);
}

/*
 * Ethernet output routine.
 * Encapsulate a packet of type family for the local net.
 * Use trailer local net encapsulation if enough data in first
 * packet leaves a multiple of 512 bytes of data in remainder.
 * If destination is this address or broadcast, send packet to
 * loop device to kludge around the fact that 3com interfaces can't
 * talk to themselves.
 */
ecoutput(ifp, m0, dst)
	struct ifnet *ifp;
	struct mbuf *m0;
	struct sockaddr *dst;
{
	int type, s, error;
 	u_char edst[6];
	struct in_addr idst;
	register struct ec_softc *es = &ec_softc[ifp->if_unit];
	register struct mbuf *m = m0;
	register struct ether_header *ec;
	register int off;
	struct mbuf *mcopy = (struct mbuf *)0;
	int usetrailers;

	if ((ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING)) {
		error = ENETDOWN;
		goto bad;
	}
	switch (dst->sa_family) {

#ifdef INET
	case AF_INET:
		idst = ((struct sockaddr_in *)dst)->sin_addr;
		if (!arpresolve(&es->es_ac, m, &idst, edst, &usetrailers))
			return (0);	/* if not yet resolved */
		if (!bcmp((caddr_t)edst, (caddr_t)etherbroadcastaddr,
		    sizeof(edst)))
			mcopy = m_copy(m, 0, (int)M_COPYALL);
		else if (!bcmp((caddr_t)edst, (caddr_t)es->es_addr,
			sizeof(edst)))
				return(looutput(&loif, m, dst));
		off = ntohs((u_short)mtod(m, struct ip *)->ip_len) - m->m_len;
		/* need per host negotiation */
		if (usetrailers && off > 0 && (off & 0x1ff) == 0 &&
		    m->m_off >= MMINOFF + 2 * sizeof (u_short)) {
			type = ETHERTYPE_TRAIL + (off>>9);
			m->m_off -= 2 * sizeof (u_short);
			m->m_len += 2 * sizeof (u_short);
			*mtod(m, u_short *) = ntohs((u_short)ETHERTYPE_IP);
			*(mtod(m, u_short *) + 1) = ntohs((u_short)m->m_len);
			goto gottrailertype;
		}
		type = ETHERTYPE_IP;
		off = 0;
		goto gottype;
#endif
#ifdef NS
	case AF_NS:
 		bcopy((caddr_t)&(((struct sockaddr_ns *)dst)->sns_addr.x_host),
		    (caddr_t)edst, sizeof (edst));

		if (!bcmp((caddr_t)edst, (caddr_t)&ns_broadhost,
			sizeof(edst))) {

				mcopy = m_copy(m, 0, (int)M_COPYALL);
		} else if (!bcmp((caddr_t)edst, (caddr_t)&ns_thishost,
			sizeof(edst))) {

				return(looutput(&loif, m, dst));
		}
		type = ETHERTYPE_NS;
		off = 0;
		goto gottype;
#endif

	case AF_UNSPEC:
		ec = (struct ether_header *)dst->sa_data;
 		bcopy((caddr_t)ec->ether_dhost, (caddr_t)edst, sizeof (edst));
		type = ec->ether_type;
		goto gottype;

	default:
		printf("ec%d: can't handle af%d\n", ifp->if_unit,
			dst->sa_family);
		error = EAFNOSUPPORT;
		goto bad;
	}

gottrailertype:
	/*
	 * Packet to be sent as trailer: move first packet
	 * (control information) to end of chain.
	 */
	while (m->m_next)
		m = m->m_next;
	m->m_next = m0;
	m = m0->m_next;
	m0->m_next = 0;
	m0 = m;

gottype:
	/*
	 * Add local net header.  If no space in first mbuf,
	 * allocate another.
	 */
	if (m->m_off > MMAXOFF ||
	    MMINOFF + sizeof (struct ether_header) > m->m_off) {
		m = m_get(M_DONTWAIT, MT_HEADER);
		if (m == 0) {
			error = ENOBUFS;
			goto bad;
		}
		m->m_next = m0;
		m->m_off = MMINOFF;
		m->m_len = sizeof (struct ether_header);
	} else {
		m->m_off -= sizeof (struct ether_header);
		m->m_len += sizeof (struct ether_header);
	}
	ec = mtod(m, struct ether_header *);
 	bcopy((caddr_t)edst, (caddr_t)ec->ether_dhost, sizeof (edst));
	bcopy((caddr_t)es->es_addr, (caddr_t)ec->ether_shost,
	    sizeof(ec->ether_shost));
	ec->ether_type = htons((u_short)type);

	/*
	 * Queue message on interface, and start output if interface
	 * not yet active.
	 */
	s = splimp();
	if (IF_QFULL(&ifp->if_snd)) {
		IF_DROP(&ifp->if_snd);
		error = ENOBUFS;
		goto qfull;
	}
	IF_ENQUEUE(&ifp->if_snd, m);
	if (es->es_oactive == 0)
		ecstart(ifp->if_unit);
	splx(s);
	return (mcopy ? looutput(&loif, mcopy, dst) : 0);

qfull:
	m0 = m;
	splx(s);
bad:
	m_freem(m0);
	if (mcopy)
		m_freem(mcopy);
	return (error);
}

/*
 * Routine to copy from mbuf chain to transmit
 * buffer in UNIBUS memory.
 * If packet size is less than the minimum legal size,
 * the buffer is expanded.  We probably should zero out the extra
 * bytes for security, but that would slow things down.
 */
ecput(ecbuf, m)
	u_char *ecbuf;
	struct mbuf *m;
{
	register struct mbuf *mp;
	register int len;
	u_char *bp;

	for (len = 0, mp = m; mp; mp = mp->m_next)
		len += mp->m_len;
	if (len < ETHERMIN + sizeof (struct ether_header))
		len = ETHERMIN + sizeof (struct ether_header);
	bp = (u_char *)(ecbuf);
	for (mp = m; mp; mp = mp->m_next) {
		if (mp->m_len == 0)
			continue;
		bcopy (mtod(mp, caddr_t), (caddr_t)bp, mp->m_len);
		bp += mp->m_len;
	}
	m_freem(m);
	return(len);
}

/*
 * Routine to copy from UNIBUS memory into mbufs.
 * Similar in spirit to if_rubaget.
 *
 * Warning: This makes the fairly safe assumption that
 * mbufs have even lengths.
 */
struct mbuf *
ecget(ecbuf, totlen, off0, ifp)
	u_char *ecbuf;
	int totlen, off0;
	struct ifnet *ifp;
{
	register struct mbuf *m;
	struct mbuf *top = 0, **mp = &top;
	register int off = off0, len;
	u_char *cp;

	cp = ecbuf + sizeof (struct ether_header);
	while (totlen > 0) {
		u_char *mcp;

		MGET(m, M_DONTWAIT, MT_DATA);
		if (m == 0)
			goto bad;
		if (off) {
			len = totlen - off;
			cp = ecbuf + sizeof (struct ether_header) + off;
		} else
			len = totlen;
		if (ifp)
			len += sizeof(ifp);
		if (len >= MLEN) {
			MCLGET(m);
			if (m->m_len == CLBYTES)
				m->m_len = len = MIN(len, CLBYTES);
			else
				m->m_len = len = MIN(MLEN, len);
		} else {
			m->m_len = len = MIN(MLEN, len);
			m->m_off = MMINOFF;
		}
		mcp = mtod(m, u_char *);
		if (ifp) {
			/*
			 * Prepend interface pointer to first mbuf.
			 */
			*(mtod(m, struct ifnet **)) = ifp;
			mcp += sizeof(ifp);
			len -= sizeof(ifp);
			ifp = (struct ifnet *)0;
		}
		bcopy((caddr_t) cp, (caddr_t) mcp, len);
		cp += len;
		*mp = m;
		mp = &m->m_next;
		if (off == 0) {
			totlen -= len;
			continue;
		}
		off += len;
		if (off == totlen) {
			cp = ecbuf + sizeof (struct ether_header);
			off = 0;
			totlen = off0;
		}
	}
	return (top);
bad:
	m_freem(top);
	return (0);
}

/*
 * Process an ioctl request.
 */
ecioctl(ifp, cmd, data)
	register struct ifnet *ifp;
	int cmd;
	caddr_t data;
{
	register struct ifaddr *ifa = (struct ifaddr *)data;
	struct ec_softc *es = &ec_softc[ifp->if_unit];
	int s = splimp(), error = 0;

	switch (cmd) {

	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;

		switch (ifa->ifa_addr.sa_family) {
#ifdef INET
		case AF_INET:
			ecinit(ifp->if_unit);	/* before arpwhohas */
			if ((ifp->if_flags & IFF_RUNNING) == 0) {
				ifp->if_flags &= ~IFF_UP;
				error = EIO;
				break;
			}
			((struct arpcom *)ifp)->ac_ipaddr =
				IA_SIN(ifa)->sin_addr;
			arpwhohas((struct arpcom *)ifp, &IA_SIN(ifa)->sin_addr);
			printf("Ethernet address %s\n", 
				ether_sprintf(ec_softc[0].es_addr));
			printf("Internet address %s\n",
				inter_sprintf(&IA_SIN(ifa)->sin_addr));
			bcopy(&IA_SIN(ifa)->sin_addr, &myinetid,
					 sizeof (myinetid));
			break;
#endif
#ifdef NS
		case AF_NS:
		    {
			register struct ns_addr *ina = &(IA_SNS(ifa)->sns_addr);

			if (ns_nullhost(*ina))
				ina->x_host = *(union ns_host *)(es->es_addr);
			else {
				/* 
				 * The manual says we can't change the address 
				 * while the receiver is armed,
				 * so reset everything
				 */
				ifp->if_flags &= ~IFF_RUNNING; 
				bcopy((caddr_t)ina->x_host.c_host,
				    (caddr_t)es->es_addr, sizeof(es->es_addr));
			}
			ecinit(ifp->if_unit); /* does ec_setaddr() */
			break;
		    }
#endif
		default:
			ecinit(ifp->if_unit);
			break;
		}
		break;

	case SIOCSIFFLAGS:
		if ((ifp->if_flags & IFF_UP) == 0 &&
		    ifp->if_flags & IFF_RUNNING) {
			ecdisable(ifp->if_unit);
			ifp->if_flags &= ~IFF_RUNNING;
		} else if (ifp->if_flags & IFF_UP &&
		    (ifp->if_flags & IFF_RUNNING) == 0)
			ecinit(ifp->if_unit);
		break;

	default:
		error = EINVAL;
	}
	splx(s);
	return (error);
}

ecsetbuf(es, i, ecbuf, rcvbufsz)
register struct ec_softc *es;
u_char *ecbuf;
{
	es->es_buf[i] = ecbuf;
	es->es_len[i] = rcvbufsz;
}

getinetid() {
	return(myinetid.s_addr);
}

#endif
#endif /* pm100 */
E 1
