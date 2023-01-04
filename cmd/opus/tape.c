/* %Q% %W% */

/*
 * Tape utility program
 *
 * tape [-f <file name>] [-b <bufsize>] [-n] command argument ...
 *
 */

#include <stdio.h>
#include <sys/mtcomm.h>
#include <sys/dkcomm.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

char *fname = "/dev/rmt/0mn";	/* default tape device for ioctl ops */
char *buffname = "/dev/rmt/0m";	/* default tape device for buffering ops */
int bufsize = (256 * 1024);	/* default buffer size for buffering ops */

char *buf;			/* buffer for buffering ops */
int fn = -1;			/* tape file number */
int silent;			/* true to suppress messages */
int fflg;			/* true if tape file name supplied */
int cnt;			/* buffer fill count */
int cmdcnt;			/* number of times to do the command */
int norewind;		/* use fname for buffering ops */
extern int errno;		/* kernel error number */
extern char *sys_errlist[];	/* printable strings for error numbers */

#define BUFOUT 4000 		/* fake ioctl code for bufout command */
#define BUFIN 4001		/* fake ioctl code for bufin command */

struct cmds {			/* command descriptions */
	char *name;
	int ioctlcode;
} cmds[] = {
	"fsf", MTIOCTL_FSF,
	"bsf", MTIOCTL_BSF,
	"fsr", MTIOCTL_FSR,
	"bsr", MTIOCTL_BSR,
	"rew", MTIOCTL_REW,
	"reset", MTIOCTL_RESET,
	"status", MTIOCTL_NOP,
	"retension", MTIOCTL_TENS,
	"erase", MTIOCTL_ERASE,
	"density", MTIOCTL_DEN,
	"wfm", MTIOCTL_WFM,
	"bufout", BUFOUT,
	"write", BUFOUT,
	"bufin", BUFIN,
	"read", BUFIN,
	"unload", MTIOCTL_UNL,
	"", 0,
};

char *usagemsg[] = {
	"usage: tape [-s] [-f <fname>] [-b <bufsize>] <command>\n",
	"	commands:\n",
	"		fsf <count>\n",
	"		bsf <count>\n",
	"		fsr <count>\n",
	"		bsr <count>\n",
	"		rew\n",
	"		reset\n",
	"		status\n",
	"		retension\n",
	"		erase\n",
	"		density <low/high>\n",
	"		wfm\n",
	"		bufout\n",
	"		write\n",
	"		bufin\n",
	"		read\n",
	"		unload\n",
	NULL
};

