#include <errno.h>
#include <math.h>
#if m88k	/* bigendian machines */
#define HI 0
#define LO 1
#else		/* littleendian machines */
#define HI 1
#define LO 0
#endif

#define BIAS 1023	/* exponent bias */

/*	frexp
**
**	return X and E (via *eptr) such that value == X * (2 ** E);
**	and 0.5 <= |X| < 1.0
**	if value is 0.0, infinity or NaN, return value, and E = 0;
*/
double
_frexp(value, eptr)
double value;
int *eptr;
{
	union {
		double d;
		unsigned long ul[2];
	} u;
	int exp;

	u.d = value;
	exp = (u.ul[HI] >> 20) & 0x7ff;	/* get abs exponent */

	if (exp == 0) {

		/*  value is 0.0 or is denormalized
		**  first handle 0.0
		*/
		if (u.ul[HI] == 0 && u.ul[LO] == 0) {
			*eptr = 0;
			return value;
		}

		/*  value is denormalized; exponent field is zero
		**  setting exponent to that of 0.5 yields X+0.5.
		**  Then subtract out 0.5.
		*/
		u.ul[HI] |= 0x3fe00000;	/* 0.5 + X */
		u.d -= (double)0.5;

		/*  extract the new exponent and set E
		*/
		exp = (u.ul[HI] >> 20) & 0x7ff;
		*eptr = exp - 2 * (BIAS - 1);

		u.ul[HI] = (u.ul[HI] & 0x800fffff) | 0x3fe00000;
		return u.d;
		
	} else if (exp == 0x7ff) {	/* NaN or infinity */
		*eptr = 0;
		return value;
	}

	*eptr = exp - (BIAS - 1);
	u.ul[HI] = (u.ul[HI] & 0x800fffff) | 0x3fe00000;
	return u.d;
}

/*	ldexp
**
**	returns: value * (2 ** exp)
*/
double
_ldexp(value, exp)
double value;
int exp;
{
	extern int errno;
	int nexp;
	union {
		double d;
		unsigned long ul[2];
	} u;

	u.d = value;
	nexp = ((u.ul[HI] >> 20) & 0x7ff) + exp;
	if (nexp <= 0 || nexp > 0x7fe) {
		errno = ERANGE;
		return HUGE_VAL;
	}
	u.ul[HI] = (u.ul[HI] & 0x800fffff) | (nexp << 20);
	return u.d;
}

/*	modf
**
**	returns signed fractional part of value,
**	and stores the integral part in *iptr.
*/
double
_modf(value, iptr)
double value;
double *iptr;
{
	int exp;
	int mask;
	union {
		double d;
		unsigned long ul[2];
	} u;

	u.d = value;
	exp = ((u.ul[HI] >> 20) & 0x7ff) - BIAS;
	if (exp < 0) {
		*iptr = (double)0.0;
		return value;
	}
	/* if exp==0, then only the phantom mantissa bit is in the integral part
	** if exp>0, then exp is the number of actual bits in the mantissa
	**    that are in the integral part.
	** if exp==20, all the 20 bits in the high part are integral.
	** if exp>=52, all the bits are integral.
	*/
	if (exp < 20) {
		u.ul[HI] &= 0xffffffff << (20 - exp);
		u.ul[LO] = 0;
	} else if (exp == 20) {
		u.ul[LO] = 0;
	} else if (exp < 52) {
		u.ul[LO] &= 0xffffffff << (52 - exp);
	}
	*iptr = u.d;
	return value - u.d;
}
