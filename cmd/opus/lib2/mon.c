#include "sys/types.h"
#include "sys/cb.h"
#include "sys/pc32k.h"
#if pm100
#include "sys/dev32k.h"
#endif
#include "sys/doscomm.h"
#include "sys/immu.h"
#include "sys/mtpr.h"
#include "compat.h"

#define max(a,b)	(a<b ? b : a)
#if pm300
#define COUNT() if (!_QUIET) _PRINTF("\b\b%2x",_timerdisp++)
#define ZERO *(short *)0x2000
#else
#define COUNT()
#define ZERO *(short *)0x0
#endif

#ifdef BOOT
#define PROMPT  "+"
#define init	main
#else
#define PROMPT  "*"
#endif

#define BADDR   0xee00  /* Bad address for test */
#define REGMAX  25      /* CPU registers plus PC, etc.*/
#define MAXERR  8
#define NS32016	0
#define NS32032 1
#define NS32332 2
#define CLIPPER 3
#if pm100
#define TOPOFSTACK DMASIZE
#define TSTLVL (*(char *)18)
#else
#define NUMCHUNK 16
#define MEMINCR 0x100000
#define PAGESIZE 0x1000
#define MAXPHYSMEM 0x1000000
#define TOPOFSTACK 256*1024	/* 256KB */
#define TSTLVL (*(char *)0x2025)
#endif

#define BASE    16      /* Default base for input numbers */

struct memerrors {
	long addr;
	long val;
	long val1;
	long val2;
	long badpar;
} 
_memsave[MAXERR];

#ifdef BOOT
int _end;
#endif

struct er_block {
	long er_dma;
	long er_par;
	long er_parbitmap;
	long er_bitmap[MAXERR];
};

struct er_block _errptblk;

char *_regname[];
unsigned _r[];

extern end;             /* End of bss */
long _exitval;		/* Return value to dos */
long _memerr;           /* Cumulative memory errors encountered */
long _memmap = 0;       /* Bit map of available 512K blocks (data area only) */
long _badmemmap = 0;    /* Bit map of bad 512K blocks (data area only) */
long _maxmem = 0;       /* Maximum size of memory (data area only) */
long _badmaxmem = 0;    /* Size of bad memory (data area only) */
long _topofstack = 0;   /* Top of stack for executing program (data area only) */
long _proctype;		/* Type of processor */
extern short _mapped;	/* Flag indicating whether we are mapped or not */
#if pm300
long _mempte[1];	/* memory 2nd level page table entries */
long _timerdisp;	/* memory test timer */
#endif

char _line[128];        /* Input buffer */
char *_lineptr;         /* Pointer to input buffer */
char *_oldlineptr;      /* Previous pointer to input buffer */

#ifndef SASH
unsigned _memlist[100]; /* List of values to fill memory */
char _tmpbuf[1024];     /* Temporary buffer for reading and writing to disk */
#endif

long _pass = 0;         /* counts number of passes of _repeated test */
long _repeat = 0;       /* Loop until a DEL is hit */
short _cbindex;         /* next available command blk */
short _quiet = 0;	/* don't print much on startup */
short _QUIET = 0;	/* don't print anything on startup */
short _verbose = 0;	/* print diagnostics on startup */
short _bootdebug = 0;	/* print diagnostics while booting */
short _loadsyms = 0;	/* load symbols with program */

unsigned short _mode;

#define CMDSIZE         256
char _cmdline[CMDSIZE];
char *_cmdptr;

struct iorb *_sys_cb;
struct io_clk *_clk_cb;
struct io_time *_time_cb;
struct io_conin *_conin_cb[CONMAX];
struct io_conout *_conout_cb[CONMAX];
struct io_hdisk *_hdisk_cb[DKMAX];
struct io_serin *_serin_cb[ASMAX];
struct io_serout *_serout_cb[ASMAX];
struct io_mt *_mt_cb[MTMAX];
struct iorb *_dos_cb; 
struct io_lpt *_lpt_cb[LPMAX];
struct io_gn *_gn_cb[GNMAX];

struct io_serin *_dupin_cb;
struct io_serout *_dupout_cb;

char *_version = "06/26/90"; /* String containing version of monitor */
#ifdef BOOT
char _initstr[] = "\n\rOpus5 Diagnostic Monitor  5.01 \n\r";
#else
char _initstr[] = "\n\rOpus5 Standalone Shell  5.01 \n\r";
#endif
#define COUNTMAX 100000;

