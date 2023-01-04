/*	opterm.c
**
**	Determine proper value for TERM based on various
**	environment variables and a database in /etc/lines.
**	Copy new TERM value to stdout.
**
**	/etc/lines format:
**
**	tty:tty type:username:hostname:term:option:comment
**
**	tty = device name (eg tty2)
**	tty type = one of:
**			CONSOLE
**			PTTY  (pseudo-tty)
**			MODEM (tty* connected to modem)
**			TTY   (other tty*)
**			SXT   (sxt*)
**	username = login name of user
**	hostname = host system of user
**	term = new TERM value
**	option = <empty> or ASK
**
**	The last line in /etc/lines matching tty:tty type:username:hostname
**	is used.
*/
#ifndef opus
#define	opus	1
#endif
#ifndef CMMU
#define	CMMU	1
#endif

#include <stdio.h>
#include "sys/types.h"
#include "sys/signal.h"
#include "sys/param.h"
#include "sys/immu.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/var.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/fcntl.h"
#include "sys/tty.h"
#include "nlist.h"
#include <utmp.h>

#define LINES		"/etc/lines"
#define ENV_TERM	"TERM"
#define ENV_LOGNAME	"LOGNAME"
#define XPARENT		"xterm"
#define XTERM		"xterms"

#define OTHER	0
#define CONSOLE	1
#define TTY	2
#define PTTY	3
#define SXT	4

#define FIELDtty	0
#define FIELDttype	1
#define FIELDusername	2
#define FIELDhostname	3
#define FIELDterm	4
#define FIELDoption	5
#define FIELDcomment	6

extern FILE *fopen();
extern char *ttyname();
extern char *getenv();
extern char *strdup();
extern int gmatch();
extern struct utmp *getutline();

char match_term[256];
char match_opt[256];

char username[L_cuserid];

char *tty;
int ttype;
char *ttypes;
char *term;
char *hostname;
FILE *lines;
char *arg0;
struct utmp *utent;
struct utmp utline;

main(argc, argv)
int argc;
char *argv[];
{
	arg0 = argv[0];

	/*  if TERM is already defined and not 'unknown' or 'network',
	**  go with the definition
	*/
	term = getenv(ENV_TERM);
	if (term) {
		if (!strcmp(term, ""))
			term = NULL;
		else if (!strcmp(term, "network"))
			term = NULL;
		else if (!strcmp(term, "unknown"))
			term = NULL;
		else {
			fprintf(stdout, "%s\n", term);
			exit(0);
		}
	}

	/*  if we have an xterm in our lineage, use 'xterms'
	*/
	if (isancestor(0, XPARENT)) {
		fprintf(stdout, "%s\n", XTERM);
		exit(0);
	}

	/*  get user name for future use
	*/
	cuserid(username);

	/*  determine our tty name and type
	*/
	gettyname();

	/*  get hostname for future use
	*/
	strncpy(utline.ut_line, tty, sizeof(utline.ut_line));
	utent = getutline(&utline);
	if (utent != NULL && !strncmp(utent->ut_line, "tty", 3)) {
		hostname = strdup(utent->ut_pad2);
	} else {
		hostname = "local";
	}

	/*  open /etc/lines
	**  if it doesn't exist, nolines() applies reasonable defaults
	**  if it does, scanlines() interprets it
	*/
	lines = fopen(LINES, "r");
	if (lines == NULL)
		nolines();
	else
		scanlines();
	exit(0);
}

/*  determine tty name and type
*/
gettyname()
{
	tty = ttyname(2);

	/*  if ttyname fails, we have no way of determining anything else,
	**  so ask.
	*/
	if (tty == NULL)
		ask(NULL);

	tty += 5;	/* skip "/dev/" */
	if (gmatch(tty, "sxt*")) {
		ttype = SXT;
		ttypes = "SXT";
	} else if (gmatch(tty, "tty[Ta-z]*")) {
		ttype = PTTY;
		ttypes = "PTTY";
	} else if (gmatch(tty, "tty*")) {
		ttype = TTY;
		ttypes = "TTY";
	} else if (gmatch(tty, "con*")) {
		ttype = CONSOLE;
		ttypes = "CONSOLE";
	} else {
		ttype = OTHER;
		ttypes = "OTHER";
	}
}

ask(def)
char *def;
{
	char gterm[5120];

	if (def)
		fprintf(stderr, "TERM[%s]=", def);
	else {
		fprintf(stderr, "TERM=");
		def = "";
	}
	if (gets(gterm) == NULL || gterm[0] == '\0')
		fprintf(stdout, "%s\n", def);
	else
		fprintf(stdout, "%s\n", gterm);
	exit(0);
}

nolines()
{
	switch (ttype) {

	case CONSOLE:
		ask("opus-pc");
		break;

	case PTTY:
		ask(XTERM);
		break;

	default:
		ask(NULL);
		break;

	} /* switch (ttype) */
}

