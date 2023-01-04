/*
 * POSIX/ANSI function _mktime()
 * time_t _mktime(struct tm * tp)
 *
 * Input is a time structure with values corresponding to local time.
 * We need to calculate the ticks correcsponding to this structure
 * (since 1/1/1970 gmt).
 * Then calculate the correct values of the structure members from the
 * calculated value of ticks and store into the structure.
 * Return the ticks value to caller.
 * The return value and the structure both have gmt values
 * If any of the structure members passed in have out of range values
 * nothing is done and we return error (-1).
 */

#include	<ctype.h>
#include  	<stdio.h>
#include  	<sys/types.h>
#include 	<time.h>

#define SEC_IN_DAY	(24 * 60 * 60)
#define	year_size(A)	(((A) % 4) ? 365 : 366)

static int check_struct();

static short	month_size[12] = 
		{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

time_t 
_mktime(tmptr)
register struct tm *tmptr;
{
	register int days;
	int  d, years, j;
	long ticks;
	/* following are used by the SECOND PART */
	register int		d0, d1;
	long			hms, day;

	if (check_struct(tmptr) != NULL) {
		return ((time_t) -1);	/* members may be out of range */
	}

	years = (tmptr->tm_year - 70); 	/* determine the correct year */
	for(d=0, days=0; d < years; d++)
		days += year_size(d);
	if(year_size(d) == 366)
		month_size[1] = 29; 	/* check for leap year */
	d = tmptr->tm_mon;
	for(j=0; j < d; ++j)
		days += month_size[j]; /* add days till the current month */
	days += (tmptr->tm_mday - 1);  /* the last day is not included since
					  it is partial */
	/*
	 * Till now we have computed the total number of days that have
	 * elapsed since 1970 (1/1/1970). Now we convert this into seconds
	 */
	
	ticks = (long) (days * (SEC_IN_DAY));
	/* add hrs min sec converted to seconds */
	ticks += (long) ((tmptr->tm_hour * 60 * 60) + (tmptr->tm_min * 60) + 
		  tmptr->tm_sec);


	/* SECOND PART 
	 * Convert the time in ticks into days, mon, year etc..
	 * and fill up the structure for return value as needed
	 * by _mktime. THIS CODE IS IDENTICAL TO gmtime()
	 * This is actually converse of what we did so far.
	 * Well, POSIX needs it. So give it!!
	 */

	/* Break initial number into days */
	hms = ticks % SEC_IN_DAY;
	day = ticks / SEC_IN_DAY;
	if(hms < 0) {
		hms += SEC_IN_DAY;
		day -= 1;
	}

	/* Generate hours:minutes:seconds */
	tmptr->tm_sec = hms % 60;
	d1 = hms / 60;
	tmptr->tm_min = d1 % 60;
	d1 /= 60;
	tmptr->tm_hour = d1;

	/* Generate day of the week. 
	 * The addend is 4 mod 7 (1/1/1970 was Thursday) 
	 */

	tmptr->tm_wday = (day + 7340036L) % 7;

	/* Generate year number */
	if(day >= 0)
		for(d1=70; day >= year_size(d1); d1++)
			day -= year_size(d1);
	else
		for(d1=70; day < 0; d1--)
			day += year_size(d1-1);
	tmptr->tm_year = d1;
	tmptr->tm_yday = d0 = day;

	/* Generate month */
	if(year_size(d1) == 366)
		month_size[1] = 29;
	for(d1 = 0; d0 >= month_size[d1]; d1++)
		d0 -= month_size[d1];
	month_size[1] = 28;
	tmptr->tm_mday = d0 + 1;
	tmptr->tm_mon = d1;
	tmptr->tm_isdst = 0;

	return(ticks); /* this is the return value of the routine */
}

/*
 * check_struct()
 * Used my _mktime(). 
 * Check the structure pointed by tp and ensure that the members are
 * within specified ranges. Otherwise return a error status.
 */
static
check_struct(tp)
register struct tm *tp;
{
	if (tp->tm_sec < 0 || tp->tm_sec > 59)
		return(~NULL);
	if (tp->tm_min < 0 || tp->tm_min > 59)
		return(~NULL);
	if (tp->tm_hour < 0 || tp->tm_hour > 23)
		return(~NULL);
	if (tp->tm_mon < 0 || tp->tm_mon > 11)
		return(~NULL);
	if (tp->tm_year < 0)
		return(~NULL);

	return (NULL);
}