char *_help[] = {
#ifndef SASH
	"\n\r",
	"Opus5 Boot Monitor -- Command format is similar to that",
	" of MS/DOS debug program\n",
	"\n\r",
#ifndef STAND
	"Boot       B  progname                    Load and execute",
	" requested program\n\r",
#endif
	"Copy       C  range address               Compares memory\n\r",
	"Dump       D  [address] | [range]         Displays memory\n\r",
	"Enter      E  address [list]              Changes memory\n\r",
	"Fill       F  range list                  Changes memory blocks\n\r",
	"Hexarith   H  value value                 Hexadecimal add-subtract\n\r",
	"Input      I  portaddress                 Reads/displays input byte\n\r",
	"Load       L  [address [drive sector sector]]  Loads file or sectors\n\r",
	"Move       M  range address               Moves memory block\n\r",
	"Output     O  portaddress byte            Sends output byte\n\r",
	"Pause      P  pause                       Pauses for MS/DOS\n\r",
	"Quit       Q                              Ends debug program\n\r",
	"Register   R                              Displays registers/flags\n\r",
	"Search     S  range list                  Searches for characters\n\r",
	"Version    V                              Displays version of monitor\n\r",
	"Write      W  [address [drive sector sector]]  Writes file or sectors\n\r",
	"\n\r",
	"NOTE: A command preceded with an '**' is repeated forever, a command\n\r",
	"      with '* loopcnt' is repeated loopcnt times.\n\r",
	"\n\r",
#endif
	0
};


main(argc,argv)
int argc;
char *argv[];
{
	static  beenhere = 0;
	static  notsashonly = 0;
	unsigned long tmp;
	int countdown;
	int i;
	int j;
	int printcnt;

#ifdef SASH
    if (beenhere++ == 0) {
	_getcmd(_cmdline,CMDSIZE);      /* ignore this program's invocation */
	for (i=0; _cmdline[i]; i++) {
		if (_cmdline[i] == '-') {
			if (_cmdline[i+1] == 'd')
				_bootdebug++;
			if (_cmdline[i+1] == 's')
				_loadsyms++;
			if (_cmdline[i+1] == 'v')
				_verbose++;
			if (_cmdline[i+1] == 'q')
				_quiet++;
			if (_cmdline[i+1] == 'Q') {
				_quiet++;
				_QUIET++;
			}
		}
	}
#endif

#if pm100
    _proctype = (*(char *)R_STAT & 03);
#endif
#if pm200
    _proctype = NS32332;
#endif
#if pm300
    _proctype = CLIPPER;
    _PRINTF(_initstr);
    _PRINTF("Level 2 Test:   ");
    _mapmemtest();
#endif
    if (!_QUIET)
	_PRINTF("\b\bPASS\n");
    if (!_quiet) {
	_PRINTF("Configuration: ");
	switch (_proctype) {
	case NS32016:
		_PRINTF("108PM   ");
		break;
	case NS32032:
		_PRINTF("110PM   ");
		break;
	case NS32332:
		_PRINTF("200PM   ");
		break;
	case CLIPPER:
		_PRINTF("300PM   ");
		break;
	}
	if (((_maxmem/1024) % 1024) == 0) {
		_PRINTF("%dMB    ",_maxmem/(1024*1024));
		if (_maxmem < 10*1024*1024) 	/* 10MB */
			_PRINTF(" ");
	} else
		_PRINTF("%4dKB  ",_maxmem/1024);
	printcnt = 2;
	for (i=0; i<COMMPAGE->dixsiz; i++) {
		static int dknum = 0;
		static int ttynum = 0;
		static int lpnum = 0;
		static int mtnum = 0;
		static int gnnum = 0;

		if (!(++printcnt % 9)) {
			_PRINTF("\n               ");
			printcnt = 1;
		}
		switch(COMMPAGE->dix[i]) {
		case CONIN:
			for (j=0; j<CONMAX; j++)
				if (_conin_cb[j] && _conin_cb[j]->io_index==i) {
					if (j == 0)
						_PRINTF("console ");
					else
						_PRINTF("con%1d    ",j);
					break;
				}
			if (j == CONMAX)
				printcnt--;
			break;
		case DISK:
			for (j=0; j<DKMAX; j++) {
				if (_hdisk_cb[j] && _hdisk_cb[j]->io_index==i) {
					switch (j) {
					case 14:
						_PRINTF("flpa    ");
						break;
					case 13:
						_PRINTF("flpb    ");
						break;
					case 12:
						_PRINTF("flpA    ");
						break;
					case 11:
						_PRINTF("flpB    ");
						break;
					default:
						if (j < 10)
							_PRINTF("dsk/%d   ",j);
						else
							_PRINTF("dsk/%d  ",j);
						break;
					}
					break;
				}
			}
			if (j == DKMAX)
				printcnt--;
			break;
		case TAPE:
			for (j=0; j<MTMAX; j++)
				if (_mt_cb[j] && _mt_cb[j]->io_index == i) {
					_PRINTF("mt/%d    ",j);
					break;
				}
			if (j == MTMAX)
				printcnt--;
			break;
		case SERIN:
			for (j=0; j<ASMAX; j++)
				if (_serin_cb[j] && _serin_cb[j]->io_index==i) {
					_PRINTF("tty%d   ",j);
					if (j < 10)
						_PRINTF(" ");
					break;
				}
			if (j == ASMAX)
				printcnt--;
			break;
		case LPT:
			for (j=0; j<LPMAX; j++)
				if (_lpt_cb[j] && _lpt_cb[j]->io_index == i) {
					if (j) {
						_PRINTF("lp%1d     ",j);
						break;
					} else {
						_PRINTF("lp      ",j);
						break;
					}
				}
			if (j == LPMAX)
				printcnt--;
			break;
		case DOS:
			_PRINTF("dos     ");
			break;
		case PCNET:
			_PRINTF("net     ");
			break;
		case ETHER:
			_PRINTF("ether   ");
			break;
		case VDI:
			_PRINTF("vdi     ");
			break;
		case GEN:
			for (j=0; j<GNMAX; j++) {
				if (_gn_cb[j] && _gn_cb[j]->io_index == i) {
					switch (j) {
					case 10:
					case 11:
					case 12:
						_PRINTF("gn%d    ",j);
						break;
					case 13:
						_PRINTF("udio    ");
						break;
					case 14:
						_PRINTF("graph   ");
						break;
					case 15:
						_PRINTF("cgi     ");
						break;
					default:
						_PRINTF("gn%d     ",j);
						break;
					}
					break;
				}
			}
			if (j == GNMAX)
				printcnt--;
			break;
		default:
			printcnt--;
		}
	}
	_PRINTF("\n");
#if pm100
	_memerrpr(_verbose);
#endif
    }

#ifdef BOOT
	_monitor();
#endif

#ifdef SASH
	_init();
	if (!_quiet)
		_PRINTF("\n");
    }

	if (_getcmd(_cmdline,CMDSIZE)) {
		main();        	/* ;; specified, so start sash */
		_getcmd(_cmdline,CMDSIZE);
	}
	else if ((_cmdline[0] == 0) && (notsashonly == 0)) {
		main();		       	/* nothing specified, so start sash */
	}
	while (_cmdline[0] != 0) {
		notsashonly++;
		_boot(_cmdline);
		if (_getcmd(_cmdline,CMDSIZE)) {
			main();      /* if ;; call sash */
			_getcmd(_cmdline,CMDSIZE);
		}
	}
#endif
	_syscmd(PC_DOSRTN,_exitval);
}

