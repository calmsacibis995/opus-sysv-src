#include "stdio.h"
#include "stand.h"
#include "sys/cb.h"
#include "sys/iorb.h"
#include "sys/commpage.h"
#include "sys/cp.h"
#include "sys/doscomm.h"
#include "sys/immu.h"
#if pm300
#include "sys/mtpr.h"
#endif
#if pm400
#include "sys/sysmacros.h"
#endif
#include "compat.h"

#define max(a,b)	(a<b ? b : a)
#define min(a,b)	(a<b ? a : b)
#if pm300
#define COUNT() if (!_QUIET) _PRINTF("\b\b%2x",_timerdisp++)
#define ZERO *(short *)0x2000
#endif
#if pm200 || pm400
#define COUNT() if (!_QUIET && !_VERBOSE) _PRINTF("\b\b%2x",_timerdisp++)
#define ZERO *(short *)0x0
#endif
#if pm100
#define COUNT()
#define ZERO *(short *)0x0
#endif

#ifdef BOOT
#define PROMPT  "+"
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
#define M88K 	4
#if pm100
#define TOPOFSTACK DMASIZE
#define TSTLVL (*(char *)18)
#else
#define PAGESIZE 0x1000
#if pm400
#define NUMCHUNK 6
#define MAXPHYSMEM 0x6000000
#define MEMINCR 0x400000
#else
#define NUMCHUNK 16
#define MAXPHYSMEM 0x1000000
#define MEMINCR 0x100000
#endif
#define TOPOFSTACK 256*1024	/* 256KB */
#if pm200 || pm400
#define TSTLVL (*(char *)0x25)
#else
#define TSTLVL (*(char *)0x2025)
#endif
#endif

#define BASE    16      /* Default base for input numbers */

struct memerrors {
	long addr;
	long val;
	long val1;
	long val2;
	long badpar;
} _memsave[MAXERR];

#ifdef BOOT
int _end;
#endif

struct er_block _errptblk;

extern char *_regname[];
extern char *S_CR0;
extern unsigned _r[];

extern end;             /* End of bss */
long _exitval;		/* Return value to dos */
long _memerr;           /* Cumulative memory errors encountered */
#if pm400
char _memmap[512];       /* Bit map of available 4MB blocks */
char _badmemmap[512];    /* Bit map of bad 4MB blocks */
#else
long _memmap = 0;       /* Bit map of available 512K blocks (data area only) */
long _badmemmap = 0;    /* Bit map of bad 512K blocks (data area only) */
#endif
long _maxmem = 0;       /* Maximum size of memory (data area only) */
long _badmaxmem = 0;    /* Size of bad memory (data area only) */
long _topofstack = 0;   /* Top of stack for executing program (data area only) */
long _proctype;		/* Type of processor */
extern short _mapped;	/* Flag indicating whether we are mapped or not */
long _mempte[1];	/* memory 2nd level page table entries */
long _timerdisp;	/* memory test timer */

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
short _VERBOSE = 0;	/* print more diagnostics on startup */
short _bootdebug = 0;	/* print diagnostics while booting */
short _debug = 0;	/* print more diagnostics while booting */
short _loadsyms = 0;	/* load symbols with program */
#if pm400
int lflag, mflag;	/* flags for medium and long pattern tests */
long _loopcnt_sav;
long _counter;		/* running count of loops through the test */
long _loopcnt = 1;         /* number of passes through test */
#endif
long memcheck = 0;	/* to check user specified memory size */
char *arg0;

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

char *_version = "5.01  06/26/90"; /* String containing version of monitor */
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
#if pm100
#ifndef STAND
	"Boot       B  progname                    Load and execute",
	" requested program\n\r",
#endif
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

#if BOOT || SASH
main(argc,argv)
char *argv[];
{
	_main(argc,argv);
}
#endif

_main(argc,argv)
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
	int cache_mode = 1;

	arg0 = argv[0];
