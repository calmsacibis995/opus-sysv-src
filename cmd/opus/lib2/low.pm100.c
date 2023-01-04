#include "stand.h"
#include "sys/cb.h"
#include "sys/iorb.h"
#include "sys/doscomm.h"
#include "sys/commpage.h"
#include "sys/cp.h"
#include "sys/param.h"
#include "sys/page.h"
#include "sys/immu.h"

#ifndef PAGESIZE
#define PAGESIZE	0x200
#endif

#define REGMAX	25	/* CPU registers plus PC, etc.*/
#define BASE	16	/* Default base for input numbers */

unsigned _r[REGMAX];	/* User register storage */

char *_regname[] = {
    "    R0","    R1","    R2","    R3","    R4","    R5","    R6","    R7",
    "    PC","    SB","    FP","   SP0","INTBAS","SR/MOD","  BPR0","  BPR1",
    "   PF0","   PF1","    SC","   MSR","  BCNT","  PTB0","  PTB1","   EIA"
};

char _hexchar[] = {
	"0123456789abcdef"
};
unsigned short _cbindex = 0;	/* next available command blk */
unsigned short _mapped = 1;	/* we are mapped except at startup */
unsigned long  _memmap;		/* map of available memory */

struct io_gen *_cmd_buf;
struct io_sys *_sys_cb;
struct io_clk *_clk_cb; 
struct io_conin *_conin_cb[CONMAX];
struct io_conout *_conout_cb[CONMAX];
struct io_hdisk *_hdisk_cb[DKMAX];
struct io_fdisk *_fdisk_cb[DKMAX];
struct io_mt *_mt_cb[MTMAX];
struct io_serin *_serin_cb[ASMAX];
struct io_serout *_serout_cb[ASMAX];
struct io_dos *_dos_cb; 
struct io_lpt *_lpt_cb[LPMAX];
struct io_gn *_gn_cb[GNMAX];

struct io_serin *_dupin_cb;
struct io_serout *_dupout_cb;

struct er_block _errptblk;

#define COUNTMAX 10000;

