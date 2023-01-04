#include	"netdb.h"

_gethostid()
{
	int *p;
	struct hostent *hp, *_gethostbyname();
	int inaddr = 0;
	char hbuf[256];

	_memset(hbuf, 0, sizeof(hbuf));
	if (_gethostname(hbuf, sizeof(hbuf)))
		return (0);

	hp = _gethostbyname(hbuf);
	if (hp) {
		p = (int *) hp->h_addr;
		inaddr = *p;
	}
	return(inaddr);
}
