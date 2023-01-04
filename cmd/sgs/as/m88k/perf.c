/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/perf.c	1.1" */


/*
 *  FILE
 *
 *	perf.c    performance reporting functions
 *
 *  SCCS
 *
 *	@(#)perf.c	1.45	5/6/87
 *
 *  DESCRIPTION
 *
 *	Collect and report performance data.
 *
 */

#include <stdio.h>
#include "gendefs.h"
#include "dbug.h"		/* Special macro based C debugging package */

unsigned numcalls;
unsigned numids;
unsigned numcoll;
static long ttime;
short Oflag = NO;
short tstlookup = NO;
static FILE *perfile;		/* performance data file descriptor */
extern FILE *fopen ();
extern long times ();

#ifdef SIM
#define times(a) 0
#endif

/*
 *	Performance data structure
 */

struct tbuffer {
    long proc_user_time;
    long proc_system_time;
    long child_user_time;
    long child_system_time;
} ptimes;


void perfsetup ()
{
    DBUG_ENTER ("perfsetup");
    /*	Performance data collected	*/
    ttime = times (&ptimes);
    DBUG_VOID_RETURN;
}

void perfreport ()
{
    DBUG_ENTER ("prefreport");
    if (tstlookup) {
	(void) printf ("Number of calls to lookup: %u\n", numcalls);
	(void) printf ("Number of identifiers: %u\n", numids);
	(void) printf ("Number of identifier collisions: %u\n", numcoll);
	(void ) fflush (stdout);
    }

    /*
     *	Performance data collected and written out here
     */
    
    ttime = times (&ptimes) - ttime;
    if ((perfile = fopen ("as.info", "r")) != NULL) {
	(void) fclose (perfile);
	if ((perfile = fopen ("as.info", "a")) != NULL) {
	    (void) fprintf (perfile,
		     "as1\t%07ld\t%07ld\t%07ld\t%07ld\t%07ld\tpass 1\n",
		     ttime, ptimes);
	    (void) fclose (perfile);
	}
    }
    DBUG_VOID_RETURN;
}

void perfreport2 ()
{
    DBUG_ENTER ("perfreport2");
    /*
     *	Performance data collected and written out here
     */
    
    ttime = times (&ptimes) - ttime;
    if ((perfile = fopen ("as.info", "r")) != NULL) {
	(void) fclose (perfile);
	if ((perfile = fopen ("as.info", "a")) != NULL) {
	    (void) fprintf (perfile,
		     "as2\t%07ld\t%07ld\t%07ld\t%07ld\t%07ld\tpass 2\n",
		     ttime, ptimes);
	    (void) fclose (perfile);
	}
    }
    DBUG_VOID_RETURN;
}
