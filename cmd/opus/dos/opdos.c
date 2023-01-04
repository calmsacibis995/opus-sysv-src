/* SID @(#)opdos.c	1.9 */

/*	opdos.c
 *
 *	 4/06/87	reduced BUFLEN for I/O version
 *	 6/10/86	isdir: recognize root directories
 *	 6/03/86	pass DOS exit code from pause
 */

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/termio.h>
#include <sys/doscomm.h>

#define BUFLEN 8192  /* length of temp buffer */
#define CMDLEN 1024  /* length of temp dos command buffer */
#define ERROFF 100  /* base of exit codes for dos related errors */
#define CONSOLE "/dev/tty"  /* wait on this device for output to drain */

char buf[BUFLEN];  /* temp buffer */
char buf2[BUFLEN*2];  /* temp buffer */
int silent;  /* 1 of no error messages should be printed */
static int dosfn;	/* fn of open dos file */
char *ttyname();
int ondel();

extern int errno;
extern int doserrno;

main(argc,argv)
int argc;
char **argv;
{
	int len;
	int arg;
	int exitcode;
	int temp1, temp2, temp3, temp4;
	char *cbuf;
	register char *cp, *cp2;

	dosfn = -1;		/* indicate no dos file open */
	exitcode = 0;		/* default exit code is 0 */
	if (signal(SIGINT, ondel) == SIG_IGN) { /* background */
		signal(SIGINT, SIG_IGN);
	}
	argc--;
	argv++;
	if (argc < 1) {
		fatal("no arguments", 1);
	}
	while (1) {
		if (strcmp(argv[0], "-s") == 0) {
			silent++;
			argc--;
			argv++;
			continue;
		}
		if (strcmp(argv[0], "-k") == 0) {
			waitsync();
			argc--;
			argv++;
			continue;
		}
		if (strcmp(argv[0], "-d") == 0) {
			drainconsole();
			argc--;
			argv++;
			continue;
		}
		break;
	}
	if (argc < 1) {
		fatal("no arguments", 1);
	}
	if (strcmp(argv[0], "exit") == 0 ||
	    strcmp(argv[0], "exit!") == 0) {

		if (strcmp(argv[0], "exit") == 0) {
			char *name;
			name = ttyname(0);	/* name of standard input */
			if (strncmp(name, "/dev/con", 8) != 0 &&
			    strcmp(name, "/dev/syscon")  != 0 &&
			    strcmp(name, "/dev/systty")  != 0) {
				fatal("must quit from console", 1);
			}
		}
		if (argc < 2) {
			arg = 0;
		} else {
			arg = atoi(argv[1]);
		}
		if (sysexit(arg) < 0) {
			dosfu("sysexit failed");
		}
	} else if (strcmp(argv[0], "pause") == 0 ||
		   strcmp(argv[0], "pause!") == 0) {

		if (strcmp(argv[0], "pause") == 0) {
			char *name;
			name = ttyname(0);	/* name of standard input */
			if (strncmp(name, "/dev/con", 8) != 0 &&
			    strcmp(name, "/dev/syscon")  != 0 &&
			    strcmp(name, "/dev/systty")  != 0) {
				fatal("must pause from console", 1);
			}
		}
		if (argc < 2) {
			arg = 0;
		} else {
			arg = atoi(argv[1]);
		}
		if ((exitcode = syspause(arg)) < 0) {
			dosfu("syspause failed");
		}
	} else if (strcmp(argv[0], "cmdline") == 0) {
		if (syscmdline(buf, CMDLEN) < 0) {
			dosfu("syscmdline failed");
		}
		printf("%s\n", buf);
	} else if (strcmp(argv[0], "exec") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		if (sysexec(argv[1], strlen(argv[1])+1) < 0) {
			dosfu("sysexec failed");
		}
	} else if (strcmp(argv[0], "command") == 0) {
		if (doswaitfg()) {
			dosfu("doswaitfg failed");
		}
		if (syscmdline(buf, CMDLEN) < 0) {
			dosfu("syscmdline failed");
		}
		for (cbuf=buf; cbuf[0]; cbuf++) {
			if (cbuf[0] == ';' && cbuf[1] == ';') {
				break;
			}
		}
		printf("%s\n", &cbuf[2]);
	} else if (strcmp(argv[0], "diskfree") == 0) {
		if (argc < 2) {
			arg = 0;
		} else {
			arg = atoi(argv[1]);
		}
		if (dosdiskfree(arg, &temp1, &temp2, &temp3, &temp4) < 0) {
			dosfu("dosdiskfree failed");
		}
		printf("%d %d %d %d\n", temp1, temp2, temp3, temp4);
	} else if (strcmp(argv[0], "chdrv") == 0) {
		if (argc < 2) {
			arg = 0;
		} else {
			arg = atoi(argv[1]);
		}
		if ((temp1 = doschdrv(arg)) < 0) {
			dosfu("doschdrv failed");
		}
	} else if (strcmp(argv[0], "mkdir") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		if (dosmkdir(argv[1]) < 0) {
			dosfu("dosmkdir failed");
		}
	} else if (strcmp(argv[0], "rmdir") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		if (dosrmdir(argv[1]) < 0) {
			dosfu("dosrmdir failed");
		}
	} else if (strcmp(argv[0], "rename") == 0) {
		if (argc < 3) {
			fatal("missing argument", 2);
		}
		if (dosrename(argv[1], argv[2]) < 0) {
			dosfu("dosrename failed");
		}
	} else if (strcmp(argv[0], "chmod") == 0) {
		if (argc < 3) {
			fatal("missing argument", 2);
		}
		if (doschmod(argv[1], 1, atoi(argv[2])) < 0) {
			dosfu("doschmod failed");
		}
	} else if (strcmp(argv[0], "getmod") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		if ((temp1 = doschmod(argv[1], 0, 0)) < 0) {
			dosfu("doschmod failed");
		}
		printf("%d\n", temp1);
	} else if (strcmp(argv[0], "isdir") == 0) {
		struct dos_find dos_find;
		char *p;
		int i, len;

		if (argc < 2) {
			fatal("missing argument", 2);
		}
		p = dos_find.path;
		strncpy(p, argv[1], 256);
		for (i=0; p[i]; ++i) {
			if (p[i] == '\\')
				p[i] = '/';
		}
		len = strlen(p);
		dos_find.attr = ATTR_DIRECTORY;
		if (len==1 && (p[0]=='/' || p[0]=='.'))
			;
		else if (len==2 && isalpha(p[0]) && p[1]==':')
			;
		else if (len==3 && isalpha(p[0]) && p[1]==':' && p[2]=='/')
			;
		else if (dosfindfirst(&dos_find) < 0) {
			if (doserrno != -18) {
				dosfu("dosfindfirst failed");
			} else {
				dos_find.attr = 0;
			}
		} 
		printf("%d\n", dos_find.attr & ATTR_DIRECTORY ? 1 : 0);
	} else if (strcmp(argv[0], "wildcard") == 0) {
		struct dos_find dos_find;
		int i, j, c;
		char path[256];

		if (argc < 2) {
			fatal("missing argument", 2);
		}

		/* skip leading colons */
		while (*argv[1] == ':')
			++argv[1];

		/* copy path arg to find structure */
		strncpy(dos_find.path, argv[1], 256);

		/* attribute 0 for normal files */
		dos_find.attr = 0;

		/* translate backslashes to slashes */
		for (i=0; c = dos_find.path[i]; ++i) {
			if (c == '\\')
				dos_find.path[i] = '/';
		}

		/* find directory part of path */
		for (i=i-1; i>=0; --i) {
			if (dos_find.path[i] == '/' || dos_find.path[i] == ':')
				break;
		}

		/* save directory part of path */
		for (j=0; j<=i; ++j)
			path[j] = dos_find.path[j];
		path[j] = 0;

		/* expand wildcard using find-first/find-next */
		if (dosfindfirst(&dos_find) >= 0) {
			do
				printf("%s%s\n", path, dos_find.name);
			while (dosfindnext(&dos_find) >= 0);
		} else {
			if (doserrno != -18) {
				dosfu("dosfindfirst failed");
			}
		}
	} else if (strcmp(argv[0], "cd") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		if (doschdir(argv[1]) < 0) {
			dosfu("doschdir failed");
		}
	} else if (strcmp(argv[0], "del") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		if (dosunlink(argv[1]) < 0) {
			dosfu("dosunlink failed");
		}
	} else if (strcmp(argv[0], "read") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		if ((dosfn = dosopen(argv[1], 0)) < 0) {
			dosfu("dosopen failed");
		}
		while (1) {
			len = dosread(dosfn, buf, BUFLEN);
			if (len < 0) {
				dosfu("dosread failed");
			}
			if (len == 0) {
				break;
			}
			if (write(1, buf, len) < len) {
				filesysfu("write failed");
			}
		}
		dos_close();
	} else if (strcmp(argv[0], "write") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		if (argc > 2) {
			arg = atoi(argv[2]);
		} else {
			arg = 0;
		}
		if ((dosfn = doscreat(argv[1], arg)) < 0) {
			dosfu("dosopen failed");
		}
		while (1) {
			len = read(0, buf, BUFLEN);
			if (len < 0) {
				filesysfu("read failed");
			}
			if (len == 0) {
				break;
			}
			if (doswrite(dosfn, buf, len) < len) {
				dosfu("doswrite failed");
			}
		}
		dos_close();
	} else if (strcmp(argv[0], "asciiread") == 0) {

		int crlf, crflag, lfflag;
		crflag = 0;
		lfflag = 0;
		crlf = 0;

		if (argc < 2) {
			fatal("missing argument", 2);
		}
		if ((dosfn = dosopen(argv[1], 0)) < 0) {
			dosfu("dosopen failed");
		}
		while (1) {
			len = dosread(dosfn, buf, BUFLEN);
			if (len < 0) {
				dosfu("dosread failed");
			}
			if (len == 0) {
				break;
			}
			cp = buf;
			while (len--) {
				if (*cp == '\n') {
					if (!crflag) {
						putchar('\n');
						lfflag = 1;
						crlf = 1;
					}
					crflag = 0;

				} else if (*cp == '\r') {
					if (!lfflag) {
						putchar('\n');
						crflag = 1;
						crlf = 1;
					}
					lfflag = 0;

				} else if (*cp == 0x1a) {
					if (!crlf) {
						putchar('\n');
					}
					goto asciireadout;
				} else {
					putchar(*cp);
					crlf = 0;
					crflag = 0;
					lfflag = 0;
				}
				cp++;
			}
		}

		asciireadout:
		dos_close();

	} else if (strcmp(argv[0], "asciiwrite") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		if (argc > 2) {
			arg = atoi(argv[2]);
		} else {
			arg = 0;
		}
		if ((dosfn = doscreat(argv[1], arg)) < 0) {
			dosfu("dosopen failed");
		}
		while (1) {
			len = read(0, buf, BUFLEN);
			if (len < 0) {
				filesysfu("read failed");
			}
			if (len == 0) {
				break;
			}
			cp = buf;
			cp2 = buf2;
			while (len--) {
				if (*cp == '\n') {
					*cp2++ = '\r';
				}
				if (*cp != '\r') {
					*cp2++ = *cp++;
				} else {
					cp++;
				}
			}
			if (doswrite(dosfn, buf2, cp2-buf2) < (cp2-buf2)) {
				dosfu("doswrite failed");
			}
		}
		dos_close();
	} else if (strcmp(argv[0], "filelen") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		if ((dosfn = dosopen(argv[1], 0)) < 0) {
			dosfu("dosopen failed");
		}
		if ((temp1 = doslseek(dosfn, 0, 2)) < 0) {
			dosfu("doslseek failed");
		}
		dos_close();
		printf("%d\n", temp1);
	} else if (strcmp(argv[0], "diskname") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		temp1 = atoi(argv[1]);
		if (argc < 3) {
			if ((errno = getdiskname(temp1, buf)) != 0) {
				filesysfu("disk function failed");
			} else {
				printf("%s\n", buf);
			}
		} else {
			if ((errno = setdiskname(temp1, argv[2])) != 0) {
				filesysfu("disk function failed");
			}
		}
	} else if (strcmp(argv[0], "diskparm") == 0) {
		if (argc < 2) {
			fatal("missing argument", 2);
		}
		temp1 = atoi(argv[1]);
		if ((errno = getdiskparm(temp1, &temp2)) != 0) {
			filesysfu("disk function failed");
		} else {
			printf("%d\n", temp2);
		}
	} else if (strcmp(argv[0], "opstart") == 0) {
		sysstartprofile();
	} else if (strcmp(argv[0], "opstop") == 0) {
		sysstopprofile();
	} else if (strcmp(argv[0], "opsave") == 0) {
		syssaveprofile();
	} else if (strcmp(argv[0], "null") == 0) {
	} else {
		fatal("illegal command", 3);
	}
	dosexit(exitcode);
}

