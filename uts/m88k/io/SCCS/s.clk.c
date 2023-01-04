h55945
s 00054/00000/00000
d D 1.1 90/03/06 12:28:36 root 1 0
c date and time created 90/03/06 12:28:36 by root
e
u
U
t
T
I 1
#include "sys/types.h"
#include "sys/commpage.h"
#include "sys/cp.h"
#include "sys/iorb.h"

int	clkrunning;
extern time_t	time;	/* time in seconds since 1970 */


/*
 * Start clock
 */

clkstart()
{
#ifndef SIMULATOR
	COMMPAGE->ckflg = S_GO;
#endif
	clkrunning = 1;
}

/*
 * Adjust the time to UNIX based time
 */

#if !WS
clkset(oldtime)
register time_t	oldtime;
{
	time = timeofday();
	if (time < 100000) 
		time = oldtime;
}
#endif

/*
 * Stop the clock.
 */

clkreld(on)
{
	/*
	 *	Stop the interval timer.
	 */
#ifndef SIMULATOR
	COMMPAGE->ckflg = on ? S_GO : S_IDLE;
#endif
	return 0;
}

rtcread()
{
	return(timeofday());
}
E 1