_opus()
{
	static  beenhere = 0;
	unsigned long tmp;
	int countdown;
	int i;
	char *devptr;
	int disknum = 0;
	int newdisknum = 0;
	int mtnum = 0;
	int conin = 0;
	int conout = 0;
	int serin = 0;
	int serout = 0;
	int lpt = 0;
	int gnnum = 0;

	countdown = COUNTMAX;
	if (!beenhere++) {
		PCCMD->pc_devtab[0] = DEVTABSIZE;
		PCCMD->pc_dix = mapcomm(PCCMD->pc_devtab);
		while (PCCMD->pc_devtab[0] == DEVTABSIZE) {
			if (countdown-- == 0) {
				countdown = COUNTMAX;
			}
		}

		_cbindex = PCCMD->pc_devtab[0] + 1;

		_sys_cb = (struct io_sys *)&PCCMD->cmd_buf[0];
		_sys_cb->io_index = 0;

		for (i=1; i<=PCCMD->pc_devtab[0]; i++) {
			switch (PCCMD->pc_devtab[i]) {
			case CLOCK:
				_clk_cb = (struct io_clk *)
					&PCCMD->cmd_buf[i];
				_clk_cb->io_index = i;
				break;
			case CONIN:
				PCCMD->cmd_buf[i].io_index = i;
				_iocmd(&PCCMD->cmd_buf[i],PC_NOP,0);
				if (PCCMD->cmd_buf[i].io_error != E_OK)
					break;
				conin = PCCMD->cmd_buf[i].io_devstat & 0x7f;
				if (conin > CONMAX)
					break;
				_conin_cb[conin] = (struct io_conin *)
					&PCCMD->cmd_buf[i];
				_conin_cb[conin]->io_index = i;
				break;
			case CONOUT:
				PCCMD->cmd_buf[i].io_index = i;
				_iocmd(&PCCMD->cmd_buf[i],PC_NOP,0);
				if (PCCMD->cmd_buf[i].io_error != E_OK)
					break;
				conout = PCCMD->cmd_buf[i].io_devstat & 0x7f;
				if (conout > CONMAX)
					break;
				_conout_cb[conout] = (struct io_conout *)
					&PCCMD->cmd_buf[i];
				_conout_cb[conout]->io_index = i;
				break;
			case DOS:
				_dos_cb = (struct io_dos *)
					&PCCMD->cmd_buf[i];
				_dos_cb->io_index = i;
				break;
			case DISK:
				while (_hdisk_cb[disknum])
					if (++disknum > DKMAX) {
						--disknum;
						break;
					}
				_hdisk_cb[disknum] = (struct io_hdisk *)
					&PCCMD->cmd_buf[i];
				_hdisk_cb[disknum++]->io_index = i;
				_iocmd(_hdisk_cb[disknum-1],PC_STAT,0);
				if (PCCMD->cmd_buf[i].io_error != E_OK)
					break;
				newdisknum = PCCMD->cmd_buf[i].io_devstat & 0xf;
				if (newdisknum == 0)
					break;
				_hdisk_cb[--disknum]->io_index = 0;
				_hdisk_cb[disknum] = 0;
				_hdisk_cb[newdisknum] = (struct io_hdisk *)
					&PCCMD->cmd_buf[i];
				_hdisk_cb[newdisknum]->io_index = i;
				break;
			case TAPE:
				PCCMD->cmd_buf[i].io_index = i;
				_iocmd(&PCCMD->cmd_buf[i],PC_STAT,0);
				if (PCCMD->cmd_buf[i].io_error != E_OK)
					break;
				mtnum = PCCMD->cmd_buf[i].io_devstat & 3;
				if (mtnum > MTMAX)
					break;
				_mt_cb[mtnum] = (struct io_mt *)
					&PCCMD->cmd_buf[i];
				_mt_cb[mtnum]->io_index = i;
				break;
			case SERIN:
				PCCMD->cmd_buf[i].io_index = i;
				_iocmd(&PCCMD->cmd_buf[i],PC_NOP,0);
				if (PCCMD->cmd_buf[i].io_error != E_OK)
					break;
				serin = PCCMD->cmd_buf[i].io_devstat & 0x7f;
				if (serin > ASMAX)
					break;
				_serin_cb[serin] = (struct io_serin *)
					&PCCMD->cmd_buf[i];
				_serin_cb[serin]->io_index = i;
				break;
			case SEROUT:
				PCCMD->cmd_buf[i].io_index = i;
				_iocmd(&PCCMD->cmd_buf[i],PC_NOP,0);
				if (PCCMD->cmd_buf[i].io_error != E_OK)
					break;
				serout = PCCMD->cmd_buf[i].io_devstat & 0x7f;
				if (serout > ASMAX)
					break;
				_serout_cb[serout] = (struct io_serout *)
					&PCCMD->cmd_buf[i];
				_serout_cb[serout]->io_index = i;
				break;
			case LPT:
				PCCMD->cmd_buf[i].io_index = i;
				_iocmd(&PCCMD->cmd_buf[i],PC_NOP,0);
				if (PCCMD->cmd_buf[i].io_error != E_OK)
					break;
				lpt = PCCMD->cmd_buf[i].io_devstat & 0x7;
				if (lpt > LPMAX)
					break;
				_lpt_cb[lpt] = (struct io_lpt *)
					&PCCMD->cmd_buf[i];
				_lpt_cb[lpt]->io_index = i;
				break;
			case GEN:
				PCCMD->cmd_buf[i].io_index = i;
				_iocmd(&PCCMD->cmd_buf[i],PC_STAT,0);
				if (PCCMD->cmd_buf[i].io_error != E_OK)
					break;
				gnnum = PCCMD->cmd_buf[i].io_devstat & 0xf;
				if (gnnum > GNMAX)
					break;
				_gn_cb[gnnum] = (struct io_gn *)
					&PCCMD->cmd_buf[i];
				_gn_cb[gnnum]->io_index = i;
				break;
			}
		}
	}
}
	
_iocmd(io_cmdblk,cmd,arg0)
struct io_gen *io_cmdblk;
unsigned char cmd;
unsigned arg0;
{
	int i;
	unsigned *argptr;

	argptr = &arg0;

	io_cmdblk->io_cmd = cmd;
	for (i=0; i<MAXIOARGS; i++)
		io_cmdblk->io_args[i] = *argptr++;
	_io_request(io_cmdblk);

	return(io_cmdblk->io_error);
}

