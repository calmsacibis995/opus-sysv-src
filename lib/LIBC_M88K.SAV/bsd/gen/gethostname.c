#include <sys/utsname.h>
#include <sys/errno.h>

_gethostname(host, maxlen)
char *host;
int maxlen;
{
	struct utsname name;
	extern errno;

	if (maxlen < 0) {
		errno = EINVAL;
		return (0);
	}
	if ((unsigned) host > (unsigned) 0xf0000000) {
		errno = EFAULT;
		return 0;
	}
	if (_uname(&name) < 0) {
		return(-1);
	} else {
		_strncpy(host, name.nodename, maxlen);
		return(0);
	}
}
