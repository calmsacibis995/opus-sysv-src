/*	FMT.C
 *
 *	format a hard disk on XT or AT
 *	  or zero out block zero
 *	requires large-model compilation
 *
 *	usage:	fmt c [bad-track file] [interleave]
 *		fmt d - interleave
 *		fmt c zero
 *
 *	1.04	12/09/85	add zero option
 *	1.03	10/11/85	allow interleave specification
 *				change AT default interleave from 2 to 3
 *	1.02	 9/03/85	remove references to monlib.c
 *	1.01	 5/25/85	fix XT bad block logic
 *				sort bad track table
 *				seek before formatting XT bad tracks
 *	1.00	 5/22/85	initial version
 */

#define REV "1.04"

#define MAXBAD 4000	/* number of bad block entries */
#define XT_ILV 6	/* XT interleave */
#define AT_ILV 3	/* AT interleave */
#define TYP_AT 0xFC	/* AT system type code */
#define void int
#define CF 0x0001	/* carry flag */

#include <stdio.h>
#include <dos.h>
#include <ctype.h>

static FILE *cfp;
union REGS regs;
struct SREGS segs;

struct bad { unsigned c; unsigned h; };
static struct bad bad[MAXBAD];
static unsigned badcnt = 0;	/* number of bad entries */

struct fbuf { char f; char n; };
static struct fbuf fmtbuf[256];

static char buf[1024];
static char *buffer;

unsigned ptrseg(), ptroff();
static int num;
static unsigned drive;
static unsigned ncyls;
static unsigned nheads;
static unsigned nsects;
static unsigned lastcyl = 0;
static unsigned ilv = 0;
static int systyp = 0;	/* system type (AT, XT) */

main(argc, argv)
int argc;
char *argv[];
{
	int c, i;
	unsigned x;
	char *p;

	printf("fmt %s\n", REV);

	peek(0xffff, 0xe, &systyp, 1);	/* get system type from ffff:e */

	buffer = buf;
	x = (ptrseg(buffer) << 4) + ptroff(buffer);
	if (x > (x+512))
		buffer = buf+512;	/* get buffer that doesn't cross 64K */

	if (argc < 2 || argc > 4) {
		printf("usage: fmt <c|d> [<bad block file>|-] [interleave]\n");
		printf("       fmt <c|d> zero\n");
		exit(1);
	}
	if (*argv[1] == 'c' || *argv[1] == 'C')
		drive = 0;
	else if (*argv[1] == 'd' || *argv[1] == 'D')
		drive = 1;
	else {
		printf("usage: fmt <c|d> [<bad block file>]\n");
		printf("       fmt <c|d> zero\n");
		exit(1);
	}
	
	if (argc > 2 && *argv[2] != '-') {
		if (!strcmp(argv[2], "zero") || !strcmp(argv[2], "ZERO")) {
			zerozero();
			exit(0);
		}
		getbad(argv[2]);	/* read bad block file */
	}

	if (systyp == TYP_AT) {
		ilv = AT_ILV;
		printf("AT: ");
	} else {
		ilv = XT_ILV;
		printf("XT: ");
	}

	if (argc > 3) {			/* get interleave */
		p = argv[3];
		ilv = 0;
		while (*p >= '0' && *p <= '9') {
			ilv = (ilv * 10) + (*p - '0');
			++p;
		}
		if (ilv < 1 || ilv > 17) {
			printf("fmt: illegal interleave: %d\r\n", ilv);
			exit(1);
		}
	}	

	printf("Ready to format drive %s? [y/n] ", ((drive==0)?"C:":"D:"));
	if ((c = getchar()) != 'y' && c != 'Y')
		exit(1);
	putchar('\n');

	/* reset drive */
	regs.h.dl = drive | 0x80;
	regs.h.ah = 0;
	if (int86x(0x13, &regs, &regs, &segs) & CF) {
		printf("fmt: reset-drive failed\n");
		exit(1);
	}

	/* get size */
	regs.h.dl = drive | 0x80;
	regs.h.ah = 8;
	if (int86x(0x13, &regs, &regs, &segs) & CF) {
		printf("fmt: get-size failed\n");
		exit(1);
	}
	nheads = regs.h.dh + 1;
	nsects = regs.h.cl & 0x3F;
	ncyls = regs.h.cl & 0xC0;
	ncyls = (ncyls << 2) + regs.h.ch + 2;	/* add back spare cylinder */

	if (systyp == TYP_AT) {
		fmt_at();
	} else {
		fmt_xt();
	}

	/* done */
	printf("fmt: done\n");
	exit(0);
}

