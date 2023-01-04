/*	cfg.c
**
**	display PM configuration and serial number
**
**	chown root cfg; chmod 4755 cfg
**
**	29 MAR 89  1.00  adapted from cpt.c and nvr.c
*/

#define VERSION "1.00"

#include <stdio.h>

struct cpt {
	int method;
	long *key;
	long keysize;
	long *buf;
	long bufsize;
};

#ifdef clipper
#define MEM_SIZE 0x2038	/* offset in comm page */
#else
#define MEM_SIZE 0x38	/* offset in comm page */
#endif

int kmem, cpt;

void
main(argc, argv)
int argc;
char *argv[];
{
	struct cpt ctl;
	long sn;
	char *argp;
	int i;
	char *cpname;
	char *prefix;
	union {
		long l;
		unsigned char c[4];
	} memsize;
	int swap;
	int arch_io = 1;

	if ((kmem = open("/dev/kmem", 2)) == -1) {
		perror("open(/dev/kmem)");
		exit(1);
	}
	if ((cpt = open("/dev/cpt", 2)) == -1) {
		perror("open(/dev/cpt)");
		exit(1);
	}

	prefix = "";
	for (i=1; i<argc; ++i) {
		if (argv[i][0] == '-') {
			argp = &argv[i][2];
			switch (argv[i][1]) {
			case 'v':	/* version */
#if ns32000
				fprintf(stderr, "100/200PM");
#endif
#if clipper
				fprintf(stderr, "300PM");
#endif
#if m88k
				fprintf(stderr, "400PM");
#endif
				fprintf(stderr, " cfg %s\n",VERSION);
				break;

			default:
				fprintf(stderr, "unknown option %s\n", argv[i]);
				goto usage;
			}
		} else {
			fprintf(stderr, "unknown argument %s\n", argv[i]);
			usage:
			fprintf(stderr, "usage: %s [-v]\n", argv[0]);
			exit(1);
		}
	}

	ctl.method = 1;		/* get serial number */
	ctl.buf = &sn;

	if (ioctl(cpt, 0, &ctl) == -1) {
		perror("ioctl(cpt)");
		exit(1);
	}
#if ns32000
	if (sn == 0xb2b2) {
		cpname = "108";
		prefix = "A";
		arch_io = 0;
	} else if (sn < 100000) {
		cpname = "108";
		prefix = "A";
		arch_io = 0;
	} else if (sn < 200000) {
		cpname = "110";
		prefix = "C";
		arch_io = 0;
	} else if (sn < 300000) {
		cpname = "220";
		prefix = "J";
	} else {
		cpname = "200";
		prefix = "M";
	}
#endif
#if clipper
	cpname = "300";
	prefix = "F";
#endif
#if m88k
	cpname = "400";
	prefix = "R";
#endif
#if m88k	/* bigendian */
	memsize.l = geti(MEM_SIZE);
	swap = memsize.c[0];
	memsize.c[0] = memsize.c[3];
	memsize.c[3] = swap;
	swap = memsize.c[1];
	memsize.c[1] = memsize.c[2];
	memsize.c[2] = swap;
#else
	if (arch_io) {
		memsize.l = geti(MEM_SIZE);
	}
#endif
	if (arch_io) {
		fprintf(stderr, "%sPM-%d  S/N %s%d",
			cpname,
			memsize.l/(1024*1024),
			prefix, sn % 100000);
		if (sn >= 100000)
			fprintf(stderr, "  [%d]", sn);
	} else {
		fprintf(stderr, "%sPM  S/N ", cpname);
		if (sn == 0xb2b2) {
			fprintf(stderr, "n/a");
		} else {
			fprintf(stderr, "%s%d\n", prefix, sn % 100000);
			if (sn >= 100000)
				fprintf(stderr, "  [%d]", sn);
		}
	}
	fprintf(stderr, "\n");
	exit(0);
}

int
geti(loc)
{
	int val;

	if (lseek(kmem, loc, 0) != loc) {
		perror("lseek(kmem)");
		exit(1);
	}
	if (read(kmem, &val, sizeof(val)) != sizeof(val)) {
		perror("read(kmem)");
		exit(1);
	}
	return(val);
}
