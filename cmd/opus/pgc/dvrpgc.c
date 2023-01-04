/*  @(#)dvrpgc.c	1.1 */

/*
 * IBM Professional Graphics Adapter driver.
 */

#include <dos.h>
#include "sys.h"
#include "mon.h"

extern ushort cfgargs;
extern long cfgarg[];

short ct_pgc();
void sv_pgc();
void cmd_pgc();

#define	UNIT 14			/* the 'gn' unix driver's default unit number */

#define MIN(a, b)	(((a) < (b)) ? (a) : (b))
#define MAX(a, b)	(((a) < (b)) ? (b) : (a))

/* driver states */

#define CMDNONE		0	/* no command in progress */
#define CMDWRT		1	/* currently writing cmd fifo from unix buffer */
#define CMDWRTS		2	/* currently writing cmd fifo from side buffer */
#define CMDRD		3	/* currently reading read fifo */
#define CMDERR		4	/* currently reading error fifo */
#define CMDCOLD		5	/* doing coldstart */
#define CMDWARM		6	/* doing warmstart */
#define CMDDRAIN	7	/* waiting for write fifo to drain */

/* memory map of controller command area */
struct pgc {
	uchar cmdfifo[256];
	uchar readfifo[256];
	uchar errfifo[256];
	uchar c_head;
	uchar c_tail;
	uchar r_head;
	uchar r_tail;
	uchar e_head;
	uchar e_tail;
	uchar coldstart;
	uchar warmstart;
	uchar errenable;
};

static ushort unit = UNIT;	/* gn unit number */
static ushort seg = 0xc600;	/* segment address of controller */
static struct pgc *pgc;		/* pointer to command area */
static uchar *bufptr;		/* current point in user buffer */
static ushort buflen;		/* remaining length of user buffer */
static uchar state = CMDNONE;	/* driver state */
static uchar *abuf;			/* pointer to side buffer */
static ushort abuflen;		/* length of side buffer */
static ushort throttle = 1024;	/* number of bytes to write before resched */
static ushort timecnt;		/* write timeout counter */
static ushort timeout = 0;	/* write timeout limit */

static struct iorb *rqb_pgc[4];		/* request queue */
static struct pq rq_pgc = {0,0,4,rqb_pgc};

struct dev dt_pgc = {GEN, 0, NULL, NULL, &rq_pgc, NULL,
		    ct_pgc, sv_pgc};

/*
 *	system controls
 */
static short
ct_pgc(cmd, dt, arg)
short cmd;
struct dev *dt;
char *arg;
{
	ushort val;
	short code;
	static struct key keys[] = {
		{ "seg", KEY_SHORT|1 },
		{"buf", KEY_SHORT|2},
		{"throttle", KEY_SHORT|3},
		{"timeout", KEY_SHORT|4},
		{ NULL, KEY_END }
	};

	switch (cmd) {

	case CT_ARG:
		if (cfgargs) {
			unit = cfgarg[0];	/* unit # override */
		}
		while (code = ga_keys(keys, &val)) switch (code) {

		case 1:
			seg = val;
			break;

		case 2:
			abuf = (uchar *)malloc(val);
			if (abuf != NULL) {
				abuflen = val;
			}
			break;

		case 3:
			throttle = MAX(256, val);
			break;

		case 4:
			timeout = val;
			break;

		default:
			return(1);
		}

		pgc = (struct pgc *)makeptr(0, seg);
		break;

	default:
		break;
	}
	return(0);
}

/*
 *	service next request
 */
static void
sv_pgc(dt)
struct dev *dt;
{
	struct iorb *iorb;
	ushort cmd;

	if (state != CMDNONE || dt_pgc.act || (iorb = qgp(dt_pgc.rqq)) == NULL) {
		return;
	}

	curiorb = iorb;
	cmd = iorb->cmd;
	switch (cmd) {

	case PC_NOP:	/* nop */
	case PC_RST:	/* reset */
		break;

	case PC_STAT:	/* status */
		iorb->dst = unit;
		break;

	case PC_WRITE:  /* write */
	case PC_READ:	/* read */
		if (chkac(iorb)) {
			iorb->err = E_MA; 	/* bad address */
			break;
		}
		dt_pgc.act = iorb;
		cmd_pgc(dt);
		return;

	case PC_IOCTL:  /* ioctl */
		if (iorb->blk != 0xffff &&
		    iorb->blk != 0xfffe) {
			dt_pgc.act = iorb;
			cmd_pgc(dt);
			return;
		}
		break;

	default:
		iorb->err = E_CMD;
	}

	iodone(iorb);
	if (rq_pgc.get != rq_pgc.put) {
		SCHED(svq, sv_pgc, dt);
	}
}

/*
 * Execute iorb.
 */
static void
cmd_pgc(dt)
struct dev *dt;
{
	struct iorb *iorb;
	ushort len;

	iorb = dt_pgc.act;

