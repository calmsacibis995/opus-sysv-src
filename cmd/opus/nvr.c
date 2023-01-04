/*	nvr.c
**
**	provide access to 9306 nonvolatile ram for testing
**
**	2.01  30 MAR 89	fix for bigendian machines
**	2.00  29 MAR 89	make command-line driven; noninteractive
**	1.10  20 MAR 89	add arch_io && UNIX support (except clipper)
**	1.04  28 SEP 88	remove memory-based clipper
**			add -p (port) option, usage message
**	23 APR 87	fix broken arch_100 logic
**	23 NOV 86	add Milo logic
**	24 JUN 85	initial version
**
**	cc -onvr [-Darch_100] [-DMSDOS] nvr.c
**	MSDOS && arch_100 not supported
*/

#define VERSION "2.01"

#if !arch_io && !arch_100
#define arch_io 1		/* default to arch_io */
#endif

#if !MSDOS && !UNIX
#define UNIX 1			/* default to UNIX */
#endif

#if m88k
#define BIGEND 1
#else
#define BIGEND 0
#endif

#include <stdio.h>
long strtol();
long lseek();

#define LDELAY 10000L	/* long delay */
#define SDELAY 100L	/* long delay */

#if arch_100
#define WBIT 0xfff000L	/* kernel virtual address of write register */
#define STLO 0xfff200L	/* kernel virtual address of status reg low byte */
#define STHI 0xfff201L	/* kernel virtual address of status reg high byte */
#define RD 0x04		/* bit mask of DO bit in status register */
#define STRD STHI	/* status register containing RD */
#define N32 0x01	/* bit mask of Opus32.32 bit in status register */

#define CS (WBIT+0)	/* chip select */
#define SK (WBIT+4)	/* clock */
#define WD (WBIT+8)	/* data in */
#endif

#if MSDOS && arch_io
#define SR0	8
#define SR1	9
#define SR2	10
#define SR3	11
#define CR0	12
#endif	/* MSDOS && arch_io */

#if UNIX && arch_io

#if clipper
#define MEM_SIZE 0x2038		/* in comm page */
#else
#define MEM_SIZE 0x38		/* in comm page */
#endif

#define CP_TYPE  0x3
#define CP_TYPEX 0xf
#define CP_X	 0x4

#if m88k
#define SR0	0x5fffff20
#define SR1	0x5fffff24
#define SR2	0x5fffff28
#define SR3	0x5fffff2c
#define CR0	0x5fffff30
#endif	/* m88k */

#if ns32000
#define SR0	0x5ffffff8
#define SR1	0x5ffffff9
#define SR2	0x5ffffffa
#define SR3	0x5ffffffb
#define CR0	0x5ffffffc
#endif	/* ns32000 */

#endif	/* UNIX && arch_io */

#if arch_io
#define RD	0x01		/* RD bit in SR0 */
#define STRD	SR0
#define SET	1
#define CLEAR	0
#define CS	0x18
#define SK	0x1a
#define WD	0x1c
#endif

/*	novram commands
*/
#define READ		0x18	/* read word */
#define WRITE		0x14	/* write word */
#define ERASE		0x1c	/* erase word */
#define EWEN		0x13	/* enable write/erase */
#define EWDS		0x10	/* disable write/erase */
#define ERASE_ALL	0x12	/* erase all */
#define WRITE_ALL	0x11	/* write all */

#if UNIX
int kmem;
#endif

#if MSDOS && arch_io
unsigned cpio = 0xf90;
#endif

void e_all(), erase(), ewen(), ewds(), send(), send1(), put1();
void wd(), cs(), sk(), delay();
void nvwrite();
unsigned nvread();
#if MSDOS
unsigned char *makeptr();
#endif

void
usage()
{
	fprintf(stderr, "\n");
	fprintf(stderr, "Widget Tester %s: Options:\n", VERSION);
	fprintf(stderr, "\n");
	fprintf(stderr, "-v             print version\n");
	fprintf(stderr, "-c             print configuration\n");
	fprintf(stderr, "-s             print serial number\n");
#if MSDOS && arch_io
	fprintf(stderr, "-p0xf90        specify I/O port\n");
#endif
	fprintf(stderr, "-R             read entire NVR\n");
	fprintf(stderr, "-E             erase entire NVR to -1\n");
	fprintf(stderr, "-e<loc>        erase word to -1\n");
	fprintf(stderr, "-r<loc>        read word\n");
	fprintf(stderr, "-w<loc> <val>  write word\n");
	fprintf(stderr, "-S<sn>         write serial number\n");
	fprintf(stderr, "\n");
	exit(1);
}

