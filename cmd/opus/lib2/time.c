/*	@(#)time.c	1.1	*/

long	__time;

long
time(tloc)
long *tloc; {

	if (tloc)
		*tloc = __time;
	return (__time);
}
