/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)mailx:main.c	1.13.1.2"
#

#include <errno.h>
#include "rcv.h"
#include <sys/stat.h>
typedef	SIG	(*sigtype)();

/*
 * mailx -- a modified version of a University of California at Berkeley
 *	mail program
 *
 * Startup -- interface with user.
 */


jmp_buf	hdrjmp;

struct stat statbuf;
struct stat *statbufp;
struct utimbuf {
	time_t	actime;
#if m88k
	time_t	acusec;
#endif
	time_t	modtime;
#if m88k
	time_t	modusec;
#endif
};
struct utimbuf times;
struct utimbuf *utimep;
int my_gid, my_uid;

/*
 * Find out who the user is, copy his mail file (if exists) into
 * /tmp/Rxxxxx and set up the message pointers.  Then, print out the
 * message headers and read user commands.
 *
 * Command line syntax:
 *	mailx [ -i ] [ -r address ] [ -h number ] [ -f [ name ] ]
 * or:
 *	mailx [ -i ] [ -r address ] [ -h number ] people ...
 *
 * and a bunch of other options.
 */

main(argc, argv)
	char **argv;
{
	register char *ef;
	register int i, argp;
	int mustsend, uflag, hdrstop(), f,goerr = 0;
	SIG (*prevint)();
	int loaded = 0;
	FILE *ibuf, *ftat;
	extern char _sobuf[];
	struct termio tbuf;
	int c;
	extern char *optarg;
	extern int optind;

#ifdef signal
	Siginit();
#endif

	/*
	 * Set up a reasonable environment.  We clobber the last
	 * element of argument list for compatibility with version 6,
	 * figure out whether we are being run interactively, set up
	 * all the temporary files, buffer standard output, and so forth.
	 */

	uflag = 0;
	argv[argc] = (char *) -1;
	inithost();
	mypid = getpid();
	intty = isatty(0);
	if (ioctl(1, TCGETA, &tbuf)==0) {
		outtty = 1;
		baud = tbuf.c_cflag & CBAUD;
	}
	else
		baud = B9600;
	image = -1;
	setbuf(stdout, _sobuf);

	/*
	 * Now, determine how we are being used.
	 * We successively pick off instances of -r, -h, -f, and -i.
	 * If called as "rmail" we note this fact for letter sending.
	 * If there is anything left, it is the base of the list
	 * of users to mail to.  Argp will be set to point to the
	 * first of these users.
	 */

	ef = NOSTR;
	argp = -1;
	mustsend = 0;
	if (argc > 0 && **argv == 'r')
		rmail++;
	while ((c = getopt(argc, argv, "efFh:HinNr:s:u:UdIT:")) != EOF)
		switch (c) {
		case 'e':
			/*
			 * exit status only
			 */
			exitflg++;
			break;

		case 'r':
			/*
			 * Next argument is address to be sent along
			 * to the mailer.
			 */
			mustsend++;
			rflag = optarg;
			break;

		case 'T':
			/*
			 * Next argument is temp file to write which
			 * articles have been read/deleted for netnews.
			 */
			Tflag = optarg;
			if ((f = creat(Tflag, 0600)) < 0) {
				perror(Tflag);
				exit(1);
			}
			close(f);
			/* fall through for -I too */

		case 'I':
			/*
			 * print newsgroup in header summary
			 */
			newsflg++;
			break;

		case 'u':
			/*
			 * Next argument is person to pretend to be.
			 */
			uflag++;
			strcpy(myname, optarg);
			break;

		case 'i':
			/*
			 * User wants to ignore interrupts.
			 * Set the variable "ignore"
			 */
			assign("ignore", "");
			break;
		
		case 'U':
			UnUUCP++;
			break;

		case 'd':
			assign("debug", "");
			break;

		case 'h':
			/*
			 * Specified sequence number for network.
			 * This is the number of "hops" made so
			 * far (count of times message has been
			 * forwarded) to help avoid infinite mail loops.
			 */
			mustsend++;
			hflag = atoi(optarg);
			if (hflag == 0) {
				fprintf(stderr, "-h needs non-zero number\n");
				goerr++;
			}
			break;

		case 's':
			/*
			 * Give a subject field for sending from
			 * non terminal
			 */
			mustsend++;
			sflag = optarg;
			break;

		case 'f':
			/*
			 * User is specifying file to "edit" with mailx,
			 * as opposed to reading system mailbox.
			 * If no argument is given after -f, we read his
			 * mbox file in his home directory.
			 */
			if ( argc == optind ) {
				ef = (char *) calloc(1, strlen(Getf("MBOX"))+1);
				strcpy(ef, Getf("MBOX"));
			}
			else 
				if ( argv[optind][0] == '-' ) {
					ef = (char *) calloc(1, strlen(Getf("MBOX"))+1);
					strcpy(ef, Getf("MBOX"));
				} 
				else {
					ef = argv[optind];
					optind++;
				}
			break;

		case 'F':
			Fflag++;
			mustsend++;
			break;

		case 'n':
			/*
			 * User doesn't want to source
			 *	/usr/lib/mailx/mailx.rc
			 */
			nosrc++;
			break;

		case 'N':
			/*
			 * Avoid initial header printing.
			 */
			noheader++;
			break;

		case 'H':
			/*
			 * Print headers and exit
			 */
			Hflag++;
			break;

		case '?':
		default:
			goerr++;
			break;
		}

	if ( optind != argc )
		argp = optind;

	/*
	 * Check for inconsistent arguments.
	 */

	if (newsflg && ef==NOSTR) {
		fprintf(stderr, "Need -f with -I flag\n");
		goerr++;
	}
	if (ef != NOSTR && argp != -1) {
		fprintf(stderr, "Cannot give -f and people to send to.\n");
		goerr++;
	}
	if (exitflg && (mustsend || argp != -1))
		exit(1);	/* nonsense flags involving -e simply exit */
	if (mustsend && argp == -1) {
		fprintf(stderr, "The flags you gave are used only when sending mail.\n");
		goerr++;
	}
	if (goerr) {
		fprintf(stderr,"usage: mailx -eiIUdFnNH -T FILE -u USER -h hops -s SUBJECT -f FILE users\n");
		exit(1);
	}
	tinit();
	input = stdin;
	rcvmode = argp == -1;
	if (!nosrc)
		load(MASTER);

	if (argp != -1) {
		load(Getf("MAILRC"));
		mail(&argv[argp]);

		/*
		 * why wait?
		 */

		exit(senderr);
	}

	/*
	 * Ok, we are reading mail.
	 * Decide whether we are editing a mailbox or reading
	 * the system mailbox, and open up the right stuff.
	 *
	 * Do this before sourcing the MAILRC, because there might be
	 * a 'chdir' there that breaks the -f option. But if the
	 * file specified with -f is a folder name, go ahead and
	 * source the MAILRC anyway so that "folder" will be defined.
	 */

	strcpy(origname, mailname);

	if (ef != NOSTR) {
		char *ename, *tmp;

		edit++;
		if (*ef == '+') {
			load(Getf("MAILRC"));
			loaded++;
		}
		ename = expand(ef);
		if (ename != ef) {
			ef = (char *) calloc(1, strlen(ename) + 1);
			strcpy(ef, ename);
		}
		strcpy(origname, ename);
		editfile = ef;
		if ( ef[0] != '/' ) {
			tmp=(char *)malloc(PATHSIZE);
			curdir(tmp);
			strcat(tmp, "/");
			ef=(char *)calloc(1, strlen(ename)+strlen(tmp)+1);
			strcpy(ef, tmp);
			strcat(ef, ename);
		}
		strcpy(mailname, ef);
	}

	cksaved();
	lock(mailname);
	if (setfile(mailname, edit) < 0) {
		unlock(mailname);
		exit(1);
	}
	unlock(mailname);

	if (!loaded) load(Getf("MAILRC"));
	if (msgCount > 0 && !noheader && value("header") != NOSTR) {
		if (setjmp(hdrjmp) == 0) {
			if ((prevint = sigset(SIGINT, SIG_IGN)) != (sigtype) SIG_IGN)
				sigset(SIGINT, hdrstop);
			announce();
			fflush(stdout);
			sigset(SIGINT, prevint);
		}
	}
	if (Hflag || (!edit && msgCount == 0)) {
		if (!Hflag)
			fprintf(stderr, "No mail for %s\n", myname);
		fflush(stdout);
		exit(0);
	}
	commands();
	if (!edit) {
		sigset(SIGHUP, SIG_IGN);
		sigset(SIGINT, SIG_IGN);
		sigset(SIGQUIT, SIG_IGN);
		quit();
	}
	exit(0);
}

/*
 * Interrupt printing of the headers.
 */
hdrstop()
{

	clrbuf(stdout);
	printf("\nInterrupt\n");
	fflush(stdout);
	sigrelse(SIGINT);
	longjmp(hdrjmp, 1);
}

