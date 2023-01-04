h64080
s 00014/00000/00120
d D 1.2 90/04/03 17:07:52 root 2 1
c added nfs_tsize & nfs_stsize functions
e
s 00120/00000/00000
d D 1.1 90/03/06 12:28:52 root 1 0
c date and time created 90/03/06 12:28:52 by root
e
u
U
t
T
I 1
#include "sys/types.h"
#include "sys/errno.h"
#include "sys/commpage.h"
#include "sys/iorb.h"


/* network opmon interface */

struct iorb *eciorb, *ecnwiorb;
int ecdix;
extern ec_inited;

netinit()
{
	/* used primarily to force ld to pull in this file */
}

ecmeminit(es, numec, rcvbufsz) 
struct ec_softc *es;
register numec, rcvbufsz;
{
	register i;
	register u_char *ecbuf;

	for ( i = 0; i < COMMPAGE->dixsiz ; i++ ) 
		if ( COMMPAGE->dix[i] == EC )
		{
			ecdix = i;
			eciorb = (struct iorb *) 
				dmastatic((1+numec)* sizeof(struct iorb));
			ecbuf = (u_char *) dmastatic((unsigned) numec * rcvbufsz);
			if (!ecbuf || !eciorb)
				return 1;
			for (i = 0; i < numec ; i++, ecbuf += rcvbufsz) {
				ecsetbuf(es, i, ecbuf, rcvbufsz);
			}
			ec_inited = 1;
			ecnwiorb = &eciorb[numec];
			return 0;
		}
	return 1;
}

ecintr(IORB)
register struct iorb *IORB;
{
	if (IORB->cmd == PC_WRITE)
		ecxint((int) IORB->val);
	else if (IORB->cmd == PC_READ)
		ecread((int) IORB->val, (int) IORB->dev, (int) IORB->cnt);
	else
		panic("ecintr");
}

ecrwio(unit, num, cmd, buf, cnt)
u_char *buf;
u_short cnt;
{
	register struct iorb *IORB;

	IORB = &eciorb[num];
	IORB->err = IORB_MAGIC;
	IORB->cmd = cmd + PC_READ;	/* 0: read, 1: write */
	IORB->buf = (long) buf;
	IORB->cnt = cnt;
	IORB->st = S_GO;
	IORB->dev = num;
	IORB->dix = ecdix;
	IORB->val = unit;
	IORB->f = ecintr;
	pcstrategy(IORB);
}

static
eccmd(opcmd, buf)
char *buf;
{
	register struct iorb *IORB;

	IORB = ecnwiorb;
	IORB->err = IORB_MAGIC;
	IORB->cmd = opcmd;
	IORB->st = S_GO;
	IORB->dix = ecdix;
	pcwait(IORB, sizeof(struct iorb));
	if (buf && opcmd == PC_STAT) {
		swapla(&IORB->val, 2);
		if (IORB->err == E_OK)
			bcopy((caddr_t) &IORB->val, (caddr_t) buf, 6);
	}
	return (IORB->err);
}

ecstat(buf)
char *buf;
{
	return(eccmd(PC_STAT, buf));
}

echwreset()		/* reset controller */
{
	if (!ec_inited)
		return (ENXIO);
	return (eccmd(PC_RST, (char *) 0) ? EIO : 0);
}

ecenable() {
	printf("ecenable called\n");
}

ecdisable() {
	printf("ecdisable called\n");
}

ec_setaddr(physaddr,unit)
u_char *physaddr;
int unit;
{
	printf("ec_setaddr called\n");
}
I 2

/*
 * Returns the prefered transfer size in bytes based on
 * what network interfaces are available.
 */
nfs_tsize()
{
	return (1024);		/* receive size */
}

nfs_stsize()
{
	return (1024);		/* transmit size */
}
E 2
E 1
