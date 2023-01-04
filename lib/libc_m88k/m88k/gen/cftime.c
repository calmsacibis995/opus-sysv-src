
#include <time.h>

int
_cftime(buf, format, t)
char	*buf, *format;
time_t	*t;
{
	return(_ascftime(buf, format, _localtime(t)));
}
