
#include "param.h"
#include "var.h"
#include "inode.h"
#include "fstyp.h"
#include "conf.h"
#include "sysmacros.h"
#include "sockinode.h"
#include "../net/netisr.h"
#include "time.h"
#include "acct.h"

#include "dir.h"
#include "user.h"
#include "proc.h"
#include "map.h"
#include "mount.h"
#undef MFREE
#include "mbuf.h"

#include "kernel.h"
#if ns32000
#include "debug.h"
#include "pfdat.h"
#endif
#if m88k
#define	memlock()
#define memunlock()
#endif

struct xinode *xinode;
struct inode *fkinode;
int sockfstyp;
extern physmem;
#define	CLSPGS	(physmem/16)
struct map mbmap[10];
struct mount fkmount;
struct mbuf *mbutl;

sockinit()
{
	int s;
	register n;
	struct mbuf *m;

	s = splimp();

	mapinit(mbmap, 10);
	mfree(mbmap, CLSPGS, 1);

	n = malloc(sptmap, CLSPGS);
	if (n == 0)
		goto bad;
	mbutl = (struct mbuf *) ctob(n);

	mbinit();

	/* This code counts on the way m_clalloc works in uipc_mbuf.c */
	n = (((v.v_inode*sizeof(struct xinode))+(CLBYTES-1))/CLBYTES);
	while (n--) {
		MGET(m, M_DONTWAIT, MT_DATA);
		MCLGET(m);
	}
	xinode = mtod(m, struct xinode *);
	if (!xinode)
		goto bad;

	/*
	 * set sockfstyp as the index into FSS table for socket entries
	 */
	sockfstyp = 0;
	for (n = 0; n < nfstyp; n++) {
		if (fstypsw[n].fs_init == sockinit) {
			sockfstyp = n;
			break;
		}
	}
	if (sockfstyp == 0 || sockfstyp == nfstyp)
		panic("sockinit: nfsinit not found in fstypsw\n");
	m = m_getclr(M_DONTWAIT, MT_DATA);
	if (m) {
		fkinode = mtod(m, struct inode *);
		fkinode->i_ftype = IFMPB;
		fkinode->i_fstyp = sockfstyp;
		fkinode->i_mntdev = &fkmount;
#ifdef FSPTR
		fkinode->i_fstypp = &fstypsw[sockfstyp];
#endif
	}
	else
		goto bad;

	loattach();
	ecattach();
	ifinit();
	domaininit();

	splx(s);
	return;
bad:
	panic("sockinit");
}

/*
 * Must be called at splimp.
 */
caddr_t
mbufmem(npg, canwait)
register int npg;
{
	register int i;
	register caddr_t m;
	extern freemem;

	i = malloc(mbmap, (long)npg);
	if (i == 0)
		panic("mbufmem: map full");
	m = (caddr_t) ((int)mbutl + ctob(i));

	/* There is a flaw in the "memreserve" logic, when size is 1.
	   This code can run at interrupt level and cannot sleep.
	*/
	if (freemem == 0) 
		panic("m_clalloc: freemem");

	reglock(&sysreg);
	memlock();
#if m88k
	if (ptmemall(&sysreg, kvtokptbl(m), npg, 1))
#endif
#if clipper
	if (ptmemall(&sysreg, kvtokptbl(m),
		npg, PM_RW, canwait == M_DONTWAIT ? NOSLEEP : 0))
#endif
#if ns32000
	if (ptmemall(&sysreg, kvtokptbl(m),
		npg, 1, canwait == M_DONTWAIT ? NOSLEEP : 0))
#endif
	{
		memunlock();
		regrele(&sysreg);
		mfree(mbmap,npg,i);
		return(NULL);
	}
	memunlock();
	regrele(&sysreg);

#if ns32000
	/* need to set protection bits in the pte */
	for (i = 0; i < npg; i++) {
		pde_t *pd;
		pd = kvtokptbl(m + ctob(i));
		pg_setprot(pd, PTE_KW);
	}
#endif
#if notdef
#if m88k
	/* need to set cache inhibit bits in the pte */
	for (i = 0; i < npg; i++) {
		register pde_t *pd;
		pd = kvtokptbl(m + ctob(i));
		pg_setci(pd);
	}
#endif
#endif
	return(m);
}

struct inode *
mkfkinode(so)
struct socket *so;
{
	register struct mbuf *mi;
	register struct inode *ip;

	mi = m_getclr(M_WAIT, MT_DATA);
	if (mi) {
		ip = mtod(mi, struct inode *);
		*ip = *fkinode;
		ip->i_fsptr = (int *) so;
		ip->i_count = 1;
		return (ip);
	}
	else
		return(0);
}

/* 
 * vax emulation routines
 */

struct vaxq 
{
	struct vaxq *flink;
	struct vaxq *blink;
};

insque(entry, pred)
register struct vaxq *entry, *pred;
{

	entry->flink = pred->flink;
	entry->blink = pred;
	if (pred->flink) 
		pred->flink->blink = entry;
	pred->flink = entry;
}

remque(entry)
register struct vaxq *entry;
{

	if (entry->blink)
		 entry->blink->flink = entry->flink;
	if (entry->flink)
		 entry->flink->blink = entry->blink;
}

#define	NETVEC(x)	(1 << (x))
#define	PNET		10

int running_softint;

softnetint()
{
	register s;

	s = spl7();
	if (running_softint) {
		splx(s);
		return;
	}
	running_softint = 1;

	while ( netisr ) {
		if (netisr & (~(NETVEC(NETISR_IP)|NETVEC(NETISR_RAW))))
			panic("netisr\n");

		if (netisr & NETVEC(NETISR_IP)) {
			netisr &= ~NETVEC(NETISR_IP);
			ipintr();
		} 
		if (netisr & NETVEC(NETISR_RAW)) {
			netisr &= ~NETVEC(NETISR_RAW);
			rawintr();
		} 
	}
	running_softint = 0;
	splx(s);
}

/*
 * Test if the current user is a super user for network purposes.
 * XXX implementation specific.
 */
netsuser(port)
{

	if (u.u_uid == 0 || u.u_ruid == 0)
		return(1);
	return ((port == 21) || (port == 20)) ;	/* XXX ftp port */
}

netreset()
{
	if (!suser())
		return;
	u.u_error = echwreset();
}