void
main(argc, argv)
int argc;
char *argv[];
{
	char *argp;
	unsigned long addr, data;
	int i;
	unsigned sn, csum;

#if UNIX
	if ((kmem = open("/dev/kmem", 2)) == -1) {
		perror("open(/dev/kmem)");
		exit(1);
	}
#endif
	if (argc == 1)
		usage();
	for (i=1; i<argc; ++i) {
		if (argv[i][0] == '-') {
			argp = &argv[i][2];
			switch (argv[i][1]) {
			case 'v':	/* version */
#if arch_100
				fprintf(stderr, "110PM");
#endif
#if arch_io
#if UNIX
#if ns32000
				fprintf(stderr, "200PM");
#endif
#if clipper
				fprintf(stderr, "300PM");
#endif
#if m88k
				fprintf(stderr, "400PM");
#endif
#else
				fprintf(stderr, "200/300/400PM");
#endif
#endif
				fprintf(stderr, " Widget Tester  %s\n",VERSION);
				break;

			case 'c':
				exit(showconf());

			case 's':
				exit(getsn(1));

#if MSDOS && arch_io
			case 'p':
				if (argp[0] == '\0')
					argp = argv[++i];
				cpio = strtol(argp, NULL, 0);
				if (cpio & 0xf00f) {
					fprintf(stderr, "invalid port 0x%x\n", cpio);
					exit(1);
				}
				break;
#endif	/* MSDOS && arch_io */

			case 'r':	/* read word */
				if (argp[0] == '\0')
					argp = argv[++i];
				addr = strtol(argp, NULL, 0);
				fprintf(stdout, "0x%04x\n",
					nvread((unsigned)addr));
				break;

			case 'R':	/* read all */
				for (addr=0; addr<16; ++addr) {
					fprintf(stdout, "%2x: 0x%04x\n",
						(unsigned)addr, nvread((unsigned)addr));
				}
				break;

			case 'S':	/* write serial number */
				if (argp[0] == '\0')
					argp = argv[++i];
				data = strtol(argp, NULL, 0);
				if (data > 65535) {
					fprintf(stderr, "S/N must be <= 65535\n");
					break;
				}
				sn = (unsigned)data;
				csum = 0xffb2;
				csum ^= sn & 0xff00;
				csum ^= (sn << 8) & 0xff00;
				nvwrite(0, csum);
				nvwrite(1, sn);
				break;

			case 'w':	/* write word */
				if (argp[0] == '\0')
					argp = argv[++i];
				addr = strtol(argp, NULL, 0);
				argp = argv[++i];
				data = strtol(argp, NULL, 0);
				nvwrite((unsigned)addr, (unsigned)data);
				break;

			case 'e':	/* erase word */
				if (argp[0] == '\0')
					argp = argv[++i];
				addr = strtol(argp, NULL, 0);
				erase((unsigned)addr);
				break;

			case 'E':	/* erase all */
				e_all();
				break;

			default:
				fprintf(stderr, "unknown option %s\n", argv[i]);
				usage();
			}
		} else {
			fprintf(stderr, "unknown argument %s\n", argv[i]);
			usage();
		}
	}
	exit(0);
}

int
showconf()
{
#if UNIX && arch_io
	int cptype;
	union {
		long l;
		unsigned char c[4];
	} memsize;
	char *cpname;

	cptype = get1(SR1) & CP_TYPE;
	switch (cptype) {
	case 0:
		cpname = "300";
		break;
	case 1:
		cpname = "220";
		break;
	case 2:
		cpname = "200";
		break;
	case 3:
		cptype = (get1(SR3) & CP_TYPEX) + CP_X;
		switch (cptype) {
		case 4:
			cpname = "400";
			break;
		default:
			cpname = "???";
			break;
		}
	}
#if BIGEND
	memsize.c[3] = get1(MEM_SIZE);
	memsize.c[2] = get1(MEM_SIZE+1);
	memsize.c[1] = get1(MEM_SIZE+2);
	memsize.c[0] = get1(MEM_SIZE+3);
#else
	memsize.c[0] = get1(MEM_SIZE);
	memsize.c[1] = get1(MEM_SIZE+1);
	memsize.c[2] = get1(MEM_SIZE+2);
	memsize.c[3] = get1(MEM_SIZE+3);
#endif
	fprintf(stderr, "%sPM-%d ", cpname, memsize.l/(1024*1024));
#endif
	return(getsn(0));
}

