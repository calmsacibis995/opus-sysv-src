/* SID @(#)dialer.c	1.6 */

/*
 * 
 * <modem_type> [-d] <tty> <baud> <number>
 *
 * Dial a modem of type <modem_type>. The program is normally invoked by
 * dial(3).
 *
 * <modem_type> The name of the program is used to indicate the kind of
 *		modem that is being dialed. The following modems are currently
 *		supported:
 *
 *			ventel		(Ventel 212+)
 *			vadic		(Vadic 3451)
 *			hayes		(Hayes Smartmodem 1200)
 *			usrobotics	(U.S. Robotics 212)
 *
 * <tty>	This is the character special device that is connected
 *		to the modem.
 *		The device should already be open by another process so that
 *		the DTR signal does not drop when dialing is complete and this
 *		process closes the line.
 *
 * <baud>	is the baud rate at which dialing should occur.
 *
 * <number>	This is the phone number to be called.
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <signal.h>
#include <dial.h>
#if m88k
#include <termio.h>
#endif

/*
 * Dialer exit codes (these are normally returned by dial(3)).
 */
#define DE_YES	0	/* Connected */
#define DE_ARG	1	/* Parameter error */
#define DE_TTY	2	/* No tty access */
#define DE_ANS	3	/* No answer */
#define DE_ERR	4	/* Dialer not responding */

/*
 * Dialer type codes
 */
#define VENTEL  1               /* Ventel 212+ */
#define VADIC   2               /* Vadic 3451 */
#define USROBOT 3               /* USRobotics autodial 212A */
#define HAYES	4		/* Hayes Smartmodem 1200 */

/*
 * Useful definitions.
 */
#define nel(x)  (sizeof(x)/sizeof((x)[0]))      /* Number of array elements */
#define reg     register                        /* Prevent pushbutton fingers */
#define ACULOG  "/usr/adm/diallog"		/* log of dialing activity */
#define MAXLIN  80                              /* max line length */
#define STATIC                                  /* Statics (global for adb) */
#define OK 100                                  /* return code that isn't EOF */

/*
 * Function-returning declarations.
 */
char *ctime();
void done();
int getbaud();
char *getdial();
char *getlogin();
struct passwd *getpwuid();
void timeout();
void show();
/* int (*signal())(); */
int strcmp();
char *strcpy();
long time();
int dial_getc();
int dial_init();
int dial_dial();
int dial_reset();
void dial_write();

/*
 * Static variables.
 */
STATIC FILE *logfd;  /* logging file descriptor */
STATIC int debug;  /* debugging flag */
STATIC char prompt[80];  /* working storage */
STATIC int modem_type;  /* type of modem */
STATIC char *progname;  /* name of program being run */
STATIC int acufn;  /* file number of acu line */

/*
 * Actual exit codes to use for the various internal exit types.
 */
STATIC int exitcodes[] = {
	0,	/* Connected */
	ILL_BD,	/* Parameter error */
	A_PROB,	/* No tty access */
	NO_ANS,	/* No answer */
	D_HUNG,	/* Dialer not responding */
};

/*
 * Table of legal baud rates and the c_cflags code for each.
 */
struct baudtab {
	char *str;
	int val;
} baudtab[] = {
	"0", B0,
	"50", B50,
	"75", B75,
	"110", B110,
	"134", B134,
	"150", B150,
	"200", B200,
	"300", B300,
	"600", B600,
	"1200", B1200,
	"1800", B1800,
	"2400", B2400,
	"4800", B4800,
	"9600", B9600,
	"EXTA", EXTA,
	"EXTB", EXTB,
	"exta", EXTA,
	"extb", EXTB,
};

/*
 * Table of acceptable modem names and their internal names.
 */
struct modemtab {
	char *name;
	int type;
} modemtab[] = {
	"vadic", VADIC,
	"usrobotics", USROBOT,
	"ventel", VENTEL,
	"hayes", HAYES,
};