_getcmd(strptr,cnt)
char *strptr;
int cnt;
{
	*strptr = _syscmd(PC_CMDCHAR,0,0);	/* get last semi-colon */
	if (*strptr == ';')
		return(1);
	while (*strptr == ' ' || *strptr == '\t')
		*strptr = _syscmd(PC_CMDCHAR,0,0);
	while (*strptr != 0 && *strptr != ';' && --cnt)
		*++strptr = _syscmd(PC_CMDCHAR,0,0);
	*strptr = 0;
	return(0);
}

#ifdef STAND
_callit(arglist)
char *arglist;
{
	int i;
	int argc;
	char *argv[20];
	char xarglist[128];
	char *narglist;

	for (i=0; i<128; i++)
		xarglist[i] = *arglist++;
	narglist = xarglist;

	argc = 0;
	if (*narglist == 0) {
		argv[argc] = narglist;
		return(main(argc,argv));
	}

	while (1) {
		while (*narglist == '\t' || *narglist == ' ')
			*narglist++ = 0;
		argv[argc++] = narglist++;
		while ((*narglist != '\t') && (*narglist != ' ')) {
			if (*narglist == 0) {
				argv[argc] = (char *)0;
				return(main(argc,argv,0));
			}
			narglist++;
		}
	}
}
#endif

