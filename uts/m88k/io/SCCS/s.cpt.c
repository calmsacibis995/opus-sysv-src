h06355
s 00340/00000/00000
d D 1.1 90/03/06 12:28:39 root 1 0
c date and time created 90/03/06 12:28:39 by root
e
u
U
t
T
I 1
/*  @(#)cpt.c	1.4 */

#include "sys/machine.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#if pm300
#include "sys/inode.h"
#include "sys/immu.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/sysreg.h"
#endif

/*	m88k, pm108 and pm300 serial numbers are returned intact
 *	pm110 serial numbers are biased by 100,000
 *	pm220 serial numbers are biased by 200,000
 *	pm2x0 serial numbers are biased by 300,000
 */

#if pm100
#include "sys/dev32k.h"

#define WBIT R_WAIT	/* virtual address of write register */
#define STHI (R_STAT+1)	/* virtual address of status register high byte */
#define MACH 0x03	/* bit mask of machine type in status register */
#define O16 0x00	/* if R_STAT & MACH == O16 then board is an OPUS32.16 */
#define O32 0x01	/* if R_STAT & MACH == O32 then board is an OPUS32.32 */

#define CS(val) *(char *)(WBIT+0) = (char)(val)
#define CK(val) *(char *)(WBIT+4) = (char)(val)
#define WD(val) *(char *)(WBIT+8) = (char)(val)
#define RD() (*(unsigned char *)STHI & 0x04)
#endif

#if pm200 || m88k	/* pm200, m88k */

#include "sys/dev32k.h"

#define CS(val) *CNTRL0 = WIDG_CS | (val)
#define CK(val) *CNTRL0 = WIDG_CK | (val)
#define WD(val) *CNTRL0 = WIDG_WD | ((val)^1)
#define RD() ((*SR0 & WIDG_RD)^1)
#define S1() (*SR1)

#endif

#if pm300	/* pm300 (IO mode only) */

#include "sys/cp.h"

#define CS(val) sysregp[CTRLREG_0] = WIDG_CS | (val)
#define CK(val) sysregp[CTRLREG_0] = WIDG_CK | (val)
#define WD(val) sysregp[CTRLREG_0] = WIDG_WD | ((val)^1)
#define RD() ((sysregp[STATREG_0] & WIDG_RD)^1)
#define S1() (sysregp[STATREG_1])

#endif

#define READ 0x18	/* read word opcode */

static int opened = 0;
static int bias = 0;
#if pm100 || pm200
static int type = 0;
#endif

struct cpt {
	int	method;
	long 	*key;
	long 	keysize;
	long 	*buf;
	long	bufsize;
	};

void wd(), ck(), cs(), send(), send1();
unsigned char getbyte();
long getlong();

cptopen()
{
	if ( opened )
		u.u_error = EBUSY;
	opened = 1;
}

cptclose()
{
	opened = 0;
}

