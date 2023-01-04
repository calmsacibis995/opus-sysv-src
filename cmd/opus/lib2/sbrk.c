/*	@(#)sbrk.c	1.1	*/
extern  char	end;
#ifdef opus
#define MARGIN 0x1000
char	*_end = &end;
#else
static	char	*_end = &end;
#endif

char *
sbrk(incr)
register incr; {
	register char *ptr;

	ptr = _end;
#if opus
	{
		/* align on 32 bit boundary */
		register e;
		e = (int) _end;
		e += 3;
		e &= ~3;
		_end = (char *)e;
		ptr = _end;
	}
	incr += 3;
	incr &= ~3;
	if (brk(_end + incr) < 0)
		return((char *)-1);
#else
	brk(_end + incr);
#endif
	return (ptr);
}

brk(val)
register char *val; {
	register char *ptr;

#ifdef opus
	int dum;
	if ((int *)(val + MARGIN) > &dum)
		return(-1);
#endif
	if (val > _end) {
		ptr = _end;
		while (ptr < val)
			*ptr++ = 0;
	}
	_end = val;
	return (0);
}