_monitor() {
	long addr1;
	long addr2;
	long count;
	unsigned mem1, mem2;
	int i, j, k;
	unsigned command;
	unsigned char rc;
	static entrycnt = 0;
	char *cmdptr;

	entrycnt++;
	_repeat = 0;
	_mode = 1;
	for (;;) {
#ifndef SASH
		if (_repeat) {
			if (_repeat-- == -1)
				_repeat = -1;
			_PRINTF("PASS # = ");
			if (_pass+2 >= 0x100)
				_PRINTF("%x\n",++_pass+1);
			else
				_PRINTF("%2x\n",++_pass+1);
		} 
		else {
			_pass = 0;
			if (entrycnt > 1)
				_PRINTF("%1x",entrycnt);
			_PRINTF(PROMPT);
			_gets(_line);
		}
		_lineptr = _line;
		_skipblanks();

		if (!_repeat && (*_lineptr == '*')) {
			if (*++_lineptr == '*') {
				_repeat = -1;
				_lineptr++;
			} 
			else 
				_repeat = _getnum() - 1;
			for (i=0; i<(_lineptr-_line); i++)
				_line[i] = ' ';
			_skipblanks();
		};
#else
		_PRINTF(PROMPT);
		_gets(_line);
		_lineptr = _line;
#endif

		_skipblanks();
		if (*_lineptr)
			command = *_lineptr++;
		else
			continue;

		_skipblanks();

		switch (command) {
#ifndef SASH

		case '?':
			i = 0;
			do {
				_putstr(_help[i++]);
			} 
			while (_help[i]);
			continue;

#if pm100
		case 'b': 
		case 'B':     /* Boot requested program */
#ifndef STAND
			for (i=0; i<CMDSIZE; i++)
				_cmdline[i] = _lineptr[i];
			_lineptr = _cmdline;
			while (1) {
				cmdptr = _lineptr;
				while (*_lineptr && (*_lineptr != ';'))
					_lineptr++;
				if (!(*_lineptr)) {
					_boot(cmdptr);
					break;
				}
				*_lineptr++ = 0;
				_boot(cmdptr);
			}
			continue;
#else
			goto error;
#endif
#endif

		case 'c': 
		case 'C':     /* Compare memory */
			_getrange(&addr1,&count);
			addr2 = _getnum();
			while (count--) {
				if (_mode == 4) {
					mem1 = *(unsigned long *)addr1;
					if (mem1 != (mem2 = *(unsigned long *)addr2))
						_PRINTF("%8x  %8x  %8x  %8x\n",
						addr1, mem1, addr2, mem2);
					addr1+=4;
					addr2+=4;
				} else if (_mode == 2) {
					mem1 = *(unsigned short *)addr1;
					if (mem1 != (mem2 = *(unsigned short *)addr2))
						_PRINTF("%8x  %4x  %8x  %4x\n",
						addr1, mem1, addr2, mem2);
					addr1+=2;
					addr2+=2;
				} else {
					mem1 = *(unsigned char *)addr1;
					if (mem1 != (mem2 = *(unsigned char *)addr2))
						_PRINTF("%8x  %2x  %8x  %2x\n",
						addr1, mem1, addr2, mem2);
					addr1++;
					addr2++;
				}
			}
			continue;

		case 'd': 
		case 'D':     /* Display memory */
			_getrange(&addr1,&count);

			if (count == 0) 
				count = 0x80;
			while (count > 0) {
				_PRINTF("%8x ",addr1);
				j = addr1 % 16;
				k = ((j + count) < 16) ? j + count : 16;
				for (i = j; i < k; i++) {
					_PRINTF("%2x",*(char *)(addr1+i-j));
					if (i == 7)
						_putchar('-');
					else
						_putchar(' ');
				}
				_PRINTF("  ");
				for (i = 0; i < j; i++)
					_PRINTF("   ");
				for (i = k; i < 16; i++)
					_PRINTF("   ");
				for (i = j; i < k; i++) {
					mem2 = *(unsigned char *)(addr1+i-j);
					if ((mem2 < ' ') || (mem2 > '~'))
						mem2 = '.';
					_putchar(mem2);
				}
				_nl();
				addr1 += k - j;
				count -= k - j;
			}
			continue;

		case 'e': 
		case 'E':     /* change memory */
			addr1 = _getnum();
			j = _getlist(_memlist);
			if (j < 0)
				continue;
			if (j == 0) {
				if (_mode == 4)
					_PRINTF("%8x  %2x.",addr1,*(long *)addr1);
				else if (_mode == 2)
					_PRINTF("%8x  %2x.",addr1,*(short *)addr1);
				else
					_PRINTF("%8x  %2x.",addr1,*(char *)addr1);
				_gets(_line);
				_lineptr = _line;
				_memlist[j++] = _getnum();
				if (*_lineptr)
					goto error;
			}
			for (i=0; i<j; i++)
				if (_mode == 4) {
					*(long *)addr1 = _memlist[i];
					addr1 += 4;
				} else if (_mode == 2) {
					*(short *)addr1 = _memlist[i];
					addr1 += 2;
				} else
					*(char *)addr1++ = _memlist[i];
			continue;

		case 'f': 
		case 'F':     /* change memory blocks */
			_getrange(&addr1,&count);
			j = _getlist(_memlist);
			if (j < 1)
				continue;
			i = 0;
			while (count--) {
				if (_mode == 4) {
					*(long *)addr1 = _memlist[i++];
					addr1+=4;
				} else if (_mode == 2) {
					*(short *)addr1 = _memlist[i++];
					addr1+=2;
				} else {
					*(char *)addr1 = _memlist[i++];
					addr1++;
				}
				i = i%j;
			}
			continue;

		case 'h': 
		case 'H':     /* hexarithmetic */
			addr1 = _getnum();
			_skipblanks();
			addr2 = _getnum();
			_PRINTF("%8x  %8x\n",addr1 + addr2, addr1 - addr2);
			continue;

#if pm100
		case 'i': 
		case 'I':     /* read/display input byte */
			addr1 = _getnum();
			_PRINTF("%2x\n",*(char *)(0xfff000 + (addr1%4)*512));
			continue;
#endif

		case 'm': 
		case 'M':     /* move memory block */
			_getrange(&addr1,&count);
			addr2 = _getnum();
			while (count--)
				if (_mode == 4) {
					*(long *)addr2 = *(long *)addr1;
					addr1 += 4;
					addr2 += 4;
				} else if (_mode == 2) {
					*(short *)addr2 = *(short *)addr1;
					addr1 += 2;
					addr2 += 2;
				} else
					*(char *)addr2++ = *(char *)addr1++;
			continue;

#if pm100
		case 'o': 
		case 'O':     /* send output byte */
			addr1 = _getnum();
			mem1 = _getnum();
			*(char *)(0xfff000 + (addr1%4)*512) = mem1;
			continue;
#endif

		case 'p': 
		case 'P':     /* pause (to go to dos) */
			_syscmd(PC_PAUSE,_getnum());
			continue;

		case 'r': 
		case 'R':     /* display registers from last trap */
			_regprint();
			continue;

		case 's': 
		case 'S':     /* search for characters */
			_getrange(&addr1,&count);
			j = _getlist(_memlist);
			if (j < 1)
				continue;
			count -= j;     /* don't look past end of range */
			while (count--) {
				for (i=0; i<j; i++) {
					if ((*(unsigned char *)(addr1+i)) 
					    != _memlist[i])
						break;
				}
				if (i == j)
					_PRINTF("%8x\n",addr1);
				addr1++;
			}
			continue;

		case 't': 
		case 'T':     /* run various tests */
			_skipblanks();
			switch(*_lineptr++) {
#if pm100
			default:
				_PRINTF("Unknown test\n");
				break;
#endif /* pm100 */
#if pm300
			case 'm':
			case 'M':
				TSTLVL = _getnum();
				_PRINTF("RAM TEST (LEVEL %d):   ",TSTLVL);
				_mapmemtest();
				if (!_memerrpr(0))
					_PRINTF("\b\bPASSED\n");
				else
					_monitor();
				break;

			case 'c':	/* set cache mode */
				if (*_lineptr == 0) {
				    _PRINTF("Current cache mode is %d.\n\n",
					_mode);
				    count = -1;
				} else
				    count = _getnum();
				if (count != 0 && count != 2 && count != 4 &&
					count != 6) {
				    _PRINTF("\ttc0\tPrivate write-through\n");
				    _PRINTF("\ttc2\tShared write-through\n");
				    _PRINTF("\ttc4\tPrivate copy-back\n");
				    _PRINTF("\ttc6\tNon-cacheable\n");
				    break;
				}
				for (i=_topofstack; i<_maxmem; i+=PAGESIZE) {
					_mempte[i/PAGESIZE] =
						(_mempte[i/PAGESIZE] &
						SDMA_MASK) | count;
				}
				flushcache();
				break;
			default:
				_PRINTF("Clipper diagnostic tests:\n");
				_PRINTF("\ttc [0|2|4|6]\t\tCache mode change\n");
				_PRINTF("\ttm [level]\t\tMemory test [level]\n");
				_PRINTF("\ttr base bound repcnt\tRam test\n");
				_PRINTF("\ttw [1|2|4]\t\tWidth of memory access for C,E,F,M commands\n");
				break;
#endif /* pm300 */
			case 'r':
			case 'R':
				_PRINTF("RAM ADDRESS TEST:   ");
				addr1 = _getnum();
				addr2 = _getnum();
				if ((count = _getnum()) == 0);
					count = 1;
				_timerdisp = 0;
				_memerr = _memtest(addr1,addr2,count);
				if (!_memerrpr(0))
					_PRINTF("\b\bPASSED\n");
				else
					_monitor();
				_PRINTF("RAM BIT TEST:   ");
				_timerdisp = 0;
				_memerr = _smemtest(addr1,addr2,count);
				if (!_memerrpr(0))
					_PRINTF("\b\bPASSED\n");
				else
					_monitor();
				break;
			case 'w':
			case 'W':
				if (*_lineptr == 0)
					_PRINTF("Width for memory access is %d bytes.\n",_mode);
				else
					_mode = _getnum();
				break;
			}
			continue;

		case 'v': 
		case 'V':     /* display _version number of monitor */
			_PRINTF("VERSION = %s\n",_version);
			continue;

		case 'l':     /* load file or abs disk sectors */
		case 'L':
		case 'w': 
		case 'W':     /* write file or abs disk sectors */
		case 'a': 
		case 'A':     /* assemble statements */
		case 'g': 
		case 'G':     /* execute with optional breakpoints */
		case 'u': 
		case 'U':     /* unassemble instructions */
			_repeat = 0;
			_PRINTF("    Not implemented\n");
			continue;

		default:
error:
			_inputerr();
			_repeat = 0;
			continue;
#else
		case '$':
			main();			/* call sash */
			break;
#endif
		case 'Q':     /* quit debug program */
#ifdef SASH
			_pause(-1);
#endif
		case 'q': 
			entrycnt--;
			return;
		}
	}
}