/*
 * Printable error message for each of the internal error codes.
 */
STATIC char *status[] = {
	"Connected",
	"Parameter error",
	"No tty access",
	"No answer",
	"Dialer not responding",
};

/*
 * Main routine.
 */
main(ac, av)
int ac;
reg char *av[];
{
	int err;
	int baud;
	char *dial;
	struct stat sb;
	struct termio   sg, osg;
	int fg;

	setuid(getuid());

	/* scan arg 0 to find program name without leading path component */

	for (progname = &av[0][strlen(av[0])-1];
	    progname > av[0] && progname[-1] != '/';
	    progname--) {
	}

	/* check for debug flag */

	if (ac > 1 && strcmp(av[1], "-d") == 0) {
		++debug;
		++av;
		--ac;
	}

	/* open logging file */

	if (access(ACULOG, 4) != 0) {
		logfd = debug ? stderr : NULL;
	} else {
		logfd = fopen(ACULOG, "a+");
	}

	/* parse parameters */

	if (ac < 4 ||
	    stat(av[1], &sb) < 0 ||
	    (sb.st_mode&S_IFMT) != S_IFCHR ||
	    (modem_type = get_modem(progname)) == 0 ||
	    (baud = getbaud(av[2])) == 0 ||
	    (dial = getdial(av[3])) == NULL)
		done(ac <= 1 ? "" : av[1],
		    ac <= 2 ? "": av[2], ac <= 3 ? "" : av[3], DE_ARG);

	/* open and initialize acu line */

	signal(SIGALRM, timeout);
	alarm(120);
	if ((acufn = open(av[1], O_RDWR | O_NDELAY)) < 0 ||
	    (fg = fcntl(acufn, F_GETFL, 0),
	    fg &= ~O_NDELAY,
	    fcntl(acufn, F_SETFL, fg) == -1) ||
	    ioctl(acufn, TCGETA, &sg) != 0 ||
	    ioctl(acufn, TCGETA, &osg) != 0 ||
	    (sg.c_iflag = 0,
	    sg.c_oflag = 0,
	    sg.c_cflag = baud|CS8|CREAD|CLOCAL,
	    sg.c_cc[VMIN] = 1,
	    ioctl(acufn, TCSETA, &sg) != 0)) {
		err = DE_TTY;
		goto finished;
	}
	alarm(0);

	/* dial */

	dial_reset(av[1]);
	if (dial_init()) {
		err = DE_ERR;
		goto finished;
	}
	err = dial_dial(dial);

finished:
	ioctl(acufn, TCSETA, &osg);
	done(av[1], av[2], av[3], err);
}

/*
 * Get the modem type given the <progname> that the program was invoked with.
 * Return 0 if <progname> dosn't represent a modem we can handle.
 */
STATIC int
get_modem(progname)
char *progname;
{
	int i;

	for (i = 0; i < nel(modemtab); i++) {
		if (strcmp(progname, modemtab[i].name) == 0) {
			return(modemtab[i].type);
		}
	}
	return(0);
}

/*
 * Returns the the termio baudrate field for a given baud rate string <rate>.
 */
STATIC int
getbaud(rate)
reg char *rate;
{
	int i;

	for (i = 0; i < nel(baudtab); i++) {
		if (strcmp(rate, baudtab[i].str) == 0) {
			return(baudtab[i].val);
		}
	}
	return(0);
}

/*
 *
 * Convert input phone number from acu(7) format into one acceptable
 * by the modem we are using.
 */
