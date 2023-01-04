#include <sys/types.h>
#include <sys/time.h>

_alarm (seconds)
unsigned seconds;
{
	register struct itimerval *locitimptr;
	struct itimerval locitimval;
	struct itimerval retitimer;

	locitimptr = &locitimval;
	locitimptr->it_interval.tv_sec = 0;
	locitimptr->it_interval.tv_usec = 0;
	locitimptr->it_value.tv_sec = seconds;
	locitimptr->it_value.tv_usec = 0;
	
	_setitimer(ITIMER_REAL,locitimptr,&retitimer);

	if (retitimer.it_value.tv_sec == 0 && retitimer.it_value.tv_usec)
		return 1;

	if (retitimer.it_value.tv_usec > 500000)
		return(retitimer.it_value.tv_sec + 1);

	return(retitimer.it_value.tv_sec);
}