_io_request(cb_addr)
struct io_gen *cb_addr;
{
	register long countdown;
	struct io_gen *io_req;

	countdown = COUNTMAX;
	cb_addr->io_status = S_GO;
	PCCMD->pc_rqblk = mapcomm(cb_addr);
	PCCMD->pc_rqflg = S_GO;
	IRQPC();
	while (cb_addr->io_status != S_DONE) {
		PCCMD->pc_cpflg = S_IDLE;
		ACKPC();
		if (countdown--) continue;
		countdown = COUNTMAX;
	}
	PCCMD->pc_cpflg = S_IDLE;
	return(0);
}

_pause(exitval) {
	if (exitval >= 0)
		_syscmd(PC_PAUSE,exitval);
	else
		_syscmd(PC_DOSRTN);
}

_syscmd(cmd,arg0,arg1)
unsigned char cmd;
unsigned short arg0,arg1;
{
	_sys_cb->io_index = 0;
	_sys_cb->io_cmd = cmd;
	_sys_cb->io_error = 0;
	_sys_cb->io_status = S_GO;
	_sys_cb->io_memaddr = arg0;
	_sys_cb->io_bcount = arg1;
	return(_sys_request(_sys_cb));
}

_sys_request(sysio)
struct io_sys *sysio;
{
	register long countdown;

	countdown = COUNTMAX;
	PCCMD->pc_syblk = mapcomm(sysio);
	PCCMD->pc_syflg = S_GO;
	IRQPC();
	while (PCCMD->pc_syflg != S_DONE) {
		ACKPC();
		if (countdown--) continue;
		countdown = COUNTMAX;
	}
	PCCMD->pc_syflg = S_IDLE;
	return(sysio->io_memaddr);
}

_putstr(str)
char *str;
{
	short siz;

	for (siz=0; str[siz]; siz++);

	_iocmd(_conout_cb[0],PC_WRITE,_mapin(str,siz),siz);
	if (_dupout_cb)
		_iocmd(_dupout_cb,PC_WRITE,_mapin(str,siz),siz);
}

_putchar(val)
char val;
{
	_iocmd(_conout_cb[0],PC_WRITE,val,0);
	if (_dupout_cb)
		_iocmd(_dupout_cb,PC_WRITE,val,0);

	if (val == 0xa)
		_putchar(0xd);
}

_ngetchar() {
	register unsigned char ch;

	_iocmd(_conin_cb[0],PC_NWREAD,0);

	if (_conin_cb[0]->io_error != E_OK)
		return(-1);

	ch = _conin_cb[0]->io_char;

	if (ch == 0xd) {
		_putchar(ch);
		ch = 0xa;
	}

	if ((ch == 0x13) || (ch == 0x11))
		return(ch);

	_putchar(ch);
	return(ch);
}

_getchar() {
	register unsigned char ch;

	if (_dupin_cb) {
		_iocmd(_dupin_cb,PC_READ,0,0,0,0,0,0,0,0);
		ch = _dupin_cb->io_char;
	} else {
		_iocmd(_conin_cb[0],PC_READ,0,0,0,0,0,0,0,0);
		ch = _conin_cb[0]->io_char;
	}

	if (ch == 0xd) {
		_putchar(ch);
		ch = 0xa;
	}

	if ((ch == 0x13) || (ch == 0x11))
		return(ch);

	_putchar(ch);
	return(ch);
}