void
getbad(fname)
char *fname;
{
	int c, n;
	int i, j;

	if ((cfp = fopen(fname, "r")) == NULL) {
		printf("fmt: can't open input file %s\n", fname);
		exit(1);
	}

	while (1) {

		while ((c = getnum()) == '#') {
			putchar(c);
			do {	/* display comments */
				if ((c = getc(cfp)) == EOF)
					break;
				putchar(c);
			} while (c != '\n');
		}

		if (c == EOF)
			break;

		if (!isspace(c)) {
			printf("fmt: bad block file format error\n");
			exit(1);
		}
		bad[badcnt].c = num;
		c = getnum();
		bad[badcnt].h = num;
		++badcnt;

		while (c != ';')	/* skip sector range */
			c = getnum();

		if (badcnt >= MAXBAD) {
			printf("fmt: bad block table overflow\n");
			exit(1);
		}
	}

	/* sort table */
	if (badcnt>1) {
		for (i=0; i<(badcnt-1); ++i) {
			for (j=i+1; j<badcnt; ++j) {
				if (bad[i].c > bad[j].c ||
				   (bad[i].c == bad[j].c &&
				    bad[i].h > bad[j].h)) {
					unsigned temp;
					temp = bad[i].c;
					bad[i].c = bad[j].c;
					bad[j].c = temp;
					temp = bad[i].h;
					bad[i].h = bad[j].h;
					bad[j].h = temp;
				}
			}
		}
	}

	printf("%d bad track%s\n", badcnt, (badcnt==1)?"":"s");
}

/*	fmt_xt()
 *
 *	format drive - XT
 */
void
fmt_xt()
{
	int i;

	/* write sector buffer */
	regs.h.dl = drive | 0x80;
	regs.h.dh = 0;	/* head */
	regs.x.cx = 1;	/* cyl, sec */
	regs.h.al = 1;	/* sector count */
	regs.x.bx = ptroff(buffer);
	segs.es = ptrseg(buffer);
	regs.h.ah = 15;
	if (int86x(0x13, &regs, &regs, &segs) & CF) {
		printf("fmt: write-buffer failed\n");
		exit(1);
	}

	/* format drive */
	recal(0);		/* cylinder 0 */
	regs.h.dl = drive | 0x80;
	regs.h.dh = 0;		/* head */
	regs.x.cx = 1;		/* cyl 0, sector 1 */
	regs.h.al = ilv;	/* interleave */
	regs.h.ah = 7;		/* format drive */
	if (int86x(0x13, &regs, &regs, &segs) & CF) {
		printf("fmt: format drive failed\n");
		exit(1);
	}
	recal(0);

	/* format bad tracks */
	if (badcnt>0)
		printf("fmt: formatting bad tracks\n");
	for (i=0; i<badcnt; ++i) {
		if (bad[i].h >= nheads)
			continue;
		seek(bad[i].c, bad[i].h);
		regs.h.dl = drive | 0x80;
		regs.h.dh = bad[i].h;
		regs.h.ch = (char) bad[i].c;
		regs.h.cl = (char)(((bad[i].c >> 2) & 0xC0) + 1);
		regs.h.al = ilv;
		regs.h.ah = 6;
		if (int86x(0x13, &regs, &regs, &segs) & CF) {
			printf("fmt: format bad track failed\n");
			exit(1);
		}
	}
}


/*	fmt_at()
 *
 *	format drive - AT
 */
void
fmt_at()
{
	unsigned head, cyl, i, j;

	/* set up format buffer */
	for (i=0; i<nsects; ++i) {
		fmtbuf[i].f = 1;	/* flag entries as uninitialized */
	}
	j = 0;
	for (i=1; i<=nsects; ++i) {
		while (fmtbuf[j].f != 1) {	/* find uninitialized flag */
			if (++j >= nsects)
				j = 0;
		}
		fmtbuf[j].f = 0;	/* flag = good sector */
		fmtbuf[j].n = i;	/* n = logical sector number */
		j += ilv;		/* skip by interleave */
	}

	/* format drive */
	fmtblk(0, (nheads>8) ? 7 : nheads-1);
	if (nheads > 8)
		fmtblk(8, nheads-1);

	/* format bad tracks */
	if (badcnt > 0) {
		printf("fmt: formatting bad blocks\n");
		for (i=0; i<nsects; ++i)
			fmtbuf[i].f = 0x80;	/* flag = bad sector */
		fmtbad(0, (nheads>8) ? 7 : nheads-1);
		if (nheads > 8)
			fmtbad(8, nheads-1);
	}
}

/*	fmtbad(lo, hi)
 *
 *	format bad tracks between heads lo & hi
 */
