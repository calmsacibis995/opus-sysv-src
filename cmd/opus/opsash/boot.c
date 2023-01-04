
#ifdef pm100
#define MMU_TYPE 32082
#define PAGESIZE NBPPT
#endif
#ifdef pm200
#define MMU_TYPE 32382
#define PAGESIZE NBPPT
#endif

#include "stand.h"
#include "sys/iorb.h"
#include "sys/commpage.h"
#include "sys/cp.h"
#include "a.out.h"
#ifndef pm400
#include "sys/page.h"
#include "sys/immu.h"
#endif

#ifdef SASH
#define _dopen open
#define _dlseek lseek
#define _dread read
#define _dclose close
#endif

#define max(a,b)	(a<b ? b : a)
#define min(a,b)	(a<b ? a : b)

#define ARGMAX	20
#define ARGLEN	256
#define BUFSIZE 1024
char _dmaslot;
char _zero;
int _memmap;
int _ptrtab;
int _loadsyms;
int _bootdebug;
int _quiet;

_boot(bootstr)
char *bootstr;
{
	register i;
	int argc;
	char *argv[ARGMAX];
	char bootname[ARGLEN];
	int arglen;

	for (arglen=0; arglen<ARGLEN; arglen++)
		if ((bootname[arglen] = bootstr[arglen]) == 0)
			break;

	if (!_quiet)
		_printf("$$ %s\n",bootname);
	argc = setargv(bootname,argv,ARGMAX);

	i = _dopen(argv[0],0);
#ifdef SASH
	if (i<0) {
		if (strncmp("dos",argv[0],3) == 0) {
			_pause(atoi(argv[1]));
			return (-1);
		} else if (*((short *)"-d") == *((short *)(argv[0])))
			_bootdebug ^= 1;
		else if (*((short *)"-s") == *((short *)(argv[0])))
			_loadsyms ^= 1;
		else
			_printf("opsash: %s: not found",argv[0]);
		_printf("\n");
		return(-1);
	}
#endif

	_copyunix(i,argc,argv,bootname,arglen);
}


