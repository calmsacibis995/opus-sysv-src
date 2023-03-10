/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libm:hypot.c	1.13"
/*LINTLIBRARY*/
/*
 *	hypot(a, b) returns sqrt(a^2 + b^2), avoiding unnecessary overflows.
 *	Returns ERANGE error and value HUGE if the correct value would overflow.
 */

#include <math.h>
#include <values.h>
#include <errno.h>
#define ITERATIONS	4

double
hypot(a, b)
register double a, b;
{
	register double t;
	register int i = ITERATIONS;
	struct exception exc;

	if ((exc.arg1 = a) < 0)
		a = -a;
	if ((exc.arg2 = b) < 0)
		b = -b;
	if (a > b) {				/* make sure |a| <= |b| */
		t = a;
		a = b;
		b = t;
	}
	if(b == 0.0) return(b);			/* both args are 0, result=0 */
	if ((t = a/b) < X_EPS) {			/* t <= 1 */
		if (b == _MAXDOUBLE)
			goto huge_out;
		return (b);			/* t << 1 */
	}
	a = 1 + t * t;				/* a = 1 + (a/b)^2 */
	t = 0.5 + 0.5 * a;			/* first guess for sqrt */
	do {
		t = 0.5 * (t + a/t);
	} while (--i > 0);			/* t <= sqrt(2) */
#if m88k
	if (b < _MAXDOUBLE/M_SQRT2) {		/* result can't overflow */
#else
	if (b < MAXDOUBLE/M_SQRT2) {		/* result can't overflow */
#endif
		if (t * b == _MAXDOUBLE)
			goto huge_out;
		return (t * b);
	}
#if m88k
	if ((t *= 0.5 * b) < _MAXDOUBLE/2) {	/* result won't overflow */
#else
	if ((t *= 0.5 * b) < MAXDOUBLE/2) {	/* result won't overflow */
#endif
		if (t + t == _MAXDOUBLE)
			goto huge_out;
		return (t + t);
	}
huge_out:
	exc.type = OVERFLOW;
	exc.name = "hypot";
	exc.retval = HUGE;
	if (!matherr(&exc))
		errno = ERANGE;
	return (exc.retval);
}