_trap(r7,r6,r5,r4,r3,r2,r1,r0,r14,status,trapch)
{
	int i,j;

	switch (trapch) {
	case 'a': _PRINTF("\nABT"); break;
	case 'f': _PRINTF("\nFPU"); break;
	case 'i': _PRINTF("\nILL"); break;
	case 's': _PRINTF("\nSVC"); break;
	case 'd': _PRINTF("\nDVZ"); break;
	case 'g': _PRINTF("\nFLG"); break;
	case 'b': _PRINTF("\nBPT"); break;
	case 't': _PRINTF("\nTRC"); break;
	case 'u': _PRINTF("\nUND"); break;
	case 'n':
		if (status & R_B_DMA) {		/* DMA abort error */
			_errptblk.er_dma++;
			if (!_mapped) {
				*(char *)PR_STAT = 0;	/* reset status */
				return(0);
			}
			_PRINTF("DMA ABORT");
		}
		else if (status & R_B_PAR) { 	/* Parity error */
			_errptblk.er_par++;
			if (!_mapped) {
				*(char *)PR_STAT = 0;	/* reset status */
				return(0);
			}
			_PRINTF("PARITY ERROR");
		}
		else 
		 	_PRINTF("NMI"); 	/* NMI from MMU or PC */
		break;
	}
	_PRINTF(" TRAP ENCOUNTERED  (STATUS=%2x)\n",status);
	_regprint();
	if (status & (R_B_DMA | R_B_PAR))
		*(char *)R_STAT = 0;		/* reset status */

	_monitor();
}

/*
 * Scaled down _version of C Library _PRINTF.
 * Only %s %d %x %X are recognized.   %d only works for 0 < x < 30
 * Used to print diagnostic information
 * directly on console tty.
 * It is not interrupt driven.
 */
_PRINTF(fmt, x1, x2)
register char *fmt;
unsigned x1, x2;
{
	register int		c;
	register unsigned int	*adx;
	register char		*s;
	register int		siz;

	adx = &x1;
	_xonxoff();
loop:
	siz = 0;
	while((c = *fmt++) != '%') {
		if(c == '\0')
			return;
		_putchar(c);
	}
	c = *fmt++;
	if (c >= '0' && c <= '9') {
		siz = c - '0';
		c = *fmt++;
	}
	if (c == 'l')
		c = *fmt++;
	if (c >= 'A' && c <= 'Z')
		c -= 'A' - 'a';
	if (c == 'd' || c == 'u' || c == 'o') 
		_printn((long)*adx, c=='o' ? 8 : 10);
	else if (c == 'x')
		_printhex((long)*adx,siz);
	else if(c == 'c')
		_putchar((char *)*adx);
	else if (c == 'e' || c == 'f' || c == 'g') {
		_printhex((long)*adx, 8);
		if ( adx == &x1 )
			adx = &x2;
		else
			adx++;
		_putchar(' ');
		_printhex((long)*adx, 8);
	} else if(c == 's') {
		s = (char *)*adx;
		while(c = *s++)
			_putchar(c);
	}
	if ( adx == &x1 )
		adx = &x2;
	else
		adx++;
	goto loop;
}

/*
 * Print an unsigned integer in base b.
 */
_printn(n, b)
long n;
{
	register long a;

	if (n<0) {	/* shouldn't happen */
		_putchar('-');
		n = -n;
	}
	if(a = n/b)
		_printn(a, b);
	_putchar(_hexchar[(int)(n%b)]);
}


/*
 * Print an unsigned integer in hex.
 */
_printhex(n,siz)  unsigned long n; long siz;
{
	register char	ch;
	register int	i;
	int	max;

	if (siz == 0)
		siz = 8;

	for (i=0; i<siz; i++) {
		ch = (n>>((siz-1)*4)) & 0xf;
		_putchar(_hexchar[ch]);
		n<<=4;
	}
}

_gets(buf)
register char	*buf;
{
	int	c;
	char	*lp;

	lp = buf;
	for (;;) {
		c = _getchar() & 0177;
		if (lp >= (buf+50)) 
			lp--;
	store:
		switch(c) {
		case '\n':
		case '\r':
			c = '\n';
			*lp++ = '\0';
			return;
		case '\b':
		case '#':
			lp--;
			if (lp < buf)
				lp = buf;
			continue;
		case 044:			/* ctrl x */
		case '@':
			lp = buf;
			_putchar('\n');
			continue;
		default:
			*lp++ = c;
			if (lp > (buf+128))
				lp = buf;
		}
	}
}

