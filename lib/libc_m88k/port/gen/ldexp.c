/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/ldexp.c	2.12"
/*LINTLIBRARY*/
/*
 *	double ldexp (value, exp)
 *		double value;
 *		int exp;
 *
 *	Ldexp returns value * 2**exp, if that result is in range.
 *	If underflow occurs, it returns zero.  If overflow occurs,
 *	it returns a value of appropriate sign and largest single-
 *	precision magnitude.  In case of underflow or overflow,
 *	the external int "errno" is set to ERANGE.  Note that errno is
 *	not modified if no error occurs, so if you intend to test it
 *	after you use ldexp, you had better set it to something
 *	other than ERANGE first (zero is a reasonable value to use).
 */

#include "shlib.h"
#include <values.h>
#include <errno.h>
/* Largest signed long int power of 2 */
#define MAXSHIFT	(BITSPERBYTE * sizeof(long) - 2)

extern double _frexp();

double
_ldexp(value, exp)
register double value;
register int exp;
{
	int old_exp;
#if m88k	/* force runtime computation to avoid cumulative atof errors */
	double mulval;
	int maxbit;

	maxbit = 1L << MAXSHIFT;
	mulval = 1.0 / maxbit;
#endif

	if (exp == 0 || value == 0.0) /* nothing to do for zero */
		return (value);
#if	!(pdp11 || M32)	/* pdp11 "cc" can't handle cast of
				   double to void on pdp11 or M32 */
	(void)
#endif
	_frexp(value, &old_exp);
	if (exp > 0) {
		if (exp + old_exp > MAXBEXP) { /* overflow */
			errno = ERANGE;
#if m88k
			return (value < 0 ? -_MAXDOUBLE : _MAXDOUBLE);
#else
			return (value < 0 ? -MAXFLOAT : MAXFLOAT);
#endif
		}
#if m88k
		for ( ; exp > MAXSHIFT; exp -= MAXSHIFT)
			value *= maxbit;
#else
		for ( ; exp > MAXSHIFT; exp -= MAXSHIFT)
			value *= (1L << MAXSHIFT);
#endif
		return (value * (1L << exp));
	}
	if (exp + old_exp < MINBEXP) { /* underflow */
		errno = ERANGE;
		return (0.0);
	}
#if m88k
	for ( ; exp < -MAXSHIFT; exp += MAXSHIFT)
		value *= mulval; /* mult faster than div */
#else
	for ( ; exp < -MAXSHIFT; exp += MAXSHIFT)
		value *= 1.0/(1L << MAXSHIFT); /* mult faster than div */
#endif
	return (value / (1L << -exp));
}
