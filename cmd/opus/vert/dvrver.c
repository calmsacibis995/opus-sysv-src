/*  @(#)dvrver.c	1.1 */

/*
 * Verticom M-16,  M-256, H-16, and H-256 driver
 */

#include <dos.h>
#include "sys.h"
#include "mon.h"

#define debugf echof

extern ushort cfgargs;
extern long cfgarg[];

short ct_ver();
void sv_ver();
void cmd_ver();

#define	UNIT 14			/* the 'gn' unix driver's default unit number */
#define HCMDBUFSEG 0xc600	/* command buffer seg/offset (h series) */
#define HCBUFLEN 0xf00		/* max len of command buffer (h series) */
#define HRCMD 0xfb0		/* read command port  (h series) */
#define HWCMD 0xfbf		/* write command port  (h series) */
#define CMDBUFSEG 0xbc00	/* command buffer seg/offset */
#define CBUFLEN 192		/* max len of command buffer */
#define RCMD 0x3da		/* read command port */
#define WCMD 0x3d8		/* write command port */
#define CRTSEG 0x40		/* segment of bios crt mode */
#define CRTOFFS 0x65		/* offset of bios crt mode */
#define GCBUSY 0x10		/* busy bit */
#define GCRDY 0x40		/* command ready bit */

#define MIN(a, b)	(((a) < (b)) ? (a) : (b))
#define MAX(a, b)	(((a) < (b)) ? (b) : (a))

/* command processor states */
#define ST_NONE	-1		/* not a legal state */
#define ST_INIT	0		/* initialize command pointers */
#define ST_EXE	1		/* start executing a command */
#define ST_NBSY	2		/* waiting hardware busy indication */
#define ST_BSY	3		/* hardware busy executing command */
#define ST_DONE	4		/* hardware finished executing command */
#define ST_END	5		/* iorb finished */

static char *crtmode;		/* pointer to bios crt mode byte */
static ushort state = ST_INIT;	/* current state of command processor */
#ifdef DEBUG
static ushort oldstate = ST_NONE;/* previous command processor state */
#endif
static ushort unit = UNIT;	/* gn unit number */
static uchar *cmdptr;		/* pointer to next command */
static uchar *cmdend;		/* end of command string */
static uchar *rcmd;		/* read control port (h series) */
static uchar *wcmd;		/* write control port (h series) */
static uchar hmode;		/* 0 for m series, 1 for h series interface */
static uchar *cbuf;		/* controller buffer */
static ushort cbuflen;		/* actual buffer len */
static ushort writelen;		/* amount of data to write */
static ushort readlen;		/* amount of data to read */
static uchar executing;		/* executing out of side buffer */
static uchar *abuf;		/* pointer to side buffer */
static ushort abuflen;		/* length of side buffer */
static ushort cmditt;		/* number of commands done since resched */
static ushort throttle = 5;	/* number of commands to do before resched */

static struct iorb *rqb_ver[4];		/* request queue */
static struct pq rq_ver = {0,0,4,rqb_ver};

struct dev dt_ver = {GEN, 0, NULL, NULL, &rq_ver, NULL,
		    ct_ver, sv_ver};

/*
 *	system controls
 */