_xonxoff()
{
	register char ch;
	register char *uptr;
	register char *cptr;

	PCCMD->pc_kflag = S_GO;
	IRQPC();
	while (PCCMD->pc_kflag == S_GO)
		ACKPC();
	if (PCCMD->pc_kflag == S_DONE) 	{	/* if char typed */
		PCCMD->pc_kflag = S_IDLE;
		if (PCCMD->pc_kchar == 0x13)
			_getchar();
		if (PCCMD->pc_kchar == 0177) {
			_exit(-1);
		}
	}
	PCCMD->pc_kflag = S_IDLE;		/* enable normal input */
}

_movb(fromaddr,toaddr,count)
register char *fromaddr, *toaddr;
register count;
{
	while (count--)
		*toaddr++ = *fromaddr++;
}

_dcreat(fname,mode)
char *fname;
{
	register rc;

	rc = _iocmd(_dos_cb,PC_NOP,(DOS_CREAT<<8),0,mode,_mapin(fname,100));

	if ((_dos_cb->io_error) || rc)
		return(-1);

	return(_dos_cb->io_ax);
}

_dopen(fname,mode)
char *fname;
{
	register rc;


	rc = _iocmd(_dos_cb,PC_NOP,(DOS_OPEN<<8) | mode,0,0,_mapin(fname,100));

	if ((_dos_cb->io_error) || rc)
		return(-1);

	return(_dos_cb->io_ax);
}

_dread(fildes,buf,nbyte)
int fildes;
char *buf;
unsigned nbyte;
{
	register rc;
	
	rc = _iocmd(_dos_cb,PC_NOP,DOS_READ<<8,fildes,nbyte,buf);

	if ((_dos_cb->io_error) || rc)
		return(-1);

	return(_dos_cb->io_ax);
}

_dwrite(fildes,buf,nbyte)
int fildes;
char *buf;
unsigned nbyte;
{
	register rc;

	rc = _iocmd(_dos_cb,PC_NOP,DOS_WRITE<<8,fildes,nbyte,buf);

	if ((_dos_cb->io_error) || rc)
		return(-1);

	return(_dos_cb->io_ax);
}

_dlseek(fildes,offset,whence)
int fildes;
long offset;
int whence;
{
	register rc;

	rc = _iocmd(_dos_cb,PC_NOP,(DOS_LSEEK<<8)|whence,fildes,offset>>16,offset);

	if ((_dos_cb->io_error) || rc)
		return(-1);

	return((_dos_cb->io_dx<<16) | _dos_cb->io_ax);
}

_dclose(fildes)
int fildes;
{
	register rc;
	
	rc = _iocmd(_dos_cb,PC_NOP,(DOS_CLOSE<<8),fildes);

	if ((_dos_cb->io_error) || rc)
		return(-1);

	return(0);
}

extern long *_dmapage;
#define OPENDMA	4

_mapin(memaddr,bcount)
long memaddr;
long bcount;
{
	int i;
	int phypage;
	int pagecnt;

#ifndef STAND
	return(memaddr);	/* all pages are mapped in during boot */
#else
	if (bcount + memaddr%PAGESIZE + OPENDMA*PAGESIZE > DMASIZE)
		return(-1);
	phypage = _getpaddr(memaddr - TXTLOG,_memmap)/PAGESIZE;
	pagecnt = (bcount - 1)/PAGESIZE + 2;

	for (i=0; i<pagecnt; i++) {
		_dmapage[i+OPENDMA] = (i+phypage) * PAGESIZE | PG_P | PG_KW;
		_eia(DMASPACE + (OPENDMA+i)*PAGESIZE);
	}

	return(memaddr%PAGESIZE + OPENDMA*PAGESIZE + DMASPACE);
#endif
}

_getpaddr(laddr,memmap)
long laddr;
long memmap;
{
	register long i;
	register long paddr;

	paddr = 0;
	for (i=0; i<32; i++) {
		if ((memmap>>i) & 01) {
			if (laddr < MEMINCR)
				break;
			laddr -= MEMINCR;
		}
		paddr += MEMINCR;
	}
	return(paddr+laddr);
}

mapcomm( iorb )
long iorb;
{
	return (( unsigned short ) iorb);
}

ACKPC()
{
	*(char *) R_C_ACK = 0;
}

IRQPC()
{
	*(char *) R_C_IRQ = 0;
}
