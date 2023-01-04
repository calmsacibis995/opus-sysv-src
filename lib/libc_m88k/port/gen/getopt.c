/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/getopt.c	1.14"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#define NULL	0
#define EOF	(-1)
#define ERR(s, c)	if(opterr){\
	extern int _strlen(), _write();\
	char errbuf[2];\
	errbuf[0] = c; errbuf[1] = '\n';\
	(void) _write(2, argv[0], (unsigned)_strlen(argv[0]));\
	(void) _write(2, s, (unsigned)_strlen(s));\
	(void) _write(2, errbuf, 2);}

#include "shlib.h"
extern int _strcmp();
extern char *_strchr();
/*
 * If building the regular library, pick up the defintions from this file
 * If building the shared library, pick up definitions from opt_data.c 
 */

extern int opterr, optind, _optopt;
extern char *optarg;

static char error1[] = ": option requires an argument -- ";
static char error2[] = ": illegal option -- ";

int
_getopt(argc, argv, opts)
int	argc;
char	**argv, *opts;
{
	static int sp = 1;
	register int c;
	register char *cp;

	if(sp == 1)
		if(optind >= argc ||
		   argv[optind][0] != '-' || argv[optind][1] == '\0')
			return(EOF);
		else if(_strcmp(argv[optind], "--") == NULL) {
			optind++;
			return(EOF);
		}
	_optopt = c = argv[optind][sp];
	if(c == ':' || (cp=_strchr(opts, c)) == NULL) {
		ERR(error2,c);
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
			ERR(error1,c);
			sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}
