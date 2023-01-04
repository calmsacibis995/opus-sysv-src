/*	@(#)stime.c	1.1	*/

stime(tloc)
long *tloc; {
	extern long __time;

	__time = *tloc;
}