#if pm300
flushcache() {
	*(long *)RESETD = CR_LVW | CR_LVX;
	*(long *)RESETI = CR_LVW | CR_LVX;
}
#endif

#ifndef SASH
_getrange(base,count)
unsigned *base;
unsigned *count;
{
	*base = _getnum();
	_skipblanks();
	if (*_lineptr == 'l' || *_lineptr == 'L') {
		_lineptr++;
		_skipblanks();
		*count = _getnum();
	} 
	else {
		*count = _getnum();
		if (*count == 0)
			*count = 0x80;
		else
			*count -= *base;
	}
}

_getlist(_memlist)
unsigned *_memlist;
{
	int entries;

	entries = 0;
	_skipblanks();
	_oldlineptr = 0;
	while (*_lineptr) {
		if (*_lineptr == '"') {
			_lineptr++;
			while ((*_lineptr != '"') && *_lineptr)
				_memlist[entries++] = *_lineptr++;
			_lineptr++;
			_skipblanks();
			continue;
		}
		if (_oldlineptr == _lineptr) {
			_inputerr();
			return(-1);
		}
		_oldlineptr = _lineptr;
		_memlist[entries++] = _getnum();
		_skipblanks();
	}
	return(entries);
}
#endif

_regprint() {
	register int i;
	extern _regnamesize;

	for (i = 0; i < _regnamesize; i++) {
		_PRINTF("%s = %8x  ",_regname[i],_r[i]);
		if ((i & 3) == 3)
			_nl();
	}
	if ((i & 3) != 0)
		_nl();
}