STATIC char *
getdial(phone)
reg char *phone;
{
	reg char *dp;
	static char result[80];

	dp = result;
	while (*phone) {
		switch (*phone) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '#':
		case '*':
		case ',':
		case 'p':
		case 't':
		case 'P':
		case 'T':
			*dp++ = *phone++;
			break;

		case 'w':
		case '=':
			switch (modem_type) {
			case VENTEL:
				*dp++ = '&';
				++phone;
				break;
			case VADIC:
				*dp++ = 'K';
				++phone;
				break;
			case USROBOT:
				*dp++ = ',';
				++phone;
				break;
			case HAYES:
				*dp++ = ',';
				++phone;
				break;
			}
			break;

		case '-':
			switch (modem_type) {
			case USROBOT:
			case HAYES:
				*dp++ = ',';
				*dp++ = ',';
				++phone;
				break;
			}
			break;

		case 'e':
		case '<':
			goto done;
			break;

		case '(':
		case ')':
		case '/':
			++phone;
			break;

		default:
			return (NULL);
		}
	}

done:
	*dp = NULL;
	return (result);
}

/*
 * Initialize the Dialer. 
 */
STATIC int
dial_init()
{
	reg int i, c, old_c;

	switch (modem_type) {

	case VENTEL:
		for (c = 0; c < 10; ++c) {
			dial_write("\r");
			sleep(1);
			dial_write("\r");
			while ((c = dial_getc(5)) != EOF)
				if (c == '$')
					return (0);
		}
		break;

	case USROBOT:
		dial_write("ATV0E0M1T\r");
		old_c = 0;
		while ((c = dial_getc(5)) != EOF)
			if (c == '\r' && old_c == '0')
				return(0);
			else old_c = c;
		break;

	case VADIC:
		return(0);

	case HAYES:
		dial_write("\rATV0Q0E0M1F1\r");
		old_c = 0;
		while ((c = dial_getc(5)) != EOF)
			if (c == '\r' && old_c == '0')
				return(0);
			else old_c = c;
		break;
	}
	return (-1);
}

/*
 * dial_dial()
 *
 * Dial a number.
 */
STATIC int
dial_dial(dial)
char    *dial;
{
	reg int c, old_c;

	switch (modem_type) {

	case VENTEL:
		dial_write("<k");
		dial_write(dial);
		dial_write("\r>");
		for (;;) {
			switch (dial_getc(120)) {
			case '\007':
				return (DE_ANS);
			case '!':
				if (dial_getc(5) == '\r')
					return (DE_YES);
			case EOF:
				return (DE_ERR);
			default:
				;
			}
		}
		break;

	case VADIC:
		dial_write("\005\r");
		if (EOF == get_prompt(10)) {
			return(DE_ERR);
		}
		sleep(1);
		dial_write("d\r");
		if (EOF == get_prompt(10)) {
			return(DE_ERR);
		}
		dial_write(dial);
		sleep(1);
		dial_write("\r");
		if (EOF == get_prompt(10)) {
			return(DE_ERR);
		}
		dial_write("\r");
		if (EOF == get_prompt(120)) {
			return(DE_ERR);
		}
		if (strcmp(prompt, "DIALING:  ON LINE") == 0)
			return(DE_YES);
		else return(DE_ERR);
		break;

	case USROBOT:
		dial_write("ATD");
		dial_write(dial);
		dial_write("\r");
		while ((c = dial_getc(120)) != NULL) {
			if (c == '\r')
				switch(old_c) {
				case '1':
					return(DE_YES);
				case '3':
					return(DE_ANS);
				}
			else old_c = c;
		}
		break;

	case HAYES:
		dial_write("ATDT");
		dial_write(dial);
		dial_write("\r");
		while ((c = dial_getc(120)) != NULL) {
			if (c == '\r')
				switch(old_c) {
				case '1':
				case '5':
					return(DE_YES);
				case '3':
					return(DE_ANS);
				}
			else old_c = c;
		}
		break;
	}

	return (DE_ERR);
}

/*
 * Reset the dialer. Returns 1 if there is some problem.
 */
