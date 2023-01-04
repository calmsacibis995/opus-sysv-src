/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-m88k:gen/ascftime.c	1.9"
/*LINTLIBRARY*/
/*
 * This routine converts time as follows.  The epoch is 0000  Jan  1
 * 1970  GMT.   The  argument  time  is  in seconds since then.  The
 * localtime(t) entry returns a pointer to an array containing:
 *
 *		  seconds (0-59)
 *		  minutes (0-59)
 *		  hours (0-23)
 *		  day of month (1-31)
 *		  month (0-11)
 *		  year
 *		  weekday (0-6, Sun is 0)
 *		  day of the year
 *		  daylight savings flag
 *
 * The routine corrects for daylight saving time and  will  work  in
 * any  time  zone provided "timezone" is adjusted to the difference
 * between Greenwich and local standard time (measured in seconds).
 *
 *	 _ascftime(buf, format, t)	-> where t is produced by localtime
 *				           and returns a ptr to a character
 *				           string that has the ascii time in
 *				           the format specified by the format
 *				           argument (see date(1) for format
 *				           syntax).
 *
 */

#include	<ctype.h>
#include 	<fcntl.h>
#include	<stdio.h>
#include	<time.h>
#include	<sys/types.h>
#include 	<sys/stat.h>
#define CFTIME_DIR	"/lib/cftime/"
#define LEN_CT_ROOT	sizeof CFTIME_DIR - 1
#define DFL_LANG	"usa_english"
#define MAXLANGLGTH 	64	/* max length of the language string */


extern char	*_getenv(), *_malloc(), *_memchr();
static char	*itoa();
static void	setcftime();
static char	language[MAXLANGLGTH] = DFL_LANG;

static STRinited;
struct _STR 
	{
		unsigned char 
	   aJan, aFeb, aMar, aApr, aMay, aJun, aJul, aAug, aSep, aOct, aNov, aDec,
	   Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec,
	   aSun, aMon, aTue, aWed, aThu, aFri, aSat,
	   Sun, Mon, Tue, Wed, Thu, Fri, Sat,
	   Local_time, Local_date, DFL_FMT,
	   AM, PM,
	LAST
	} _STR;
static char * _cftime[] = {
	"Jan","Feb","Mar","Apr","May","Jun","Jul", "Aug", "Sep","Oct", "Nov", "Dec",
	"January", "February", "March","April",
	"May","June", "July", "August", "September",
	"October", "November", "December",
	"Sun","Mon", "Tue", "Wed","Thu", "Fri","Sat",
	"Sunday","Monday","Tuesday","Wednesday", "Thursday","Friday","Saturday",
	"%H:%M:%S","%m/%d/%y", "%a %b %e %T %Z %Y",
	"AM", "PM"};

int
_ascftime(buf, format, tm)
char	*buf, *format;
struct tm	*tm;
{
	register char	*cp, *p,  c;
	register int	r;
	int		i,ret_val=0, temp;
	char		*ptr;

	if (!STRinited) {
		cp = (char *) &_STR;
		for ( r = 0; r < sizeof(struct _STR) ; r++)
			*cp++ = r;
		STRinited = 1;
	}
	setcftime();
	/* Set format string, if not already set */
	if (format == NULL || *format == '\0') 
		if (((format = _getenv("LC_TIME")) == 0) || *format == 0)
			format =  _cftime[_STR.DFL_FMT];