_reinit() {
	_syscmd(PC_REINIT);
}       

#ifndef SASH
long
_getnum() {
	register char ch;
	register long newnum;
	register long num;

	_skipblanks();
	num = 0;
	for(;;) {
		ch = *_lineptr;
		newnum = num * BASE + ch;
		if ((ch >= '0') && (ch <= '9'))
			num = newnum - '0';
		else if ((ch >= 'A') && (ch <= 'F'))
			num = newnum - 'A' + 10;
		else if ((ch >= 'a') && (ch <= 'f'))
			num = newnum - 'a' + 10;
		else
			return(num);
		_lineptr++;
	}
}
#endif

_skipblanks() {
	register char ch;

	ch = *_lineptr;
	while ((ch == ' ') || (ch == '\t'))
		ch = *++_lineptr;
}

_nl() {
	_putchar('\n');
}

_memsize() {
	register long i;

	_memmap = _maxmem = 0;
	_badmemmap = _badmaxmem = 0;

	for (i=NUMCHUNK; i>=1; i--)
		*(long *)(i*MEMINCR-4) = i | (i<<8);

	for (i=1; i<=NUMCHUNK; i++)
		if (((*(long *)(i*MEMINCR-4) & 0xff) == i) ||
				(((*(long *)(i*MEMINCR-4) >> 8)&0xff) == i)) {
			_memmap |= 1 << (i-1);
			_maxmem += MEMINCR;
		}

	/* Clear bogus dma and parity errors found while sizing memory */
	_errptblk.er_dma = _errptblk.er_par = 0;
#if STAND
	_topofstack = _maxmem;		/* if standalone, put stack at top */
#else
	_topofstack = TOPOFSTACK;	/* if diagnostic, set much lower */
#endif
	return(_topofstack);
}