#if 0
    if (beenhere++ == 0) {
#endif
#if pm400
	for (i=1; i<argc; i++) {
		if (strcmp(argv[i], "-memcheck") == 0) {
			if (i != (argc-1)) {
				tmp = strtol(argv[i+1],NULL,0);
				if(tmp > 0 && tmp < 256) {
					memcheck = tmp;
				} else {
					_PRINTF("%s: -memcheck argument must be between 1 and 256\n", arg0);
					usage();
				}
			} else {
				_PRINTF("%s: -memcheck requires an argument\n", argv);
				usage();
			}
		} else if (argv[i][0] == '-') {
			switch(argv[i][1]) {
				case 'n':
					if (argv[i][2] == 'c')
						cache_mode = 0;
					break;
				case 'd':
					_bootdebug++;
					break;
				case 'D':
					_debug++;
					_VERBOSE++;
					break;
				case 'h':
					usage();
					exit(0);
				case 'l':
					_loopcnt = atol(argv[i+1]);
					if (_loopcnt < 0 || _loopcnt > 1000000) {
						_PRINTF("\n%s: bad value for loop count.\n", arg0);
						usage();
					}
					break;
				case 'p':
					if (argv[i][2] == '1')
						mflag++;
					if (argv[i][2] == '2')
						lflag++;
					break;
				case 'v':
					_verbose++;
					break;
				case 'V':
					_verbose++;
					_VERBOSE++;
					break;
				case 'q':
					_quiet++;
					break;
				case 'Q':
					_quiet++;
					_QUIET++;
					break;
				default:
					_PRINTF("%s: invalid argument.\n", arg0);
					usage();
			}
		}
	}
#else
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
			if (_cmdline[i+1] == 's') {
				_quiet++;
				_QUIET++;
			}
		}
	}
#endif /* pm400 */
#if 0
#endif

#if pm100
    _proctype = (*(char *)R_STAT & 03);
#endif
#if pm200
    _proctype = NS32332;
#endif
#if pm300
    _proctype = CLIPPER;
#endif
#if pm400
    _proctype = M88K;
#endif

#if pm400
	_loopcnt_sav = _loopcnt;
	_counter = 1;
	while (_loopcnt--) {
		if (_loopcnt_sav > 1) {
			_PRINTF("\nPass #%d of %d", _counter, _loopcnt_sav);
			_counter++;
		}
#endif

#if pm200 || pm300 || pm400
    if (!_quiet)
    	_PRINTF(_initstr);
    if (!_QUIET)
    	_PRINTF("Level 2 Test:   ");
#if pm400
    	_memerr = _mapmemtest(cache_mode);
#else
    	_memerr = _mapmemtest();
#endif
#endif
    if (!_QUIET)
	if (_memerr)
		_PRINTF("\b\bFAIL\n");
	else
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
	case M88K:
		_PRINTF("M88000  ");
		break;
	}
	if (((_maxmem/1024) % 1024) == 0) {
		_PRINTF("%dMB    ",_maxmem/(1024*1024));
		if (_maxmem < 10*1024*1024) 	/* 10MB */
			_PRINTF(" ");
	} else
		_PRINTF("%4dKB  ",_maxmem/1024);
	printcnt = 2;