int
getsn(flag)
{
	union {
		unsigned long l;
		unsigned short s[2];
		struct sn {
#if BIGEND
			unsigned char csum;
			unsigned char b0;
			unsigned char b3;
			unsigned char b2;
#else
			unsigned char b0;
			unsigned char csum;
			unsigned char b2;
			unsigned char b3;
#endif
		} sn;
	} sn;
		
	sn.s[0] = nvread(0);
	sn.s[1] = nvread(1);
	if (sn.l != 0xffffffffL && sn.sn.csum == (sn.sn.b2 ^ sn.sn.b3 ^ 0xff)) {
		if (flag)
			fprintf(stdout, "%d\n", (sn.sn.b3 << 8) + sn.sn.b2);
		else
			fprintf(stderr, "S/N %d\n", (sn.sn.b3<<8) + sn.sn.b2);
		return(0);
	} else {
		if (flag)
			fprintf(stderr, "invalid\n");
		else
			fprintf(stderr, "S/N: invalid checksum\n");
		return(1);
	}
}

void
e_all()
{
	ewen();
	cs(1);
	send(ERASE_ALL, 5);
	send(0, 4);
	cs(0);
	delay(LDELAY);
	ewds();
}

void
erase(addr)
unsigned addr;
{
	ewen();
	cs(1);
	send(ERASE, 5);
	send(addr, 4);
	cs(0);
	delay(LDELAY);
	ewds();
}

void
nvwrite(addr, data)
unsigned addr, data;
{
	ewen();
	cs(1);
	send(WRITE, 5);
	send(addr, 4);
	send(data, 16);
	cs(0);
	delay(LDELAY);
	ewds();
}

unsigned
nvread(addr)
unsigned addr;
{
	int i;
	int data = 0;

	cs(1);
	send(READ, 5);
	send(addr, 4);
	for (i=15; i>=0; --i) {
		sk(1);
		data |= rd() << i;
		sk(0);
	}
	cs(0);
	return(data);
}

void
ewen()
{
	cs(1);
	send(EWEN, 5);
	send(0, 4);
	cs(0);
}

void
ewds()
{
	cs(1);
	send(EWDS, 5);
	send(0, 4);
	cs(0);
}

void
send(val, n)
int val, n;
{
	while(n--) {
		send1((val >> n) & 1);
	}
}

void
send1(val)
int val;
{
	wd(val);
	sk(1);
	sk(0);
}

void
wd(val)
int val;
{
	static int lastval = -1;

	if (val == lastval)
		return;
	lastval = val;
#if arch_io
	put1(WD | (val ^ 1), CR0);
#else
	put1(val, WD);
#endif
}

int
rd()
{
#ifdef arch_100
	if (get1(STRD) & RD)
		return(1);
	return(0);
#else
	if (get1(STRD) & RD)
		return(0);
	return(1);
#endif
}

void
cs(val)
int val;
{
#if arch_io
	put1(CS | val, CR0);
#else
	put1(val, CS);
#endif
	sk(1);
	sk(0);
	if (val == 0)
		wd(0);
}

void
sk(val)
int val;
{
#if arch_io
	put1(SK | val, CR0);
#else
	put1(val, SK);
#endif
}

void
delay(cnt)
unsigned long cnt;
{
	unsigned long i;

	i = 0;
	while (i < cnt)
		++i;
}

void
put1(val, loc)
{
#if UNIX
	unsigned char cval;

	cval = val;
	lseek(kmem, loc, 0);
	write(kmem, &cval, 1);
#endif
#if MSDOS && arch_io
	outp(cpio + loc, val);
	delay(SDELAY);
#endif
}

int
get1(loc)
{
	unsigned char val;

#if UNIX
	lseek(kmem, loc, 0);
	read(kmem, &val, 1);
#endif
#if MSDOS && arch_io
	val = inp(cpio + loc);
	delay(SDELAY);
#endif
	return((int)val);
}

#if MSDOS
unsigned char *
makeptr(p)
unsigned char *p;
{
	return(p);
}
#endif
