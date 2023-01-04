#include <fcntl.h>

_creat(path, mode)
char *path;
int mode;
{
	return(_open(path, O_WRONLY|O_CREAT|O_TRUNC, mode));
}