_mapmemtest()
{
	register int i;
	register int errcnt = 0;
	register int errs = 0;
	register int parerrs;

	_timerdisp = 0;

	for (i=0; i<(MAXPHYSMEM/MEMINCR); i++) {
		if ((_memmap>>i) & 01) {
			parerrs = _errptblk.er_par;
			if (TSTLVL == 2)		/* no memory test */
				break;
	    	    	errs = _memtest(max(_topofstack,i*MEMINCR),(i+1)*MEMINCR,1);
			if (TSTLVL == 1)		/* extended test */
	    	    		errs += _smemtest(max(_topofstack,i*MEMINCR),
					(i+1)*MEMINCR,1);
			if (errs || (parerrs != _errptblk.er_par)) {
				_memmap &= ~(01<<i);
				_badmemmap |= (01 << i);
				_maxmem -= MEMINCR;
				_badmaxmem += MEMINCR;
			}
			if (parerrs != _errptblk.er_par)
				_errptblk.er_parbitmap |= (01 << i);
	    	    	errcnt += errs;
		}
	}
	ZERO = _memmap;
	return(errcnt);
}
	
_memtest(base,bound,repcnt)
char *base;
register char *bound;
long repcnt;
{
	register long *i;
	register long modi;
	register long val1;
	long errcnt = 0;
	long parerrs;
	long val2;

	COUNT();
	while (repcnt--) {
		for (i=(long *)base; i<(long *)bound; ) {
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
			*(i++) = (long)i;
		}

		for (i=(long *)base; i<(long *)bound; i++) {
			if ((val1 = *i) != (long)i) {
				notifypc();
				parerrs = _errptblk.er_par;
				val2 = *i;
				if ((_saverr(errcnt++,i,i,val1,val2,
					parerrs != _errptblk.er_par) != 0)
					&& !_verbose)
					return(errcnt);
			}
		}

		modi = 0xf7f7efef;
		for (i=(long *)base; i<(long *)bound; ) {
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
			*(i++) = (long)i ^ modi;
		}

		for (i=(long *)base; i<(long *)bound; i++) {
			modi = ((long) i ^ 0xf7f7efef);
			if ((val1 = *i) != modi) {
				notifypc();
				parerrs = _errptblk.er_par;
				val2 = *i;
				if ((_saverr(errcnt++,i,modi,val1,val2,
					parerrs != _errptblk.er_par) != 0)
					&& !_verbose)
					return(errcnt);
			}
		}
	}
	return(errcnt);
}

notifypc() {
#if pm100
	if (_mapped)
		*(char *)R_WAIT = 0;	/* signal pc */
	else
		*(char *)PR_WAIT = 0;	/* signal pc */
#else
	/* NOTIFY PC OF ERROR */
#endif
}

_smemtest(base,bound,repcnt)
char *base;
register char *bound;
long repcnt;
{
	register i;
	long errcnt = 0;

	while (repcnt--) {
		errcnt = _memvaltest(errcnt,base,bound,repcnt,0);
		errcnt = _memvaltest(errcnt,base,bound,repcnt,-1);
		for (i=0; i<16; i++)
			errcnt = _memvaltest(errcnt,base,bound,repcnt,
				(1<<i) | ((1<<i)<<16));
		for (i=0; i<16; i++)
			errcnt = _memvaltest(errcnt,base,bound,repcnt,
				~((1<<i) | ((1<<i)<<16)));
	}
	return(errcnt);
}

_memvaltest(errcnt,base,bound,repcnt,value)
long errcnt;
char *base;
register char *bound;
long repcnt;
register unsigned long value;
{
	register long *i;
	register long val1;
	long val2;
	long parerrs;

	COUNT();
	for (i=(long *)base; i<(long *)bound; i++)
		*i = (long)value;

	for (i=(long *)base; i<(long *)bound; i++)
		if ((val1 = *i) != (long)value) {
			notifypc();
			parerrs = _errptblk.er_par;
			val2 = *i;
			if ((_saverr(errcnt++,i,i,val1,val2,
				parerrs != _errptblk.er_par) != 0) && !_verbose)
				return(errcnt);
		}
	return(errcnt);
}

_saverr(errcnt,addr,shouldbe,was1,was2,parity) {
	if (errcnt < MAXERR) {
		_memsave[errcnt].addr = addr;  
		_memsave[errcnt].val = shouldbe;
		_memsave[errcnt].val1 = was1;
		_memsave[errcnt].val2 = was2;
		_memsave[errcnt].badpar = parity;
	}
	_errptblk.er_bitmap[addr/MEMINCR] |=
			(shouldbe ^ was1) | (shouldbe ^ was2);
	return(errcnt >= MAXERR);	/* return non-zero if too many */
}