scanlines()
{
	int i;
	char lbuf[5120];
	int matched;
	char *p;
	int field;
	char *fields[FIELDcomment+8];
	int line;

	line = 0;
	matched = 0;
	for (;;) {
		if (fgets(lbuf, sizeof(lbuf), lines) == NULL)
			break;
		++line;
		fields[0] = lbuf;
		if (lbuf[0] == '\0' || lbuf[0] == '#')
			continue;
		for (p=lbuf, field=1; *p && field < 8; ++p) {
			if (*p == '\n') {
				*p = '\0';
				break;
			}
			if (*p == ':') {
				*p = '\0';
				fields[field++] = p + 1;
			}
		}
		if (field < FIELDcomment)
			continue;

		/* strip "/dev/" from lines entry
		*/
		if (!strncmp(fields[FIELDtty], "/dev/", 5))
			fields[FIELDtty] += 5;

		/* allow change in tty type string
		*/
		if (!strcmp(tty, fields[FIELDtty]))
			ttypes = strdup(fields[FIELDttype]);

		if (!gmatch(tty, fields[FIELDtty]))
			continue;
		if (strcmp(ttypes, fields[FIELDttype]))
			continue;
		if (!gmatch(username, fields[FIELDusername]))
			continue;
		if (!gmatch(hostname, fields[FIELDhostname]))
			continue;
		matched = 1;
		strncpy(match_term, fields[FIELDterm], 256);
		strncpy(match_opt,  fields[FIELDoption], 256);
	}
	if (matched == 0)
		ask(NULL);
	if (!strcmp(match_opt, "ASK"))
		ask(match_term);
	fprintf(stdout, "%s\n", match_term);
}

#if m88k
struct proc prc;
struct var v;
int mem, kmem;
struct user u;

struct nlist nl[] = {
	{ "_proc",0,0,0,0,0 },
	{ "_v",0,0,0,0,0 },
	{ 0,0,0,0,0,0 },
};

/*	isancestor()
**
**	Determine whether the specified ancestor program name
**	is an ancestor of the specified pid.  If pid==0, use our pid.
**	Return 0 (false) or 1.
*/
isancestor(pid, ancestor)
int pid;
char *ancestor;
{
	char parent[5120];
	char *p, *pp;

	if (pid == 0)
		pid = getpid();
	parent[sizeof(parent) - 1] = 0;
	while (pid > 0) {
		initvars(pid);
		pp = parent;
		strncpy(pp, u.u_psargs, sizeof(parent) - 1);
		for (p=pp; *p; ++p) {
			if (*p == ' ' || *p == '\t' || *p == '\n') {
				*p = 0;
				break;
			}
		}
		if (ancestor[0] != '/') for (p=pp; *p; ++p) {
			if (*p == '/')
				pp = p + 1;
		}
		if (!strcmp(pp, ancestor)) {
			return(1);
		}
		pid = prc.p_ppid;	/* parent pid */
	}
	return(0);
}

initvars(pid)
register pid;
{
	register i;

	if (nl[0].n_value == 0) {
		/* address of process table */
		nlist("/dev/unix", nl);
		if(nl[0].n_value==0)
			fatal("no namelist");

		/* open mem & kmem */
		if ((mem = open("/dev/mem", O_RDONLY)) < 0)
			fatal("cannot open /dev/mem");
		if ((kmem = open("/dev/kmem", O_RDONLY)) < 0)
			fatal("cannot open /dev/kmem");
		kmemread(nl[1].n_value, &v, sizeof(v));
	}

	/* find process table slot by pid */
	for (i = 0; i < v.v_proc ; i++ )
	{
		kmemread( nl[0].n_value + i * sizeof(struct proc),
				&prc, sizeof(struct proc));
		if (prc.p_pid == pid)
			break;
	}
	if (i == v.v_proc)
		fatal("cannot find pid");

	/* read user area */
	memread(prc.p_ubpgtbl[0].pgi.pg_spde & ~POFFMASK, 
			&u, sizeof(struct user));
}

kmemread(kva, uva, count)
{
	if (lseek(kmem, kva, 0) != kva)
		fatal("lseek");
	if (read(kmem, uva, count) != count)
		fatal("kmemread");
}

memread(kpa, uva, count)
{
	if (lseek(mem, kpa, 0) != kpa)
		fatal("lseek");
	if (read(mem, uva, count) != count)
		fatal("memread");
}

fatal(string)
{
	extern errno;

	printf("%s: %s:", arg0, string);
	if (errno)
		perror("");
	ask(NULL);
}
#else	/* not m88k */
/*
**  for machines not supported, return false always
*/
isancestor(pid, ancestor)
int pid;
char *ancestor;
{
	return(0);
}
#endif