cptioctl(dev, cmd, arg, mode)
{
	struct cpt cpt;
	int key, i, buf;

	if (copyin((char *) arg, (char *) &cpt, sizeof ( struct cpt )))
	{
		u.u_error = EFAULT;
		return;
	}

	switch ( cpt.method )
	{
		case 0:
			cdlock(u.u_procp);
			break;

		case 1:
			cdlock(u.u_procp);
			buf = getit();
			if ( u.u_error )
				break;
			if (copyout(&buf, cpt.buf, sizeof (long))) {
				u.u_error = EFAULT;
				return;
			}
			break;
			
		case 2:
			key = 0;
			goto case2or3;

		case 3:
			key = getit();
			if ( u.u_error )
				break;

		case2or3:
			cdlock(u.u_procp);
			for (i=0; i<cpt.keysize; ++i) {
				if (copyin(&cpt.key[i], (char *)&buf, 4)) {
					u.u_error = EFAULT;
					return;
				}
				key ^= buf;
			}
			for (i=0; i<cpt.bufsize; ++i) {
				if (copyin(&cpt.buf[i], (char *)&buf, 4)) {
					u.u_error = EFAULT;
					return;
				}
				buf ^= key;
				if (copyout(&buf, &cpt.buf[i], 4)) {
					u.u_error = EFAULT;
					return;
				}
				key = (key << 1) | ((key<0) ? 1 : 0);
			}
			break;
		
		case 4:
			cdlock(u.u_procp);
			if (copyin(cpt.buf, (char *)&buf, 4)) {
				u.u_error = EFAULT;
				return;
			}
			delay(2*HZ);
			if (buf != getit()) {
				psignal(u.u_procp, SIGKILL);
			}
			break;

		case 5:
			cdlock(u.u_procp);
			buf = getit();
			if ( u.u_error )
				break;
			if (copyout(&buf, cpt.buf, sizeof(long))) {
				u.u_error = EFAULT;
				return;
			}
			for (i=1; i<cpt.bufsize; ++i) {
				buf = getlong(i * sizeof(long));
				if (copyout(&buf, &cpt.buf[i], sizeof(long))) {
					u.u_error = EFAULT;
					return;
				}
			}
			break;
			
		default:
			u.u_error = EINVAL;
			break;
	}
}

static long
getit()
{
	register val;
	unsigned char b1, b2, csum;

#if pm100
	type = (*(char *)R_STAT) & MACH;
	if( type == O32) {
		cs(0);
		ck(0);
		wd(0);
		bias = 100000;	/* 32032 */
	}
#else
	cs(0);
	ck(0);
	wd(0);
#if pm200
	type = S1() & CP_TYPE;
	if (type == 1)
		bias = 200000;	/* 32332 */
	else if (type == 2)
		bias = 300000;	/* 32532 */
#endif
#endif

	b1 = getbyte(2);
	b2 = getbyte(3);
	csum = getbyte(1);
#if pm100
	if ((b1 == 0xb2) && (b2 == 0xb2) && (csum == 0xb2))
		return(0xb2b2);
#endif
	if ( csum == ( b1 ^ b2 ^ 0xff ) )
		val = ((b2 << 8) | b1) + bias;
	else {
		u.u_error = EIO;
		val = bias;
	}
	return(val);
}

static long
getlong(addr)
{
	return (long)getbyte(addr)
		+ (getbyte(addr+1) << 8)
		+ (getbyte(addr+2) << 16)
		+ (getbyte(addr+3) << 24);
}

static unsigned char
getbyte(addr)
int addr;
{
	int data = 0;

#if pm100
	if (type == O16) {
		if (addr & 1) {	/* odd byte */
			data = *(char *)(R_WAIT + (addr & 6));
		} else {
			data = *(long *)(R_WAIT + (addr & 4));
			if (addr & 2) {
				data >>= 16;
			}
		}
		return(data & 0xff);
	}
#endif

	data = xread(addr>>1);
	if (addr & 1)
		data >>= 8;

	return(data & 0xff);
}

static int
xread(addr)
int addr;
{
	register i, data;

	data = 0;
	cs(1);
	delay(HZ/15);
	send(READ, 5);
	send(addr, 4);
	for (i=15; i>=0; --i) {
		ck(1);
		if (RD())
			data |= 1<<i;
		ck(0);
	}
	wd(0);
	cs(0);
	return(data);
}

static void
send(val, n)
int val, n;
{
	while(n--) {
		send1(val >> n);
	}
}

static void
send1(val)
int val;
{
	wd(val & 1);
	ck(1);
	ck(0);
}

static void
cs(val)
int val;
{
	CS(val);
	send1(0);
}

/*	wd() and ck() are implemented as procedures to slow things down a bit
 */
static void
wd(val)
int val;
{
	WD(val);
}

static void
ck(val)
int val;
{
	CK(val);
}

#if m88k
cptgetid()
{
	return(getit());
}
#endif
E 1
