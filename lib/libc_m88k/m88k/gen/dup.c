#include <fcntl.h>

_dup(fildes)
int fildes;
{
	return(_fcntl(fildes,F_DUPFD,0));
}