_copyunix(io,argc,argv,bootname,arglen)
register io;
register argc;
register char *argv[];
register char *bootname;
register int arglen;
{
FILHDR fhdr;
SCNHDR hdr;
register int pageno;
register int physpage;
register int dpages, tpages;
int tsize, dsize, bsize;
int txtsiz, datsiz, bsssiz;
int tstart, dstart;
int taddr;
int nextpage;
int page;
int blpagetab;
long *blptrs;
int lastblpage;
int dmapagetab;
long *dmaptrs;
int tmpvadpage;
long *tmpvadptrs;
int tlpagetab;
long *tlptrs;
long *addr;
int stackpage;
unsigned long *argptr;
char *arglist;
char *strbase;
int reqcnt;
int i;
int cnt;
char tmpbuf[BUFSIZE];
int kernsyms;
int sympage;
int stringtab;
int stringtabsz;
int stringstor;
int curloc;
int kmemmap;
int symsize;

#if pm100
#ifndef SASH
if (_bootdebug) _printf("entering copyunix with fildes %x\n",io);
#endif

	_dlseek(io, 0L, 0);
	if ((cnt = _dread(io,&fhdr,sizeof(fhdr))) != sizeof(fhdr)) {
#ifndef SASH
if (_bootdebug) _printf("initial header read returned %d bytes\n",cnt);
#endif
		goto err;
	}

#ifndef SASH
if (_bootdebug) _printf("magic is %x\n",fhdr.f_magic);
#endif

	sympage = 0;
	if (fhdr.f_magic != NS32GMAGIC)
			goto err;

#ifndef SASH
if (_bootdebug) _printf("seeking to %x for text\n",fhdr.f_opthdr+sizeof(fhdr));
#endif
	if (readhdr(io,&fhdr,&hdr,".text") == 0) {
		tsize = hdr.s_size;
		tstart = hdr.s_scnptr;
		taddr = hdr.s_paddr;
	} else
		goto err;

#ifndef SASH
if (_bootdebug) _printf("tsize is %x, tstart is %x\n",tsize,tstart);
#endif

	if (readhdr(io,&fhdr,&hdr,".data") == 0) {
		dsize = hdr.s_size;
		dstart = hdr.s_scnptr;
	} else
		goto err;

#ifndef SASH
if (_bootdebug) _printf("dsize is %x, dstart is %x\n",dsize,dstart);
#endif

	if (readhdr(io,&fhdr,&hdr,".bss") == 0) {
		bsize = hdr.s_size;
	} else
		goto err;

	kernsyms = 0;		 /* find sizes of symbol table elements */
	if (_loadsyms && (fhdr.f_nsyms != 0)) {
		stringtabsz = 0;
		stringstor = fhdr.f_symptr + fhdr.f_nsyms * SYMESZ;
		_dlseek(io, stringstor, 0);
		_dread(io,&stringtabsz,sizeof(int));
		kernsyms = sizeof(fhdr) + fhdr.f_nsyms * SYMESZ +
			stringtabsz; 
	}

#ifndef SASH
if (_bootdebug) _printf("bsize is %x\n",bsize);
#endif

				/* determine number of text and data pages */
	tpages = ((tsize - 1) / PAGESIZE) + 1;
	dpages = ((dsize + bsize - 1) / PAGESIZE) + 1;

#ifndef SASH
if (_bootdebug) _printf("tpages is %x, dpages is %x\n",tpages,dpages);
#endif

				/* text, data, bss, stack, top level maps */
	nextpage = tpages + dpages;

	if (nextpage&1)		/* round up to 1k for top level page table */
		nextpage++;

				/* find location of top level pagetable */
	tlpagetab = nextpage;
	tlptrs = (long *)(TXTLOG + nextpage*PAGESIZE);
	nextpage += 2;

				/* find location of tmpvad page */
	tmpvadpage = nextpage;
	tmpvadptrs = (long *)(TXTLOG + nextpage*PAGESIZE);
	nextpage++;

				/* find location of dma page */
	dmapagetab = nextpage;
	dmaptrs = (long *)(TXTLOG + nextpage*PAGESIZE);
	nextpage++;

				/* find location of bottom level maps */
	blpagetab = nextpage;
	blptrs = (long *)(TXTLOG + nextpage*PAGESIZE);
	lastblpage = MINMEM/PAGESIZE;
	nextpage += lastblpage / NPG_PG;

	stackpage = nextpage;
	nextpage += KSTKSIZE;

	if (kernsyms) {
		sympage = nextpage;
		nextpage += kernsyms / PAGESIZE + 1;
	}
	
#ifndef SASH
if (_bootdebug)
_printf("tlptrs is %x, blptrs is %x, dmaptrs is %x\n",tlptrs,blptrs,dmaptrs);
#endif

					/* invalidate top level maps */
	for (i = 0; i < TLPAGES*PAGESIZE/sizeof(long *); i++)
		tlptrs[i] = 0;

	tlptrs[0] = _ptrtab | PG_P | PG_KW;	/* sash page 0 for return */
			
					/* set up top level map */
	for (i=0; i < lastblpage/NPG_PG; i++)
		tlptrs[taddr/TLPAGESIZE+i] = _getpaddr((blpagetab+i)*PAGESIZE,
			_memmap) | PG_P | PG_KW;

					/* set up tmpvad and dma map */
	tlptrs[TMPVAD/TLPAGESIZE] = _getpaddr(tmpvadpage*PAGESIZE,_memmap)
		| PG_P | PG_KW;
	tlptrs[DMAMEM/TLPAGESIZE] = _getpaddr(dmapagetab*PAGESIZE,_memmap)
		| PG_P | PG_KW;

					/* set up bottom level map */
	for (i=0; i < lastblpage; i++)
		blptrs[i] = _getpaddr(i * PAGESIZE,_memmap) | PG_P | PG_KW;

					/* clear bottom level tmpvad map */
	for (i=0; i < NPG_PG; i++)
		tmpvadptrs[i] = 0;

					/* clear up bottom level dma map */
	for (i=0; i < NPG_PG; i++)
		dmaptrs[i] = 0;

					/* set up bottom level dma map */
	for (i=0; i < 4; i++)
		dmaptrs[i] = _getpaddr(i * PAGESIZE,_memmap) | PG_P | PG_KW;

					/* set up bottom level special regs */
	dmaptrs[(R_WAIT-DMAMEM)/PAGESIZE] = PR_WAIT | PG_P | PG_KW;
	dmaptrs[(R_STAT-DMAMEM)/PAGESIZE] = PR_STAT | PG_P | PG_KW;
	dmaptrs[(R_C_ACK-DMAMEM)/PAGESIZE] = PR_C_ACK | PG_P | PG_KW;
	dmaptrs[(R_C_IRQ-DMAMEM)/PAGESIZE] = PR_C_IRQ | PG_P | PG_KW;
	dmaptrs[(NVILOC-DMAMEM)/PAGESIZE] = 0 | PG_P | PG_KW;

#ifndef SASH
if (_bootdebug) _printf("loading text\n");
#endif

	_dlseek(io,tstart,0);		/* load in text area */

	reqcnt = BUFSIZE - (tstart & (BUFSIZE-1));
	for (i=0; i<tsize; ) {
		if (_dread(io,tmpbuf,reqcnt) != reqcnt)
			goto err;
		_movb(tmpbuf,TXTLOG+i,reqcnt);
		i += reqcnt;
		reqcnt = min(tsize-i,BUFSIZE);
		_xonxoff();
	}

#ifndef SASH
if (_bootdebug) _printf("loading data\n");
#endif

	_dlseek(io,dstart,0);		/* load in data area */

	reqcnt = BUFSIZE - (dstart & (BUFSIZE-1));
	for (i = tpages*PAGESIZE; i < tpages*PAGESIZE + dsize; ) {
		if (_dread(io,tmpbuf,reqcnt) != reqcnt)
			goto err;
		_movb(tmpbuf,TXTLOG+i,reqcnt);
		i += reqcnt;
		reqcnt = min(dsize+tpages*PAGESIZE-i,BUFSIZE);
		_xonxoff();
	}

	if (kernsyms) {

#ifndef SASH
if (_bootdebug) _printf("loading symbol table\n");
#endif

		_dlseek(io,0,0);
		curloc = TXTLOG + sympage * PAGESIZE;
		if (_dread(io,tmpbuf,sizeof(fhdr)) != sizeof(fhdr))
			goto err;
		_movb(tmpbuf,curloc,sizeof(fhdr));

		curloc += sizeof(fhdr);
		_dlseek(io,fhdr.f_symptr,0);
		symsize = fhdr.f_nsyms * SYMESZ + stringtabsz;
		reqcnt = BUFSIZE - (fhdr.f_symptr & (BUFSIZE-1));
		for (i=0; i<symsize; ) {
			if (_dread(io,tmpbuf,reqcnt) != reqcnt)
				goto err;
			_movb(tmpbuf,curloc + i,reqcnt);
			i += reqcnt;
			reqcnt = min(BUFSIZE,symsize-i);
		}
	}

	addr = (long *)(TXTLOG + tpages*PAGESIZE + dsize);
	for (i=0; i<bsize>>2; i++)	/* clear bss area */
		*addr++ = 0;

					/* set up arguments for loaded pgm */
	argptr = (unsigned long *)(TXTLOG+(stackpage+KSTKSIZE)*PAGESIZE);
	strbase = (char *)(argptr - arglen/sizeof(char *) - 1);
	for (i=0; i<=arglen; i++)
		strbase[i] = bootname[i];
	argptr = (unsigned long *)strbase;
	*--argptr = 0;			/* last argument */
	for (i=argc; i>0; i--)
		*--argptr = (unsigned long)(argv[i-1] - bootname + strbase +
			taddr - TXTLOG);
	arglist = (char *)argptr;
	*--argptr = sympage ? taddr + sympage * PAGESIZE : 0;	/* symbols */
	*--argptr = (unsigned long) (arglist + taddr - TXTLOG);	/* argv */
	*--argptr = argc;				/* argc */
	kmemmap = 0;
	for (i=0; i<NUMCHUNK; i++)
		if ((_memmap >> i) & 1)
			kmemmap |= 0xf << (i*4);
	*--argptr = kmemmap;				/* memory map */
	*--argptr = _getpaddr(nextpage*PAGESIZE,_memmap)/PAGESIZE; /* nxt pg */

#ifndef SASH
if (_bootdebug)
_printf("stkptr is %x, memmap(%x), nextpage(%x), dmapagetab(%x)\n",
argptr,_memmap,nextpage,dmapagetab);

if (_bootdebug) {
_printf("_jptoit(%x,%x,%x)\n",taddr,((int)argptr)+taddr-TXTLOG,
	tlpagetab*PAGESIZE | PG_P | PG_KW);
}
#endif

	_dclose(io);
	_reinit();		/* _reinitialize device table */
	
	_jptoit(taddr,((int)argptr)+taddr-TXTLOG,_getpaddr(tlpagetab*PAGESIZE,
		_memmap)|PG_P|PG_KW);
	return(0);

err:
	_printf("opsash: %s: could not load\n",argv[0]);
	_dclose(io);
#endif
	return(-1);
}


readhdr(io,fhdr,hdr,secname)
register io;
register FILHDR *fhdr;
register SCNHDR *hdr;
register char *secname;
{
	register int i;

	_dlseek(io, fhdr->f_opthdr + sizeof (FILHDR), 0);
	for (i=0; i<fhdr->f_nscns; i++) {
		if (_dread(io,hdr,sizeof(SCNHDR)) != sizeof(SCNHDR))
			return(-1);
		if (strncmp(secname,hdr->s_name,strlen(secname)))
			continue;
		break;
	}
	if (i == fhdr->f_nscns)
		return(-1);
	else
		return(0);
}

setargv (argbuf,argv,argmax)
register char *argbuf;
char *argv[];
int argmax;
{
	register int i;

	for (i=0; i<argmax; i++) {
		while (*argbuf == ' ' || *argbuf == '\t')
			argbuf++;
		if (*argbuf == 0) {
			--i;
			break;
		}
		argv[i] = argbuf;
		while (*argbuf != ' ' && *argbuf != '\t' && *argbuf)
			argbuf++;
		if (*argbuf == 0)
			break;
		*argbuf++ = 0;
	}
	return(++i);
}