	/* start new command */
	if (state == CMDNONE) {
		if (iorb == NULL) {
			return;
		}
		curiorb = iorb;
		timecnt = 0;
		switch (iorb->cmd) {
		case PC_WRITE:
			if (iorb->cnt <= abuflen) {
				bcopy(iorb->addr, cpseg, abuf, iorb->cnt);
				bufptr = abuf;
				buflen = iorb->cnt;
				dt_pgc.act = NULL;
				iodone(iorb);
				state = CMDWRTS;
				/* give opmon a chanch to complete the iorb */
				curiorb = NULL;
				SCHED(svq, cmd_pgc, dt);
				return;
			} else {
				bufptr = makeptr(iorb->addr, cpseg);
				buflen = iorb->cnt;
				state = CMDWRT;
			}
			break;

		case PC_READ:
			bufptr = makeptr(iorb->addr, cpseg);
			buflen = iorb->cnt;
			state = iorb->blkh != 0 ? CMDERR : CMDRD;
			break;

		case PC_IOCTL:
			switch (iorb->blk) {

			case 0:
				state = CMDDRAIN;
				break;

			case 1:
				state = CMDCOLD;
				pgc->coldstart = 1;
				break;

			case 2:
				state = CMDWARM;
				pgc->warmstart = 1;
				break;

			case 3:
				pgc->errenable = 0;
				break;

			case 4:
				pgc->errenable = 1;
				break;
			}
			break;
		}
	}

	/* continue current command */
	switch (state) {

	case CMDRD:
		len = readfifo(pgc->readfifo, &pgc->r_head, &pgc->r_tail,
					bufptr, buflen);
		iorb->cnt = len;
		goto done;
		break;

	case CMDERR:
		len = readfifo(pgc->errfifo, &pgc->e_head, &pgc->e_tail,
					bufptr, buflen);
		iorb->cnt = len;
		goto done;
		break;

	case CMDWRT:
	case CMDWRTS:
		len = writefifo(pgc->cmdfifo, &pgc->c_head, &pgc->c_tail,
					bufptr, buflen);
		bufptr += len;
		buflen -= len;
		if (buflen <= 0) {
			goto done;
		}
		if (timeout != 0 && (timecnt++) > timeout) {
			echos("dvrpgc: write timeout\r\n");
			iorb->err = E_TO;
			goto done;
		}
		break;

	case CMDCOLD:
		if (pgc->coldstart == 0) {
			goto done;
		}
		break;

	case CMDWARM:
		if (pgc->warmstart == 0) {
			goto done;
		}
		break;

	case CMDDRAIN:
		if (pgc->c_head == pgc->c_tail) {
			goto done;
		}
		break;

	default:
		goto done;
		break;
	}
	if (timeout != 0 && (timecnt++) > timeout) {
		echos("dvrpgc: command timeout\r\n");
		iorb->err = E_TO;
		goto done;
	}
	curiorb = NULL;
	SCHED(svq, cmd_pgc, dt);
	return;

done:
	if (state != CMDWRTS) {
		dt_pgc.act = NULL;
		iodone(iorb);
	}
	state = CMDNONE;
	if (rq_pgc.get != rq_pgc.put) {
		SCHED(svq, sv_pgc, dt);
	}
}

/*
 * Write <len> bytes from <buf> to <fifo> whose control ptrs are
 * <head> and <tail>. Returns cnt of bytes transfered.
 */
int
writefifo(fifo, head, tail, buf, len)
uchar *fifo;
uchar *head;
uchar *tail;
uchar *buf;
ushort len;
{
	int actlen;
	int cnt;
	uchar h, t;

	dbg_pgc("write(%x%x,%x%x,%x)\r\n", fifo, buf, len);
	for (cnt = 0 ; cnt < throttle; cnt += actlen) {
		h = *head;
		t = *tail;
		if (h >= t) {
			actlen = ((t == 0) ? 255 : 256) - h;
		} else {
			actlen = t - h - 1;
		}
		actlen = MIN(actlen, len);
		if (actlen <= 0) {
			break;
		}
		bcopy(buf, &fifo[h], actlen);
		len -= actlen;
		*head += actlen;
		buf += actlen;
	}
	dbg_pgc("write rtn %x\r\n", cnt);
	return(cnt);
}

/*
 * Read up to <len> bytes into <buf> from <fifo> whose control ptrs are
 * <head> and <tail>. Returns count of bytes read.
 */
int
readfifo(fifo, head, tail, buf, len)
uchar *fifo;
uchar *head;
uchar *tail;
uchar *buf;
ushort len;
{
	ushort actlen;
	int cnt = 0;
	uchar h, t;

	dbg_pgc("read(%x%x %x %x %x)\r\n", fifo, *head, *tail, len);
	while (1) {
		h = *head;
		t = *tail;
		if (h < t) {
			actlen = 256 - t;
		} else {
			actlen = h - t;
		}
		actlen = MIN(actlen, len);
		if (actlen <= 0) {
			break;
		}
		bcopy(&fifo[t], buf, actlen);
		len -= actlen;
		*tail += actlen;
		buf += actlen;
		cnt += actlen;
	}
	dbg_pgc("read rtn %x\r\n", cnt);
	return(cnt);
}

/*
 * local debugging
 */
dbg_pgc(fmt, a, b, c, d, e, f)
char *fmt;
int a, b, c, d, e, f;
{

	/*echof(fmt, a, b, c, d, e, f);*/
}