#if pm100
	for (i=1; i<PCCMD->pc_devtab[0]; i++) {
#else
	for (i=0; i<COMMPAGE->dixsiz; i++) {
#endif
		static int dknum = 0;
		static int ttynum = 0;
		static int lpnum = 0;
		static int mtnum = 0;
		static int gnnum = 0;

		if (!(++printcnt % 9)) {
			_PRINTF("\n               ");
			printcnt = 1;
		}
#if pm100
		switch(PCCMD->pc_devtab[i]) {
#else
		switch(COMMPAGE->dix[i]) {
#endif
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
						_PRINTF("X11     ");
						break;
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
#if pm100 || pm400
	_memerrpr(_verbose);
#endif

#if pm400
	if (_memerr)
		exit(1);
	} /* while (_loopcnt--) */
#endif
    }

#ifdef BOOT
	_monitor();
#endif

#ifdef SASH
	_init();
	if (!_quiet)
		_PRINTF("\n");
#if 0
    }
#endif

	if (_getcmd(_cmdline,CMDSIZE)) {
		sash();        	/* ;; specified, so start sash */
		_getcmd(_cmdline,CMDSIZE);
	}
	else if ((_cmdline[0] == 0) && (notsashonly == 0)) {
		_syscmd(PC_DOSRTN,-1);	/* nothing specified, so just return */
	}
	while (_cmdline[0] != 0) {
		notsashonly++;
		_boot(_cmdline);
		if (_getcmd(_cmdline,CMDSIZE)) {
			sash();      /* if ;; call sash */
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
#if 1
			fgets (_line, sizeof (_line) - 1, stdin);
#else
			_gets(_line);
#endif
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
#if 1
		fgets (_line, sizeof (_line) - 1, stdin);
#else
		_gets(_line);
#endif
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
				_memerr = _ememtest(addr1,addr2,count);
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
			sash();			/* call sash */
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

#if pm400
	memset(_memmap, 0, sizeof(_memmap));
	memset(_badmemmap, 0, sizeof(_memmap));
#else
	_memmap = _maxmem = 0;
	_badmemmap = _badmaxmem = 0;
#endif

#if pm100
	for (i=NUMCHUNK; i>=1; i--)
		*(long *)(i*MEMINCR-4) = i | (i<<8);

	for (i=1; i<=NUMCHUNK; i++) {
		if (((*(long *)(i*MEMINCR-4) & 0xff) == i) ||
				(((*(long *)(i*MEMINCR-4) >> 8)&0xff) == i)) {
			_memmap |= 1 << (i-1);
			_maxmem += MEMINCR;
		}
	}
#endif
#if pm200 || pm300 || pm400
#if pm400
	_maxmem = swapl(COMMPAGE->mem_size);
#else
	_maxmem = COMMPAGE->mem_size;
#endif
	for (i=0; i<NUMCHUNK; i++)
		if (_maxmem > i*MEMINCR)
#if pm400
			_memmap[i] = 1;
#else
			_memmap |= 1 << i;
#endif
#endif

	/* Clear bogus dma and parity errors found while sizing memory */
	_errptblk.er_dma = _errptblk.er_par = 0;
#if STAND
	_topofstack = _maxmem;		/* if standalone, put stack at top */
	_topofstack = min(_maxmem,0x400000); /* limit stack to 4MB */
#else
	_topofstack = TOPOFSTACK;	/* if diagnostic, set much lower */
#endif
	return(_topofstack);
}

#if pm400
extern long _pagetab0[];
extern long _pagetables[];
extern long _tlpagetab[];
#endif

#if pm400
_mapmemtest(ca_mode)
int ca_mode;
#else
_mapmemtest()
#endif
{
	register long *temp1;
	register long temp2;
	register int i;
	register long errcnt = 0;
	register int parerrs;
	register int j;
	register char *base;
	register char *bound;
	long *pagetableptr;
	long modebits;

	_timerdisp = 0;

#if pm400 /* Only the first 4MB are mapped in, so map in the rest of memory */
	/*   up to a max of 32MB */
	/* round second level page tables up to 4k boundary */
	if (_VERBOSE)
		_PRINTF("\nmapping memory...");
	pagetableptr = (long *)(((long)_pagetables + 0xfff) & ~0xfff);
	for (i=1; i<8; i++)	/* top level page table */
		_tlpagetab[i] = (_tlpagetab[0] & 0xfff) |
			 (long)&pagetableptr[(i-1)*1024];
	modebits = _pagetab0[0x10] & 0xfff; /* steal mode bits from typ pg */
	for (i=0x400000; i<0x2000000; i+=0x1000)
		pagetableptr[(i-0x400000)/0x1000] = i | modebits;

	for (i = 0; i < 7 ; i++)
		_pagetab0[btoc(pagetableptr+(1024*i))] |= PG_CI;
	if (_VERBOSE)
		_PRINTF("done\n");

	if (COMMPAGE0->nmiflg == 2)
		_parity();

	if (memcheck) {
		if ((_maxmem/0x100000) != memcheck)
			_PRINTF("\n\nmemory check **ERROR**:\n");
		else
			_PRINTF("\n\nmemory check OK:\n");

			_PRINTF("user specified:\t%dMB \n", memcheck);
			_PRINTF("opmon reports:\t%dMB\n\n", _maxmem/0x100000);
		if ((_maxmem/0x100000) != memcheck)
			exit(1);

	}

	initcache();

	if (COMMPAGE0->nmiflg == 2)
		_parity();

#endif
	if (_VERBOSE)
		_PRINTF("zeroing memory (%dMB)...", (swapl(COMMPAGE->mem_size)/0x100000));
	else
		_PRINTF("\b\b00");

	bzero16(_topofstack,(swapl(COMMPAGE->mem_size) - _topofstack));
	if (COMMPAGE0->nmiflg == 2)
		_parity();
	if (_VERBOSE)
		_PRINTF("done\n");

	switch (ca_mode) {
		case 0:
			if (_VERBOSE)
				_PRINTF("caching ");
			cacheoff(0); /* data cache */
			cacheoff(1); /* instruction cache */
			if (COMMPAGE0->nmiflg == 2)
				_parity();
			if (_VERBOSE)
				_PRINTF("disabled\n");
			break;
		case 1:
		default:
			if (_VERBOSE)
				_PRINTF("caching ");
			cacheon(0); /* data cache */
			cacheon(1); /* instruction cache */
			if (COMMPAGE0->nmiflg == 2)
				_parity();
			if (_VERBOSE)
				_PRINTF("enabled\n");
			break;
	}
	if (_VERBOSE)
		_PRINTF("reading zeroed memory...");
	for (i=0; i<(MAXPHYSMEM/MEMINCR); i++) {
		if (_memmap[i]) {
			for (temp1=((long *)max(_topofstack,i*MEMINCR));temp1<(long *)((i+1)*MEMINCR); temp1++) { 
				if (temp2 = *temp1) {
					_PRINTF("\nbank %x:", i);
					_PRINTF("\nlocation 0x%8x: wrote 0x00000000, read 0x%8x\n", temp1, temp2);

					exit(1);
				}
			}
		}
	}
	if (_VERBOSE)
		_PRINTF("done\n");
		
	for (i=0; i<(MAXPHYSMEM/MEMINCR); i++) {
		if (_memmap[i]) {
			if (_VERBOSE) {
				_PRINTF("\n%x: ", i);
				if (_maxmem/MEMINCR >= i+8)
					_PRINTF("(Loc. 0x%x - 0x%x):\n", i*MEMINCR, ((i+8)*MEMINCR)-1);
				else
					_PRINTF("(Loc. 0x%x - 0x%x):\n", i*MEMINCR, _maxmem-1);
			}
			parerrs = _errptblk.er_par;
			if (TSTLVL & 2)		/* no memory test */
				break;
	    	    	errcnt += _memtest(errcnt,max(_topofstack,i*MEMINCR),(i+1)*MEMINCR,1);
#if pm400
			if (COMMPAGE0->nmiflg == 2)
				_parity();
#endif
			if (mflag || lflag || (TSTLVL & 4))		/* extended test 1 (patterns) */
	    	    		errcnt += _mmemtest(errcnt,max(_topofstack,i*MEMINCR),
					(i+1)*MEMINCR,1);
#if pm400
			if (COMMPAGE0->nmiflg == 2)
				_parity();
#endif
			if (lflag || (TSTLVL & 1))	{	/* extended test 2 (more patterns) */
	    	    		errcnt += _ememtest(errcnt,max(_topofstack,i*MEMINCR),
					(i+1)*MEMINCR,1);
#if pm400
				if (COMMPAGE0->nmiflg == 2)
					_parity();
#endif
			}
			if (errcnt || (parerrs != _errptblk.er_par)) {
#if pm400
				_memmap[i] = 0;
				_badmemmap[i] = 1;
#else
				_memmap &= ~(01<<i);
				_badmemmap |= (01 << i);
#endif
				_maxmem -= MEMINCR;
				_badmaxmem += MEMINCR;
			}
			if (parerrs != _errptblk.er_par)
				_errptblk.er_parbitmap |= (01 << i);
		}
	}
#if !pm400
	ZERO = _memmap;
#endif
	if (_debug && errcnt)
		_PRINTF("_mapmemtest: returning %d errors\n", errcnt);
	return(errcnt);
}
	
_memtest(errcnt,base,bound,repcnt)
long errcnt;
char *base;
register char *bound;
long repcnt;
{
	register long *i;
	register long modi;
	register long val1;
	long parerrs;
	long val2;
	long errs = 0;

	COUNT();
	while (repcnt--) {
		if (_VERBOSE)
			_PRINTF("writing addresses...");
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
#if pm400
		if (COMMPAGE0->nmiflg == 2)
			_parity();
#endif
		}
		if (_VERBOSE)
			_PRINTF("done, reading addresses...");
		for (i=(long *)base; i<(long *)bound; i++) {

			if ((val1 = *i) != (long)i) {
				notifypc();
#if pm400
				if (COMMPAGE0->nmiflg == 2)
					_parity();
#endif
				parerrs = _errptblk.er_par;
				val2 = *i;
				errs++;
				if ((_saverr(errcnt++,i,i,val1,val2,
					parerrs != _errptblk.er_par) != 0)
					&& !_verbose)
					return(errs);
			}
		}
		if (_VERBOSE)
			_PRINTF("done\n");

#if pm400
		COUNT();
#endif

		if (_VERBOSE)
			_PRINTF("writing ~addresses...");
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
/* DC */
#if pm400
		if (COMMPAGE0->nmiflg == 2)
			_parity();
#endif
		}
		if (_VERBOSE)
			_PRINTF("done, reading ~addresses...");

		for (i=(long *)base; i<(long *)bound; i++) {
			modi = ((long) i ^ 0xf7f7efef);
			if ((val1 = *i) != modi) {
				notifypc();
#if pm400
				if (COMMPAGE0->nmiflg == 2)
					_parity();
#endif
				parerrs = _errptblk.er_par;
				val2 = *i;
				errs++;
				if ((_saverr(errcnt++,i,modi,val1,val2,
					parerrs != _errptblk.er_par) != 0)
					&& !_verbose)
					return(errs);
			}
		}
		if (_VERBOSE)
			_PRINTF("done\n");
	}
	if (_debug && errs)
		_PRINTF("_memtest: returning %d errors\n", errs);
	return(errs);
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

_mmemtest(errcnt,base,bound,repcnt)
long errcnt;
char *base;
register char *bound;
long repcnt;
{
	register i;
	long errs = 0;
	long lerrs = 0;

	while (repcnt--) {
		errs = _memvaltest(errcnt,base,bound,repcnt,0L);
		lerrs += errs;
		errcnt += errs;
		errs = _memvaltest(errcnt,base,bound,repcnt,-1L);
		lerrs += errs;
		errcnt += errs;
		errs = _memvaltest(errcnt,base,bound,repcnt,0x55555555L);
		lerrs += errs;
		errcnt += errs;
		errs = _memvaltest(errcnt,base,bound,repcnt,0xaaaaaaaaL);
		lerrs += errs;
		errcnt += errs;
		errs = _memvaltest(errcnt,base,bound,repcnt,0xffff0000L);
		lerrs += errs;
		errcnt += errs;
		errs = _memvaltest(errcnt,base,bound,repcnt,0x0000ffffL);
		lerrs += errs;
		errcnt += errs;
		errs = _memvaltest(errcnt,base,bound,repcnt,0x5555aaaaL);
		lerrs += errs;
		errcnt += errs;
		errs = _memvaltest(errcnt,base,bound,repcnt,0xaaaa5555);
		lerrs += errs;
	}
	if (_debug && lerrs)
		_PRINTF("_mmemtest: returning %d errors\n", lerrs);
	return(lerrs);
}

_ememtest(errcnt,base,bound,repcnt)
long errcnt;
char *base;
register char *bound;
long repcnt;
{
	register i;
	long errs = 0;

	while (repcnt--) {
		for (i=0; i<16; i++)
			errs += _memvaltest(errcnt,base,bound,repcnt,
				(1<<i) | ((1<<i)<<16));
		for (i=0; i<16; i++)
			errs += _memvaltest(errcnt,base,bound,repcnt,
				~((1<<i) | ((1<<i)<<16)));
	}
	if (_debug && errs)
		_PRINTF("_ememtest: returning %d errors\n", errs);
	return(errs);
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
	int errs = 0;

	COUNT();
	if (_VERBOSE)
		_PRINTF("writing 0x%8x...", value);
	for (i=(long *)base; i<(long *)bound; i++) {
		*i = (long)value;
	}
#if pm400
	if (COMMPAGE0->nmiflg == 2)
		_parity();
#endif
	if (_VERBOSE)
		_PRINTF("done, reading 0x%8x...", value);
	for (i=(long *)base; i<(long *)bound; i++)
		if ((val1 = *i) != (long)value) {
			notifypc();
#if pm400
			if (COMMPAGE0->nmiflg == 2)
				_parity();
#endif
			parerrs = _errptblk.er_par;
			val2 = *i;
			errs++;
			if ((_saverr(errcnt++,i,value,val1,val2,
				parerrs != _errptblk.er_par) != 0) && !_verbose)
				return(errs);
		}
	if (_VERBOSE)
		_PRINTF("done\n");
	if (_debug && errs)
		_PRINTF("_memvaltest: returning %d errors\n", errs);
	return(errs);
}

_saverr(errcnt,addr,shouldbe,was1,was2,parity) {
	if (_debug)
		_PRINTF("\nsaverr: errcnt=%8x,addr=%8x,shouldbe=%8x,was1=%8x,was2=%8x,parity=%8x\n", errcnt,addr,shouldbe,was1,was2,parity);
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
#if pm400
		if (_counter)
			_PRINTF("\nPass #%d\n", _counter);
	    	_PRINTF("\nMEMORY TEST: %dMB PASSED, %dMB FAILED, %d ERRORS\n",
			_maxmem/0x100000,_badmaxmem/0x100000,_memerr);
#else
	    	_PRINTF("\nMEMORY TEST: %dKB PASSED, %dKB FAILED, %d ERRORS\n",
			_maxmem/1024,_badmaxmem/1024,_memerr);
#endif
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
#if pm400
			if (_memmap[i])
#else
			if ((_memmap>>i) & 01)
#endif
				_PRINTF("++++");
#if pm400
			else if (_badmemmap[i])
#else
			else if ((_badmemmap>>i) & 01)
#endif
				_PRINTF("%4x",_errptblk.er_bitmap[i]|
					_errptblk.er_bitmap[i]>>16);
			else 
				_PRINTF("....");
			if ((_errptblk.er_parbitmap>>i) & 01)
				_PRINTF("* ");
			else
				_PRINTF("  ");
		    }
		} else if (_proctype == M88K) {
#if 0 /* old code */
		    _PRINTF("       (4 MB) -->  ");
		    for (i=0; i < (MAXPHYSMEM / MEMINCR); i++) {
				if ((i % 16) == 0) {
					for (j=0; j < (MAXPHYSMEM / MEMINCR); j++) {
						if (i == 0)
							_PRINTF("       #%1x ",j);
						else
							_PRINTF("      #%2x ",j+i);
					}
					_PRINTF("\n                    ");
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
				if ((i % 6) == 0)
					_PRINTF("\n");
		    }
#else /* new code */
			_PRINTF("\n(4 MB per division)\n");
			i = 0;
			while (i < _maxmem/MEMINCR) {
#if 0
				if ((i % 8) == 0) {
					if (_maxmem/MEMINCR >= i+8)
						_PRINTF("(Loc. 0x%x - 0x%x):\n", i*MEMINCR, ((i+8)*MEMINCR)-1);
					else
						_PRINTF("(Loc. 0x%x - 0x%x):\n", i*MEMINCR, _maxmem-1);
				}
#endif
				for (j=0; j < 8; j++) {
					if (i+j < 10)
						_PRINTF("      #%1d  ",j+i);
					else
						_PRINTF("     #%1d  ",j+i);
					if (i+j+1 == _maxmem/MEMINCR)
						break;
				}
				_PRINTF("\n");
				for (j=0; j < 8; j++) {
					if (_memmap[i])
						_PRINTF("++++++++");
					else if (_badmemmap[i])
						_PRINTF("%8x",_errptblk.er_bitmap[i]);
					else 
						_PRINTF("........");
					if ((_errptblk.er_parbitmap>>i) & 03)
						_PRINTF("* ");
					else
						_PRINTF("  ");
					i++;
					if (i == _maxmem/MEMINCR)
						break;
				}
				_PRINTF("\n");
			}
#endif
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
#if pm400
			if (_memmap[i])
#else
			if ((_memmap>>i) & 03)
#endif
				_PRINTF("++++++++");
#if pm400
			else if (_badmemmap[i])
#else
			else if ((_badmemmap>>i) & 03)
#endif
				_PRINTF("%8x",_errptblk.er_bitmap[i]);
			else 
				_PRINTF("........");
			if ((_errptblk.er_parbitmap>>i) & 03)
				_PRINTF("* ");
			else
				_PRINTF("  ");
		    }
		}
	    	for (i=0; i<_memerr; i++) {
			_PRINTF("\n");
	    		if (i == MAXERR) {
	    			_PRINTF("    ...");
	    			break;
	        	}
			if (_proctype == NS32016)
				banksize = MEMINCR;
			else if (_proctype == M88K)
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

#if pm400
_parity() {
	_PRINTF("%s: parity error\n", arg0);
	_errptblk.er_par++;
	COMMPAGE->nmiflg = 0;
	*S_CR0 = CLR_CP;	/* clear coprocessor error status */
}
#endif

#if SASH && (pm200 || pm300 || pm400)
_init() {
}
#endif

#if pm400
usage()
{
	_PRINTF("\n%s (pm400):\n", arg0);
	_PRINTF("usage: %s -[hqDQV] [-l #] [-nc] [-p1|-p2] [-memcheck #]\n", arg0);
	_PRINTF("\noptions:\n");
	_PRINTF("\n-nc:	turn caching off [default: caching on]\n");
	_PRINTF("-h:	print usage message\n");
	_PRINTF("-l #:	loop # times [1-1000000]\n");
	_PRINTF("-p1:	run medium memory test\n");
	_PRINTF("-p2:	run long pattern test\n");
	_PRINTF("-q:	quiet mode\n");
	_PRINTF("-Q:	QUIET mode\n");
	_PRINTF("-V:	verbose mode\n");
	_PRINTF("-D:	debug mode\n");
	_PRINTF("-memcheck [number of mb]):	checks reported memory size\n");
	exit(1);
}
#endif