	/* Build date string by parsing format string */
	cp = buf;
	while ((c = *format++) != '\0') {
		if (c == '%') {
			switch (*format++) {
			case '%':	/* Percent sign */
				*cp++ = '%';
				continue;
			case 'a':	/* Abbreviated weekday name */
				for(p = _cftime[_STR.aSun + tm->tm_wday]; *p != '\0';p++)
					*cp++ = *p;
				continue;
			case 'A':	/* Weekday name */
				for(p =  _cftime[_STR.Sun + tm->tm_wday]; *p != '\0';p++)
					*cp++ = *p;
				continue;
			case 'h':
			case 'b':	/* Abbreviated month name */
				for(p = _cftime[_STR.aJan + tm->tm_mon];*p != '\0';p++)
					*cp++ = *p;
				continue;
			case 'B':	/* Month name */
				for(p = _cftime[_STR.Jan + tm->tm_mon];*p != '\0'; p++)
					*cp++ = *p;
				continue;
			case 'c':	/* Date and time */
#if m88k
				/* modified to be consistent with date(1) */
				cp += ascftime(cp,"%m/%d/%y %H:%M:%S",tm);
#else
				cp += ascftime(cp,"%m/%d/%y %I:%M %p",tm);
#endif
				continue;
			case 'd':	/* Day number */
				cp = itoa(tm->tm_mday, cp, 2);
				continue;
			case 'D':	/* Shorthand for %m/%d/%y */
				cp += _ascftime(cp,"%m/%d/%y",tm);
				continue;
			case 'e':       /* Day number without leading zero */
				if (tm->tm_mday < 10) {
					*cp++ = ' ';
                                	cp = itoa(tm->tm_mday, cp, 1);
				}
				else
					cp = itoa(tm->tm_mday, cp, 2);
                                continue;
			case 'H':	/* Hour (24 hour version) */
				cp = itoa(tm->tm_hour, cp, 2);
				continue;
			case 'I':	/* Hour (12 hour version) */
				cp = itoa(tm->tm_hour > 12 ? tm->tm_hour - 12 : tm->tm_hour, cp, 2);
				continue;
			case 'j':	/* Julian date */
				cp = itoa(tm->tm_yday + 1, cp, 3);
				continue;
			case 'm':	/* Month number */
				cp = itoa(tm->tm_mon + 1, cp, 2);
				continue;
			case 'M':	/* Minute */
				cp = itoa(tm->tm_min, cp, 2);
				continue;
			case 'n':	/* Newline */
				*cp++ = '\n';
				continue;
			case 'p':	/* AM or PM */
				if (tm->tm_hour >= 12) 
					for (p = _cftime[_STR.PM]; *p != '\0'; p++)
						*cp++ = *p;
				else
					for (p = _cftime[_STR.AM]; *p != '\0'; p++)
                                                *cp++ = *p;
				continue;
			case 'r':	/* Shorthand for %I:%M:%S AM or PM */
				cp += _ascftime(cp,"%I:%M:%S %p",tm);
				continue;
			case 'R':	/* Time as %H:%M */
				cp += _ascftime(cp,"%H:%M",tm);
				continue;
			case 'S':	/* Seconds */
				cp = itoa(tm->tm_sec, cp, 2);
				continue;
			case 't':	/* Tab */
				*cp++ = '\t';
				continue;
			case 'T':	/* Shorthand for %H:%M:%S */
				cp += _ascftime(cp,"%H:%M:%S",tm);
				continue;
			case 'U':	/* Weekday number, taking Sunday as
					 * the first day of the week */
#if 1
/*
 * Modified for ANSI
 */
				i = 0;
				if ((tm->tm_yday - tm->tm_wday) % 7)
					i = tm->tm_wday + 1;
				i = (tm->tm_yday + 7 - i) / 7;
				cp = itoa(i,cp,2);
				continue;
#else
/*
 * This is the AT&T code
 */
				temp = tm->tm_yday - tm->tm_wday;
				if (temp >= -3 ) {
					i = (temp + 1) / 7 + 1;	/* +1 for - tm->tm_wday */
					if (temp % 7 >= 4) i++;
				}
				else i = 52;
				cp = itoa(i,cp,2);
				continue;
#endif
			case 'w':	/* Weekday number */
				cp = itoa(tm->tm_wday, cp, 1);
				continue;
			case 'W':	/* Week number of year, taking Monday as
					 * first day of week */
#if 1
/*
 * Modified for ANSI
 */
				i = 0;
				if ((tm->tm_yday + 1 - tm->tm_wday) % 7)
					i = tm->tm_wday + 1;
				i = (tm->tm_yday + 7 - i) / 7;
				cp = itoa(i,cp,2);
				continue;
#else
/*
 * This is the AT&T code
 */
				if (tm->tm_wday == 0)
					temp = tm->tm_yday - 6;
				else
					temp = tm->tm_yday - tm->tm_wday + 1;
				if (temp >= -3 ) {
					i = (temp + 1) / 7 + 1;	/* 1 for 
								   -tm->tm_wday */
					if (temp % 7 >= 4) i++;
				}
				else  i = 52; /* less than 4 days in the first
							week causes it to belong to
							the tail of prev year */
				cp = itoa(i, cp, 2);
				continue;
#endif
			case 'x':	/* Localized date format */
				cp += _ascftime(cp, _cftime[_STR.Local_date], tm);
				continue;
			case 'X':	/* Localized time format */
				cp += _ascftime(cp, _cftime[_STR.Local_time], tm);
				continue;
			case 'y':	/* Year in the form yy */
				cp = itoa(tm->tm_year, cp, 2);
				continue;
			case 'Y':	/* Year in the form ccyy */
				cp = itoa(1900 + tm->tm_year, cp, 4);
				continue;
			case 'Z':	/* Timezone */
				for(p = tzname[tm->tm_isdst]; *p != '\0'; p++)
					*cp++ = *p;
				continue;
			default:
				*cp++ = c;
				*cp++ = *(format - 1);
				continue;
			}
		}
	 	*cp++ = c;
	}
	ret_val = (int)(cp - buf);
	*cp = '\0';
	return(ret_val);
}