STATIC int
dial_reset(fname)
reg char *fname;
{
	struct termio sg;
	int fn;
	int oldbaud;

	if ((fn = open(fname, O_RDWR | O_NDELAY)) < 0) {
		return(1);
	}
	ioctl(fn, TCGETA, &sg);
	oldbaud = sg.c_cflag & CBAUD;
	sg.c_cflag &= ~CBAUD;
	ioctl(fn, TCSETA, &sg);  /* turn off DTR */
	sg.c_cflag |= oldbaud;
	ioctl(fn, TCSETA, &sg);  /* reset speed */
	close(fn);
	fn = open(fname, O_RDWR | O_NDELAY);  /* turn on DTR */
	sleep(2);
	ioctl(fn, TCFLSH, 0);
	close(fn);
	return(0);
}

/*
 * Read from the dialer, timing out after a specified
 * number of seconds. Returns the input character, or
 * EOF on timeout.
 */
STATIC int
dial_getc(limit)
int             limit;
{
	char c;
	reg int count;
int fg;

	if (debug)
		fprintf(stderr, "dial_getc() ");
	signal(SIGALRM, timeout);
	alarm(limit);
	count = read(acufn, &c, 1);
	alarm(0);
	if (count == 1) {
		c &= 0177;
		if (debug) {
			show(c);
			fprintf(stderr, "\n");
		}
		return (c);
	} else {
		if (debug)
			fprintf(stderr, "<EOF>\n");
		return (EOF);
	}
}

/*
 * Write to the dialer.
 */
STATIC void
dial_write(buf)
reg char *buf;
{
	int c;
	int len;

	if (debug)
		fprintf(stderr, "dial_write() ");
	if (buf == NULL) {
		write(acufn, "\0", 1);
		if (debug) {
			show('\0');
		}
	} else {
		len = strlen(buf);
		write(acufn, buf, len);
		if (debug) {
			for ( ; len; --len)
				show(*buf++);
			fprintf(stderr, "\n");
		}
	}
}


/*
 * Display a character unambiguously.
 */
STATIC void
show(c)
reg int c;
{
	if (c < ' ' || c > '~')
		fprintf(stderr, "^%c", c ^ '@');
	else if (c == '^')
		fprintf(stderr, "^^");
	else
		putc(c, stderr);
}

/*
 * Null signal handler. Used to force a SIGALRM to
 * terminate an open(2) or read(2) request without
 * aborting the program.
 */
STATIC void
timeout()
{
	if (debug)
		fprintf(stderr, "\ntimeout()\n");
}

/*
 * Log status and exit with a specified code.
 */
STATIC void
done(tty, baud, phone, code)
char    *tty;
char    *baud;
char    *phone;
int     code;
{
	reg char        *user;
	reg struct passwd       *pwp;
	long            now;
	char            timestr[26];

	if (logfd != NULL) {
		if ((pwp = getpwuid(getuid())) != NULL)
			user = pwp->pw_name;
		else if ((user = getlogin()) == NULL)
			user = "?";
		time(&now);
		strcpy(timestr, ctime(&now));
		timestr[24] = NULL;
		fprintf(logfd, "%s (%s) %s %s %s %s\n",
		    user, timestr, tty, baud, phone,
		    code < nel(status) ? status[code] : "Unknown exit code!");
	}
	exit(exitcodes[code]);
}

/*
 * gets a string that is the result of a vadic operation and
 * puts it into the global string 'prompt'. 'to' is the max amount of time
 * to wait for the result string.
 */
STATIC int
get_prompt(to)
int     to;
{
	char *p;

	if (debug)
		fprintf(stderr, "get_prompt() ");
retry:
	p = prompt;
	while ((*p = dial_getc(to)) != EOF) {
		if (p > prompt && p[0] == '\012' && p[-1] == '\015') {
			p[-1] = '\0';  /* delete line terminator */
			if (strlen(prompt) == 0) {
				goto retry;  /* ignore blank lines */
			}
			if (debug) {
				fprintf(stderr, "good return <%s>\n",
				    prompt);
			}
			return(OK);
		}
		p++;
	}
	p[0] = '\0';  /* terminate string */
	if (debug) {
		fprintf(stderr, "bad returned <%s>\n", prompt);
	}
	return(EOF);
}