void
fmtbad(lo, hi)
unsigned lo, hi;
{
	unsigned i, cyl, head;

	recal(lo);	/* recalibrate */
	for (i=0; i<badcnt; ++i) {
		cyl = bad[i].c;
		head = bad[i].h;
		if (head >= lo && head <= hi)
			fmttrk(cyl, head);
	}
}

/*	fmtblk(lo, hi)
 *
 *	format all tracks between heads lo & hi
 */
void
fmtblk(lo, hi)
unsigned lo, hi;
{
	int cyl, head;

	recal(lo);	/* recalibrate */

	/* format all cylinders from head=lo to hi */
	for (cyl = 0; cyl<ncyls; ++cyl) {
		for (head = lo; head<=hi; ++head)
			fmttrk(cyl, head);
	}
}

/*	fmttrk(cyl, head)
 *
 *	format track; seek if necessary
 */
void
fmttrk(cyl, head)
unsigned cyl, head;
{
	seek(cyl, head);	/* seek if necessary */

	/* format track */
	regs.h.dl = drive | 0x80;
	regs.h.dh = head;
	regs.h.ch = (char) cyl;
	regs.h.cl = (char) (((cyl >> 2) & 0xC0) + 1);
	regs.h.al = nsects;	/* sectors */
	regs.h.ah = 5;		/* format track */
	regs.x.bx = ptroff(fmtbuf);
	segs.es = ptrseg(fmtbuf);
	if (int86x(0x13, &regs, &regs, &segs) & CF) {
		printf("fmt: format track failed (%d/%d)\n", cyl, head);
		exit(1);
	}
}

/*	recal(head)
 *
 *	recalibrate
 */
void
recal(head)
unsigned head;
{
	regs.h.dl = drive | 0x80;
	regs.h.dh = head;
	regs.x.cx = 1;	/* cyl/sector */
	regs.h.ah = 17;	/* recalibrate */
	if (int86x(0x13, &regs, &regs, &segs) & CF) {
		printf("fmt: recalibrate failed\n");
		exit(1);
	}
	lastcyl = 0;
}

/*	seek(cyl, head)
 *
 *	seek to cyl if not already there
 */
void
seek(cyl, head)
unsigned cyl, head;
{
	if (cyl != lastcyl) {
		regs.h.dl = drive | 0x80;
		regs.h.dh = head;
		regs.h.ch = (char) cyl;
		regs.h.cl = (char) (((cyl >> 2) & 0xC0) + 1);
		regs.h.ah = 12;	/* seek */
		if (int86x(0x13, &regs, &regs, &segs) & CF) {
			printf("fmt: seek failed (%d)\n", cyl);
			exit(1);
		}
		lastcyl = cyl;
		printf("%4d\b\b\b\b", cyl);
	}
}

/*	getnum()
 *
 *	get number from bad-sector file
 *	return termination char or EOF
 *	put number in "num"
 */
int
getnum()
{
	int c;

	num = 0;
	while ((c = getc(cfp))==' ' || c=='\t' || c=='\n' || c=='\r')
		;

	if (c < '0' || c > '9')
		return(c);	/* could be EOF */

	do {
		num = num*10 + (c - '0');
	} while ((c = getc(cfp)) >= '0' && c <= '9');

	return(c);
}

/*	ptrseg(p)
 *	ptroff(p)
 *	char *p;
 *
 *	return segment or offset portion of pointer
 */
unsigned
ptrseg(o, s)
unsigned o, s;
{
	return(s);	/* return segment portion */
}

unsigned
ptroff(o, s)
unsigned o, s;
{
	return(o);	/* return offset portion */
}

/*	zerozero()
 *
 *	zero out block zero of specified drive
 */
void
zerozero()
{
	int c, i;

	printf("Ready to zero drive %s? [y/n] ", ((drive==0)?"C:":"D:"));
	if ((c = getchar()) != 'y' && c != 'Y')
		exit(1);
	putchar('\n');

	/* reset drive */
	regs.h.dl = drive | 0x80;
	regs.h.ah = 0;
	if (int86x(0x13, &regs, &regs, &segs) & CF) {
		printf("fmt: reset-drive failed\n");
		exit(1);
	}

	/* clear buffer */
	for (i=0; i<512; ++i)
		buffer[i] = 0;

	/* write block 0 */
	recal(0);		/* cylinder 0 */
	regs.h.dl = drive | 0x80;
	regs.h.dh = 0;	/* head */
	regs.x.cx = 1;	/* cyl, sec */
	regs.h.al = 1;	/* sector count */
	regs.x.bx = ptroff(buffer);
	segs.es = ptrseg(buffer);
	regs.h.ah = 3;
	if (int86x(0x13, &regs, &regs, &segs) & CF) {
		printf("fmt: write failed\n");
		exit(1);
	}

	/* done */
	printf("fmt: done\n");
	exit(0);
}