/*
 * Exit because of some fatal error explained by 'str' and 'exitcode'.
 */
fatal(str, exitcode)
char *str;
int exitcode;
{

	if (!silent) {
		fprintf(stderr, "opdos: %s\n", str);
	}
	dosexit(exitcode);
}

/*
 * Warn the user of some error explained by 'str'.
 */
warn(str)
char *str;
{

	if (!silent) {
		fprintf(stderr, "opdos: %s\n", str);
	}
}

/*
 * Exit because of a dos error which is defined in doserrno.
 */
dosfu(str)
char *str;
{
	char buf[1024];

	if (!silent) {
		if (doserrno < 0) {
			fprintf(stderr, "opdos: %s: DOS error %d\n",
				str, -doserrno);
		} else {
			sprintf(buf, "opdos: %s", str);
			perror(buf);
		}
	}
	dosexit(ERROFF+doserrno);
}

/*
 * Exit because of a file system error which is defined in errno.
 */
filesysfu(str)
char *str;
{
	char buf[1024];

	if (!silent) {
		sprintf(buf, "opdos: %s", str);
		perror(buf);
	}
	dosexit(ERROFF+errno);
}

/*
 *  close dos file and reset dosfn
 */
dos_close()
{
	int fn;

	fn = dosfn;
	dosfn = -1;
	if (dosclose(fn) < 0) {
		dosfu("dosclose failed");
	}
}

/*
 *  Final exit; close any open dos file
 */
dosexit(exitcode)
int exitcode;
{
	if (dosfn > 0) {
		dosclose(dosfn);
	}
	exit(exitcode);
}

/*
 *  Catch DELs
 */
ondel()
{
	dosexit(0);
}

/*
 * Wait for output to drain on console before returning.
 */
drainconsole()
{
	int fn;
	struct termio t;

	if ((fn = open(CONSOLE, O_RDONLY)) < 0) {
		return;
	}
	ioctl(fn, TCGETA, &t);
	ioctl(fn, TCSETAW, &t);
	close(fn);
}

/*
 * Start a sync and wait for disk queues to drain.
 */
waitsync()
{

	sync();
	draindisk();
}
