# ifndef NOSCCS
static char *sccsid = "@(#)cmddir.c	1.4	4/14/86";
static char *cpyrid = "@(#)Copyright (C) 1985 by National Semiconductor Corp.";
# endif NOSCCS

/* Find the directory containing the currently executing command:

   	cmddir(arg0,result) 

   Arg0 should be argv[0], and result a pointer to a character array
   large enough for the complete path name of the current command.
   Upon exit result contains only the directory part of the path 
   name (with trailing /).

   If arg0 contains a pathname with any directory references, then the
   directory part is extracted from arg0.

   Otherwise, the PATH environment variable is used to search for an
   executable file of the name <arg0>, in the manner of execvp(3).  The
   directory part of the first successfully located command path is returned.

   The result is always returned with a trailing "/" (including "./").
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>


#if HOSTSYS5 || HOSTVMS
#  define index strchr
#  define rindex strrchr 
#endif

char *getenv();
char *index();
char *rindex();

#if HOSTVMS
#  include <stsdef.h>
#  define ERROR_EXITVAL STS$K_ERROR
#else
#  define ERROR_EXITVAL 1
#endif

#if !HOST_5_2
#define	NULL	0
#endif


static 
is_a_cmd(fullpath) char *fullpath;
{
	struct stat statbuf;
	register mode;
	if (stat(fullpath,&statbuf) < 0) return(0);
	mode = statbuf.st_mode;
	/* If directory, the "execute" bit means something else */
	if (mode & S_IFDIR) return(0);

	return (
	   ((mode<<6) & S_IEXEC) 		/* OTHERS executable */
	|| ( ((mode<<3) & S_IEXEC) && 		/* GROUP  executable */
	     ((statbuf.st_gid == getegid()) ||
	      (statbuf.st_gid == getgid())) )
	|| ( ((mode   ) & S_IEXEC) && 		/* USER   executable */
	     ((statbuf.st_uid == geteuid()) ||
	      (statbuf.st_uid == getuid())) )
	);
}

void
cmddir(arg0,result) 
	register char *arg0; 
	char *result;
{
	register char *path, *cp;

	if (cp = rindex(arg0,'/')) { /* Explicit directory given */
		int len = cp+1-arg0; 
		strncpy(result,arg0,len);
		result[len+1] = '\0';
		return;
	} else {
		path = getenv("PATH"); if (path==NULL) path = ":/bin:/usr/bin";

		do {
			register char *cpcmd = result;
			while (*path != ':' && *path) *cpcmd++ = *path++;
			path++ ;  /* pass delimiter or null */
			if (cpcmd == result) *cpcmd++ = '.';
			*cpcmd++ = '/';	strcpy(cpcmd,arg0);
			if (is_a_cmd(result)) { 
				/* SUCCESS */
#ifdef EXPAND_LINK
				expand_link(result, cpcmd);
#else
				*cpcmd = '\0';  /*leave only "<dir-path>/" */
#endif
				return; 
			}
		} while (path[-1]==':');
		fprintf(stderr,"cmddir unable to locate command. (arg0=%s)\n",
			arg0);
		exit(ERROR_EXITVAL);
	}
} /* cmddir */
  

  
#ifdef EXPAND_LINK
/*----------------------------------------------------------------------
 *	result points to the pathname of an executable file and cpcmd
 *  points to its last '/'. put the hard name of the directory containing 
 *  result in result. If result is not the name of a symbolic link, doing
 *  *cpcmd = '\0' is enough. If it is a symbolic link, keep replacing the 
 *  last component of result (or entire result) by its symbolic value.
 *--------------------------------------------------------------------*/
expand_link(result, cpcmd)
    char *result, *cpcmd;
{
#define LBUFSIZE 200
    struct stat ls;
    char lbuf[LBUFSIZE];
    int  cc;

    while (1) {
        fprintf(stderr, "expand_link(\"%s\", \"%s\")\n", result, cpcmd);
        if (lstat(result, &ls) == -1) {
	    perror("Cmddir panic");
	    fprintf(stderr,"Could'nt do lstat(\"%s\")\n", result);
	    exit(ERROR_EXITVAL);
        };
	if ((ls.st_mode & S_IFMT) != S_IFLNK) {
	    *cpcmd = '\0';
	    return;
	}
	cc = readlink(result, lbuf, LBUFSIZE);
	if (cc == -1) {
	    perror("Cmddir panic");
	    fprintf(stderr, "Could'nt do readlink\n");
	    exit(ERROR_EXITVAL);
	}
	lbuf[cc] = '\0';
	/* if  link name is absolute, replace the entire result with it else
	   replace the last component with it */
	strcpy(((lbuf[0] == '/') ? result : cpcmd), lbuf);
	cpcmd = rindex(result, '/') + 1;
    }
}
#endif
		
/* /* ***** TEST PROGRAM ***** */
/* #include <stdio.h>
/* #include <sys/times.h>
/* #define NTESTS 1000
/* 
/* main(argc,argv) char **argv; {
/* 	single_test(argv[0]);	
/* 	/* time_test(argv[0]); */
/* 	return(0);
/* } /* main */
/* 
/* single_test(arg0) char *arg0; {
/* 	char ds[100]; /* directory string result */
/* 	cmddir(arg0,ds);
/* 	printf("result is \"%s\"\n",ds);
/* }
/* 
/* time_test(arg0) char *arg0; {
/* 	struct tms start,stop;
/* #	define DELTA_MS(fld) ( ((stop.fld-start.fld)*1000)/60 )
/* 	register i;
/* 	char ds[100]; /* directory string result */
/* 
/* 	printf("Starting... "); fflush(stdout);
/* 	i = NTESTS / 12;  /* do them 12 at a time to reduce loop overhead */
/* 	times(&start);
/* 	while (i--) {
/* 	   cmddir(arg0,ds);cmddir(arg0,ds);cmddir(arg0,ds);
/* 	   cmddir(arg0,ds);cmddir(arg0,ds);cmddir(arg0,ds);
/* 	   cmddir(arg0,ds);cmddir(arg0,ds);cmddir(arg0,ds);
/* 	   cmddir(arg0,ds);cmddir(arg0,ds);cmddir(arg0,ds);
/* 	}
/* 	times(&stop);
/* 	printf("%d cmddir completed; answer is \"%s\".\n",NTESTS,ds);
/* 	i = (DELTA_MS(tms_utime)+DELTA_MS(tms_stime)) + (NTESTS/2);
/* 	printf("u = %d ms  s = %d ms  (total of %d.%d ms per call)\n",
/* 	       DELTA_MS(tms_utime), DELTA_MS(tms_stime),
/* 	       i / NTESTS, i % NTESTS	 
/* 	      );		
/* } /* time_test */
/* /* ***** END TEST PROGRAM ***** */