_memerrpr(vflag) {
	int i, j; 
	int bit;
	unsigned bitmask;
	int maxpmem;
	int banksize;

	maxpmem = _maxmem + _badmaxmem;
	if (vflag || _memerr || _errptblk.er_par) {
	    	_PRINTF("\nMEMORY TEST: %dKB PASSED, %dKB FAILED, %d ERRORS\n",
			_maxmem/1024,_badmaxmem/1024,_memerr);
		if (_proctype == NS32016) {
		    _PRINTF("    BANK (512 KB)  --> ");
		    for (i=0; i < (MAXPHYSMEM / MEMINCR); i++) {
			if ((i % 16) == 0) {
				for (j=0; j < (MAXPHYSMEM / MEMINCR); j++) {
					if (i == 0)
						_PRINTF("   #%1x ",j);
					else
						_PRINTF("  #%2x ",j+i);
				}
				_PRINTF("\n                        ");
			}
			if ((_memmap>>i) & 01)
				_PRINTF("++++");
			else if ((_badmemmap>>i) & 01)
				_PRINTF("%4x",_errptblk.er_bitmap[i]|
					_errptblk.er_bitmap[i]>>16);
			else 
				_PRINTF("....");
			if ((_errptblk.er_parbitmap>>i) & 01)
				_PRINTF("* ");
			else
				_PRINTF("  ");
		    }
		} else {
		    _PRINTF("    BANK (1 MB)   -->  ");
		    for (i=0; i < (MAXPHYSMEM / MEMINCR); i+=2) {
			if ((i % 16) == 0) {
				for (j=0; j < (MAXPHYSMEM / MEMINCR); j+=2) {
					if (i == 0)
						_PRINTF("       #%1x ",j/2);
					else
						_PRINTF("      #%2x ",(j+i)/2);
				}
				_PRINTF("\n                        ");
			}
			if ((_memmap>>i) & 03)
				_PRINTF("++++++++");
			else if ((_badmemmap>>i) & 03)
				_PRINTF("%8x",_errptblk.er_bitmap[i]);
			else 
				_PRINTF("........");
			if ((_errptblk.er_parbitmap>>i) & 03)
				_PRINTF("* ");
			else
				_PRINTF("  ");
		    }
		}
		_PRINTF("\n");
	    	for (i=0; i<_memerr; i++) {
	    		if (i == MAXERR) {
	    			_PRINTF("    ...");
	    			break;
	        	}
			if (_proctype == NS32016)
				banksize = MEMINCR;
			else
				banksize = MEMINCR*2;
			_PRINTF("    ADDR=%6x (BANK %1x), WRITTEN=%8x, ",
	    			_memsave[i].addr,_memsave[i].addr/banksize,
				_memsave[i].val);
			if (_memsave[i].val1 == _memsave[i].val2) {
				bitmask = _memsave[i].val ^ _memsave[i].val1;
				if (_proctype == NS32016) {
					bitmask = ((bitmask>>16) | bitmask)
						& 0xffff;
					for (bit=0; bit<16; bit++) {
						if ((bitmask>>bit) & 1) {
							if ((bitmask>>bit) != 1)
								bit = 32;
							break;
						}
					}
					bit = 32;
				} else {
					for (bit=0; bit<32; bit++) {
						if ((bitmask>>bit) & 1) {
							if ((bitmask>>bit) != 1)
								bit = 32;
							break;
						}
					}
				}
				_PRINTF(" READ=%8x%s",_memsave[i].val1,
					_memsave[i].badpar ? "*" : " ");
				if (bit < 32)
	        			_PRINTF (" (BIT %d)\n",bit);
				else
					_PRINTF("\n");
			} else
	        		_PRINTF("READ1=%8x, READ2=%8x%s\n",
				 	_memsave[i].val1,_memsave[i].val2,
					_memsave[i].badpar ? "*" : " ");
	    	}
	}

	if (_errptblk.er_dma)
		_PRINTF("\nMEMORY TEST: DMA: %d ERRORS\n",_errptblk.er_dma);
	if (_errptblk.er_par)
		_PRINTF("\nMEMORY TEST: PARITY: %d ERRORS\n",
			_errptblk.er_par);
	/*if (_memerr || _errptblk.er_par || _errptblk.er_dma)
		_PRINTF("\n");*/
	_errptblk.er_dma = _errptblk.er_par = 0;

	return(_memerr);
}

_inputerr() {
	int i;

	for (i=0; i<(_lineptr - _line); i++)
		_putchar(' ');
	_PRINTF(" ^ Error\n");
}

_wait() {
	char line[50];

	_PRINTF("#");
	_monitor();
}

#ifndef SASH
_init() {
}
#endif