static short
ct_ver(cmd, dt, arg)
short cmd;
struct dev *dt;
char *arg;
{
	ushort val;
	short code;
	static struct key keys[] = {
		{ "h", KEY_KEY|1 },
		{ "m", KEY_KEY|2 },
		{"buf", KEY_SHORT|3},
		{"throttle", KEY_SHORT|4},
		{ NULL, KEY_END }
	};

	switch (cmd) {

	case CT_ARG:
		if (cfgargs) {
			unit = cfgarg[0];	/* unit # override */
		}
		while (code = ga_keys(keys, &val)) switch (code) {

		case 1:
			hmode = 1;
			break;

		case 2:
			hmode = 0;
			break;

		case 3:
			abuf = (uchar *)malloc(val);
			if (abuf != NULL) {
				abuflen = val;
			}
			break;

		case 4:
			throttle = val;
			break;

		default:
			return(1);
		}

		if (hmode) {
			rcmd = makeptr(HRCMD, HCMDBUFSEG);
			wcmd = makeptr(HWCMD, HCMDBUFSEG);
			cbuf = makeptr(0, HCMDBUFSEG);
			cbuflen = HCBUFLEN;
		} else {
			crtmode = makeptr(CRTOFFS, CRTSEG);
			cbuf = makeptr(0, CMDBUFSEG);
			cbuflen = CBUFLEN;
		}
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
sv_ver(dt)
struct dev *dt;
{
	struct iorb *iorb;
	ushort cmd;

	if (executing || dt_ver.act || (iorb = qgp(dt_ver.rqq)) == NULL) {
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
		dt_ver.act = iorb;
		cmd_ver(dt, cmd);
		return;

	case PC_IOCTL:  /* ioctl */
		break;

	default:
		iorb->err = E_CMD;
	}

	iodone(iorb);
	if (rq_ver.get != rq_ver.put) {
		SCHED(svq, sv_ver, dt);
	}
}


/*
 * Execute a buffer of commands.
 */
static void
cmd_ver(dt, cmd)
struct dev *dt;
short cmd;
{
	struct iorb *iorb;
	ushort len;

	if ((iorb = dt_ver.act) == NULL  && !executing) {
		return;
	}

	if (!executing) {
		curiorb = iorb;
	}

	while (1) {
#ifdef DEBUG
		if (state != oldstate) {
			oldstate = state;
			debugf("new state %d\r\n", state);
		}
#endif
		switch (state) {

		case ST_INIT:
			if (!iorb->cnt) {
				state = ST_END;
				break;
			}
			if (cmd == PC_WRITE && iorb->cnt <= abuflen) {
				bcopy(iorb->addr, cpseg, abuf, iorb->cnt);
				cmdptr = abuf;
				executing = 1;
				dt_ver.act = NULL;
				iodone(iorb);
			} else {
				cmdptr = makeptr(iorb->addr, cpseg);
			}
			cmdend = &cmdptr[iorb->cnt];
			cmditt = 0;
			state = ST_EXE;
			break;

		case ST_EXE:
			if (checkcmd()) {
				iorb->err = E_MA;
				state = ST_END;
				break;
			}
#ifdef DEBUG
			debugf("start %x %x %x %x %x\r\n", writelen, cmdptr[0],
					cmdptr[1], cmdptr[2], cmdptr[3]);
#endif
			bcopy(cmdptr, cbuf, writelen);
			if (hmode) {
				*wcmd |= GCRDY;
			} else {
				outp(WCMD, *crtmode | GCRDY);
			}
			state = ST_NBSY;
			break;

		case ST_NBSY:
			if (hmode) {
				if ((*rcmd & GCBUSY) == 0) {
					curiorb = NULL;
					SCHED(svq, cmd_ver, dt);
					return;
				} else {
					*wcmd &= ~GCRDY;
					state = ST_BSY;
				}
			} else {
				if ((inp(RCMD) & GCBUSY) == 0) {
					curiorb = NULL;
					SCHED(svq, cmd_ver, dt);
					return;
				} else {
					outp(WCMD, *crtmode & ~GCRDY);
					state = ST_BSY;
				}
			}
			break;

		case ST_BSY:
			if (hmode) {
				if (*rcmd & GCBUSY) {
					curiorb = NULL;
					SCHED(svq, cmd_ver, dt);
					return;
				} else {
					state = ST_DONE;
				}
			} else {
				if (inp(RCMD) & GCBUSY) {
					curiorb = NULL;
					SCHED(svq, cmd_ver, dt);
					return;
				} else {
					state = ST_DONE;
				}
			}
			break;

		case ST_DONE:
#ifdef DEBUG
			debugf("readback %d %d\r\n", readlen, executing);
			debugf("%x %x %x %x\r\n", cbuf[0], cbuf[1],
						  cbuf[2], cbuf[3]);
#endif
			if (readlen && !executing) {
				bcopy(cbuf, cmdptr, readlen);
			}
			cmdptr += MAX(readlen, writelen);
			if (cmdptr >= cmdend) {
				state = ST_END;
			} else {
				state = ST_EXE;
				if (++cmditt > throttle) {
					cmditt = 0;
					curiorb = NULL;
					SCHED(svq, cmd_ver, dt);
					return;
				}
			}
			break;

		case ST_END:
			state = ST_INIT;
			if (!executing) {
				dt_ver.act = NULL;
				iodone(iorb);
			} else {
				executing = 0;
			}
			if (rq_ver.get != rq_ver.put) {
				SCHED(svq, sv_ver, dt);
			}
			return;
		}
	}
}

/*
 * Set <readlen> and <writelen> according to the current command which is
 * pointed to by <cmdptr>.
 */
static void
checkcmd()
{

	readlen = 0;
	switch (*cmdptr) {

	case 0x20:
		writelen = cmdptr[1] + 1;
		readlen = cmdptr[1] + 1;
		break;

	case 0x40:
	case 0x41:
	case 0x85:
		writelen = 1;
		break;

	case 0x31:
	case 0x42:
	case 0x44:
	case 0x46:
	case 0x47:
	case 0x48:
	case 0x86:
	case 0x9d:
	case 0x9f:
		writelen = 2;
		break;

	case 0x4a:
	case 0x73:
	case 0x75:
	case 0x94:
	case 0xa0:
	case 0x9c:
	case 0x9e:
		writelen = 3;
		break;

	case 0x98:
		writelen = 4;
		break;

	case 0x60:
	case 0x61:
	case 0x62:
	case 0x63:
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x67:
	case 0x68:
	case 0x69:
	case 0x84:
		writelen = 5;
		break;

	case 0x7f:
	case 0x96:
	case 0x9a:
	case 0xba:
		writelen = 6;
		break;

	case 0x70:
	case 0x71:
	case 0x76:
	case 0x77:
	case 0x78:
	case 0x79:
		writelen = 7;
		break;

	case 0x90:
	case 0x92:
		writelen = 8;
		break;

	case 0x50:
	case 0x51:
	case 0x6e:
	case 0x6f:
	case 0x80:
		writelen = 9;
		break;

	case 0x91:
		writelen = 13;
		break;

	case 0x83:
		writelen = 16;
		break;

	case 0xb0:
		readlen = 15;
		writelen = 1;
		break;

	case 0x6a:
	case 0x6b:
	case 0x6c:
	case 0x6d:
		writelen = (cmdptr[1] * 4) + 2;
		break;

	case 0x7c:
	case 0x7d:
	case 0x82:
		writelen = cmdptr[1] + 2;
		break;

	case 0x95:
		writelen = (((cmdptr[1] + 7) >> 3) * cmdptr[2]) + 3;
		break;
	
	case 0x97:
		writelen = ((cmdptr[1] + 7) >> 3) + 3;
		break;

	case 0x99:
		writelen = (((cmdptr[3] + 7) >> 3) * cmdptr[4]) + 4;
		break;

	case 0x9b:
		writelen = cmdptr[2] + 4;
		break;

	case 0x7e:
		readlen = 2;
		writelen = 5;
		break;

	case 0xb1:
		readlen = 7;
		writelen = 2;
		break;

	case 0xb2:
		readlen = 8;
		writelen = 1;
		break;

	case 0xb3:
		readlen = 5;
		writelen = 1;
		break;
	
	case 0xb4:
		readlen = 4;
		writelen = 1;
		break;

	case 0xb5:
		readlen = 6;
		writelen = 1;
		break;

	case 0xb6:
		readlen = 13;
		writelen = 1;
		break;

	case 0xb7:
	case 0xb8:
		readlen = 5;
		writelen = 1;
		break;

	case 0xb9:
		readlen = 2;
		writelen = 5;
		break;

	case 0x81:
		readlen = cmdptr[1] + 2;
		writelen = 2;
		break;

	case 0xff:
		writelen = cmdptr[1] + (cmdptr[2] << 8);
		readlen = cmdptr[3] + (cmdptr[4] << 8);
		cmdptr += 5;
		break;

	default:
		return(-1);
		break;
	}
	return(0);
}