static void
setcftime()
{
	register char *p;
	register int j;
	char *mystr[sizeof(struct _STR)], *file,  *q;
	static char *ostr = (char *)0 ;
	char *str;
	struct stat buf;
	static char pathname[128] = CFTIME_DIR;
	int fd;


	if ((file = _getenv("LANGUAGE")) == 0 || file[0] == '\0')
	{
		/* Use the default _cftime[] */

		if (pathname[LEN_CT_ROOT] == '\0')
			return;
		file = language;
	}	
	else if (_strcmp(file,language) == 0)
		return;

	(void) _strcpy(&pathname[LEN_CT_ROOT],file);

	if ( (fd = _open(pathname,O_RDONLY)) == -1)
		return;

	if( (_fstat(fd,&buf)) != 0)
		return;


	if ( (str = _malloc(buf.st_size + 2)) == NULL )
		return;

	if ( (_read(fd, str, buf.st_size)) != buf.st_size)
	{
		_free(str);
		return;
	}

	/* Set last character of str to '\0' */
	q = &str[buf.st_size];
	q[0] = '\n';
	q[1] = '\0';

	/* p will "walk thru" str */
	p = str;  	

	j = -1;
	while (*p != '\0')
	{ 
		/* "Look for a newline, i.e. end of sub-string
		 * and  change it to a '\0'. If LAST pointers
		 * have been set in mystr, but the newline hasn't been seen
		 * yet, keep going thru the string leaving mystr alone.
		 */
		if (++j < _STR.LAST) 
			mystr[j] = p;
		p = _memchr(p,'\n',buf.st_size + 2);
		*p++ = '\0';
	}
	if (j == _STR.LAST)
	{
		_memcpy(_cftime, mystr,sizeof(mystr)); 
		_strncpy(language, file, MAXLANGLGTH);
		if (ostr != 0)	 /* free the previoulsy allocated local array*/
			_free(ostr);
		ostr = str;
	}
	(void) _close(fd);
	return;
	
}

static char *
itoa(i, ptr, dig)
register int	i;
register char	*ptr;
register int	dig;
{
	switch(dig) {
	case 4:
		*ptr++ = i / 1000 + '0';
		i = i - i / 1000 * 1000;
	case 3:
		*ptr++ = i / 100 + '0';
		i = i - i / 100 * 100;
	case 2:
		*ptr++ = i / 10 + '0';
	case 1:
		*ptr++ = i % 10 + '0';
	}

	return(ptr);
}