main(argc, argv)
int argc;
char **argv;
{
	int c;
	extern char *optarg;
	extern int optind;
	struct cmds *p;
	int parm;
	int err;
	int len;
	int index;
	int eof;
	struct stat statbuf;
	int fh;
	int isfifo;
	struct diskcommand dkcommand;

	while ((c = getopt(argc, argv, "f:b:sn")) != EOF) {
		switch (c) {

		case 'n':
			norewind++;
			break;

		case 's':
			silent++;
			break;

		case 'f':
			fname = optarg;
			fflg++;
			break;

		case 'b':
			bufsize = atoi(optarg);
			if (optarg[strlen(optarg) - 1] == 'w' ||
				optarg[strlen(optarg) - 1] == 'W') {
				bufsize *= 2;
			}
			if (optarg[strlen(optarg) - 1] == 'b' ||
				optarg[strlen(optarg) - 1] == 'B') {
				bufsize *= 512;
			}
			if (optarg[strlen(optarg) - 1] == 'k' ||
				optarg[strlen(optarg) - 1] == 'K') {
				bufsize *= 1024;
			}
			break;

		case '?':
			usage();
			break;
		}
	}
	for (p = cmds; strcmp(p->name, argv[optind]) != 0; p++) {
		if (strlen(p->name) == 0) {
			usage();
		}
	}
	optind++;
	parm = 0;
	cmdcnt = 1;
	switch (p->ioctlcode) {

	case BUFOUT:
		if (!fflg && !norewind) {
			fname = buffname;
		}
		opentape(O_WRONLY);
		if ((buf = (char *)malloc(bufsize)) == NULL) {
			error("can't allocate memory for buffer\n");
			exit(2);
		}
		fstat(0, &statbuf);
		isfifo = (statbuf.st_mode & S_IFMT) == S_IFIFO;
		while (1) {
			for (cnt = 0; cnt < bufsize; cnt += len) {
				len = read(0, &buf[cnt], bufsize - cnt);
				if (len < 0) {
					error("error reading stdin (%s)\n",
					      sys_errlist[errno]);
					closetape();
					exit(2);
				} else if (len == 0) {
					break;
				}
			}
			if (isfifo)
				sleep(2);	/* allow pipe transmitter to refill pipe */
			writetape();
			if (len == 0) {
				break;
			}
		}
		closetape();
		break;

	case BUFIN:
		if (!fflg && !norewind) {
			fname = buffname;
		}
		fh = open("/dev/rdsk/0s0", O_RDONLY);
		opentape(O_RDONLY);
		if ((buf = (char *)malloc(bufsize)) == NULL) {
			error("can't allocate memory for buffer\n");
			exit(2);
		}
		fstat(1, &statbuf);
		isfifo = (statbuf.st_mode & S_IFMT) == S_IFIFO;
		sleep(1);
		eof = 0;
		while (!eof) {
			eof = readtape();
			for (index = 0; index < cnt; index += len) {
				len = write(1, &buf[index], cnt - index);
				if (len < 0) {
					error("error writing stdout (%s)\n",
					      sys_errlist[errno]);
					closetape();
					exit(2);
				}
			}

			if (isfifo) do {	/* wait for pipe to empty */
				fstat(1, &statbuf);
			} while (statbuf.st_size);
			sync();					/* insure all is in disk queue */
			dkcommand.drive = 0;	/* wait for disk queue to complete */
			if (fh > 0) {
				if (ioctl(fh, DKDRAINDISK, &dkcommand) == -1) {
					error("error in ioctl \n", sys_errlist[errno]);
					closetape();
					exit(2);
				}
			}
		}
		closetape();
		if (fh > 0)
			close(fh);
		break;

	case MTIOCTL_FSF:
	case MTIOCTL_BSF:
		if (argc > optind) {
			cmdcnt = atoi(argv[optind]);
		}
		goto docmd;
		break;

	case MTIOCTL_FSR:
	case MTIOCTL_BSR:
	case MTIOCTL_DEN:
		if (argc > optind) {
			parm = atoi(argv[optind]);
		} else {
			parm = 1;
		}
		goto docmd;
		break;

	default:
	docmd:
		opentape(O_RDONLY);
		while (cmdcnt-- > 0) {
			err = ioctl(fn, p->ioctlcode, &parm);
			if (err && errno != EIO) {
				error("ioctl failed (%s)\n", sys_errlist[errno]);
				exit(2);
			}
		}
		if (!silent) {
			printstatus(parm);
		}
		closetape();
		break;
	}
}

usage()
{
	char **p;

	for (p = usagemsg; *p != NULL; p++) {
		fprintf(stderr, *p);
	}
	exit(2);
}

opentape(flag)
int flag;
{

	if ((fn = open(fname, flag)) < 0) {
		error("open failed (%s)\n", sys_errlist[errno]);
		exit(2);
	}
}

closetape()
{

	close(fn);
}

error(fmt, a, b, c, d, e, f)
char *fmt;
int a, b, c, d, e, f;
{

	if (!silent) {
		fprintf(stderr, "tape: ");
		fprintf(stderr, fmt, a, b, c, d, e, f);
	}
}

writetape()
{
	int len;
	int index;

	for (index = 0; index < cnt; index += len) {
		len = write(fn, &buf[index], min(bufsize, cnt - index));
		if (len < 0) {
			error("error writing tape (%s) \n", sys_errlist[errno]);
			exit(2);
		}
	}
}

/*
 * Reads data from tape into buf. cnt is set to the amount read
 * in. If there is an error then this routine aborts the program
 * If there is an eof then the routine returns 1 else it returns
 * 0. Note that it may return 1 with cnt non zero.
 */
int
readtape()
{
	int len;

	for (cnt = 0; cnt < bufsize; cnt += len) {
		len = read(fn, &buf[cnt], min(bufsize, bufsize - cnt));
		if (len < 0) {
			error("error reading tape (%s) \n", sys_errlist[errno]);
			exit(2);
		} else if (len == 0) {
			return(1);
		}
	}
	return(0);
}


min(a, b)
{
	
	return((a > b) ? b : a);
}

printstatus(i)
register i;
{
	if ( !i )
		return;

	if ( i & S_WPROT )
		fprintf(stderr, "nowrite ");
	if ( i & S_OFFLINE )
		fprintf(stderr, "offline ");
	if ( i & S_BOT )
		fprintf(stderr, "bot " );
	if ( i & S_EOT )
		fprintf(stderr, "eot ");
	if ( i & S_EOF )
		fprintf(stderr, "eof ");
	if ( i & S_ERR )
		fprintf(stderr, "error ");
	if ( i & S_ILL )
		fprintf(stderr, "illegal ");
	if ( i & S_TO )
		fprintf(stderr, "timeout ");
	fprintf(stderr, "\n");
}
