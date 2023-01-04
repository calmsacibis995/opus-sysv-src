#ifndef __Itime
#define __Itime

#define NULL 0

#ifdef	m88k
#define CLK_TCK	(sysconf(3))
#else
#define	CLK_TCK	64		/* Ticks/second of the clock */
#endif

typedef unsigned int size_t;
typedef long clock_t;
typedef long time_t;

struct	tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

struct timeval {
	time_t	tv_sec;
	long	tv_usec;
};

struct itimerval {
	struct timeval	it_interval;
	struct timeval	it_value;
};

#define ITIMER_REAL	0
#define ITIMER_VIRTUAL	1
#define ITIMER_PROF	2

#define CLOCKS_PER_SECOND 1000000

clock_t clock(void);
double difftime(time_t, time_t);
time_t mktime(struct tm *);
time_t time(time_t *);
char *asctime(const struct tm *);
char *ctime(const time_t *);
struct tm *gmtime(const time_t *);
struct tm *localtime(const time_t *);
size_t strftime(char *, size_t, const char *, const struct tm *);

void tzset(void);

extern long timezone;
extern int daylight;
extern char *tzname[];

#endif
