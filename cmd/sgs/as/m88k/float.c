/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident "$Header: float.c 2.2 88/04/08 14:03:12 root Exp $ RISC Source Base"

#include "systems.h"
#include "dbug.h"

#if FLOAT

#include <ctype.h>
#include <symbols.h>		/* needed for instab.h */
#include "instab.h"
#include "program.h"

#define SINGLEBIAS (127)
#define DOUBLEBIAS (1023)

extern double lclatof ();
#ifdef MOTOROLA_CROSS
extern double floor ();
/* extern double pow (); */
extern double log2 ();
#endif

int atofs (string, ret)
char *string;
unsigned long *ret;
{
    void fpdtos ();
    int rtnval = 1;

    DBUG_ENTER ("atofs");
    if (!atofd (string, ret)) {
	rtnval = 0;
	fpdtos (ret);
    }
    DBUG_RETURN (rtnval);
}

#ifndef MOTOROLA_CROSS
/*
**	The Motorola cross-environment routines (following these)
**	have problems.  We can just use atof().
*/
extern double atof();
union dl {
	double d;
	long l[2];
};

int
atofd(string, ret)
char *string;
unsigned long *ret;
{
	union dl dl;

	DBUG_ENTER("atofd");
	dl.d = atof(string);
	ret[0] = dl.l[0];
	ret[1] = dl.l[1];
	DBUG_RETURN(0);
}

/*ARGSUSED*/
int
atofx(string, lp)
char *string;
OPERAND *lp;
{
	DBUG_ENTER("atofx");
	DBUG_RETURN(0);
}

double
lclatof(p)
register char *p;
{
	DBUG_ENTER("lclatof");
	DBUG_RETURN(atof(p));
}

#else	/* MOTOROLA_CROSS */
int atofd (string, ret)
char *string;
unsigned long *ret;
{
    double dbl;
    double fexp;
    double two16 = 65536.0;
    long sbit;
    long exponent;
    long mantissa;
    short leadbit = 0;

    /*
     *	Some of the cast operations, namely
     *	double -> float and double -> long  don't necessarily
     *	have the same implementation on various machines
     *	that is why the following two variables are defined.
     *	Any time there are used in this file it is to get
     *	around the cast operator.
     */
    double dtmp;		/*  dummy var needed for casting */
    float ftmp;			/*  dummy var needed for casting */
    long i;
    long nf;

    DBUG_ENTER ("atofd");
    dbl = lclatof (string);
    if (dbl == 0.0) {
	ret[0] = 0x0L;
	ret[1] = 0x0L;
	DBUG_RETURN (0);
    }
    if (dbl > 0.0) {
	sbit = 0L;
    } else {
	sbit = 1L;
	dbl *= -1.0;
    }
    /*
     *	Find out how many two's are in the floating point number ('dbl')
     *	then divide 'dbl' by 2**(number of 2's) (by multiplying
     *	by its reciprical) in order to scale it down within the range
     *	1.0 =< dbl =< 2.0
     *	Note: Due to rounding and imprecise results the resulting
     *	number may be slightly < 1.0 or slightly > 2.0
     *	which is why we must check for these cases.
     */
    fexp = floor (log2 (dbl));
    /*
     * The following line of code is commented out due to some problems
     * with the precision of the pow function. It is replaced by the
     * following while loop and if stmt.
     *
     dbl *= pow((double)2.0,-fexp);
     */
    i = (long) fexp;
    nf = 0;
    if (i < 0) {
	nf = 1;
	i = -i;
    }
    dtmp = (double) 1.0;
    while (i-- > 0L) {
#if ns32000
#define MAXDOUBLE	1.79769313486230e+308
	if (dtmp > (MAXDOUBLE/2)) dtmp = MAXDOUBLE/2;
#endif
	dtmp *= (double) 2.0;
    }
    if (nf == 0) {
	dbl *= ((double) 1.0 / dtmp);
    } else {
	dbl *= dtmp;
    }
    if (dbl >= 2.0) {
	dbl /= (double) 2.0;
	++fexp;
    } else if (dbl < 1.0) {
	dbl *= (double) 2.0;
	--fexp;
    }
    DBUG_PRINT ("flt", ("fexp = %f dbl = %.13e  %lx %lx", fexp, dbl, dbl));
    if (fexp > -(DOUBLEBIAS - 1)) {		/* normalized numb */
	exponent = (long) fexp + (long) DOUBLEBIAS;
    } else {
	/* denormalized numb, must adjust mantissa
	 * e.g. a number like 1.0E-1025 will be coded
	 * as 0.001E-1022 with some precision lost
	 */
	exponent = 0L;
#ifdef NOTUSEDANYMORE
	fexp += (double)(DOUBLEBIAS-1);
	dbl *= pow((double)2.0, fexp);
#endif
	DBUG_PRINT ("flt", ("denorm numb - dbl= %.13e fexp = %f", dbl, fexp));
    }
    if (exponent != 0L) {
	leadbit = 1;
    }
    DBUG_PRINT ("flt", ("leadbit = %d", leadbit));

    /*
     *	Multiply by 2**20 to get the first 20 bits of the fraction
     *	necessary to fill in the first word.
     *	Then subtract out this number and multiply by 2**32 to
     *	get the remaining fractional part for the second word of the double
     */

    dtmp = (dbl - (double) leadbit) * (double) (1L << 20);
				/* mult by 2**20 */
    ftmp = floor (dtmp);	/* takes the place of a cast */
    mantissa = ftmp;		/* takes the place of a cast */
    ret[0] = (sbit << 31) | (exponent << 20) | mantissa;
    DBUG_PRINT ("flt", ("sbit = %ld, (exp<<20) = %lx, mantissa = %lx",
	    sbit, (exponent << 20), mantissa));
    DBUG_PRINT ("flt", ("dtmp = %.13e, ftmp = %.13e, mantissa = %ld",
	    dtmp, ftmp, mantissa));
    dtmp -= mantissa;
    dtmp *= two16;
    ftmp = floor (dtmp);	/* takes the place of a cast */
    ret[1] = ftmp;
    dtmp -= ret[1];
    dtmp *= two16;
    ftmp = dtmp;		/* takes the place of a cast */
    mantissa = ftmp;		/* takes the place of a cast */
    ret[1] = (ret[1] << 16) | mantissa;
    DBUG_PRINT ("flt", ("result (val,val2)= %08lx  %08lx", ret[0], ret[1]));
    DBUG_RETURN (0);
}

/*ARGSUSED*/
int atofx (string, lp)
char *string;
OPERAND *lp;
{
    DBUG_ENTER ("atofx");
    DBUG_RETURN (0);
}

double lclatof (p)
register char *p;
{
    register char c;
    double fl;
    double flexp;
    double exp5;
    /*	'big' is (2 ^ number of significant bits in a fp number) */
    double big = 4503599627370496.0;	/* 2^52 */
    double ldexp ();
    register int eexp;
    register int exp;
    register int bexp;
    register short neg;
    register short negexp;

    DBUG_ENTER ("lclatof");
    neg = 1;
    while ((c = *p++) == ' ') {;}
    if (c == '-') {
	neg = -1;
    } else if (c == '+') {
	;
    } else {
	--p;
    }
    exp = 0;
    fl = 0;
    while ((c = *p++), isdigit (c)) {
	if (fl < big) {
	    fl = 10 * fl + (c - '0');
	} else {
	    exp++;
	}
    }
    if (c == '.') {
	while ((c = *p++), isdigit (c)) {
	    if (fl < big) {
		fl = 10 * fl + (c - '0');
		exp--;
	    }
	}
    }
    negexp = 1;
    eexp = 0;
    if ((c == 'E') || (c == 'e')) {
	if ((c = *p++) == '+') {
	    ;
	} else if (c == '-') {
	    negexp = -1;
	} else {
	    --p;
	}
	while ((c = *p++), isdigit (c)) {
	    eexp = 10 * eexp + (c - '0');
	}
	if (negexp < 0) {
	    eexp = -eexp;
	}
	exp = exp + eexp;
    }
    negexp = 1;
    if (exp < 0) {
	negexp = -1;
	exp = -exp;
    }
    flexp = 1;
    exp5 = 5;
    bexp = exp;
    while (1) {
	if (exp & 01) {
	    flexp *= exp5;
	}
	exp >>= 1;
	if (exp == 0) {
	    break;
	}
	exp5 *= exp5;
    }
    if (negexp < 0) {
	fl /= flexp;
    } else {
	fl *= flexp;
    }
    fl = ldexp (fl, negexp * bexp);
    DBUG_RETURN ((neg < 0) ? -fl : fl);
}

double log2 (x)
double x;
{
    extern double log ();

    DBUG_ENTER ("log2");
    DBUG_RETURN (log (x) / log ((double) 2.0));
}
#endif	/* MOTOROLA_CROSS */

int validfp (p)
register char *p;
{
    register int c;
    char integer;
    char decimal;
    char frac;
    char e;
    char exp;
    int rtnval;

    DBUG_ENTER ("validfp");
    /* check for sign */
    if ((*p == '-') || (*p == '+')) {
	++p;
    }
    /* check for integral part */
    integer = 0;
    while ((c = *p++), isdigit (c)) {
	integer = 1;
    }
    /* check for decimal pt. */
    decimal = 0;
    if (c == '.') {
	decimal = 1;
	/* check for fraction */
	frac = 0;
	while ((c = *p++), isdigit (c)) {
	    frac = 1;
	}
    }
    /* check for exponent */
    e = 0;
    if ((c == 'E') || (c == 'e')) {
	e = 1;
	if ((*p == '-') || (*p == '+')) {
	    ++p;
	}
	exp = 0;
	while ((c = *p++), isdigit (c)) {
	    exp = 1;
	}
    }

    /* 
     * are all the necessary parts here?
     * invalid fp numbers look like:
     *	[digits]	(no decimal)
     *	.		(no digits)
     *	.[eE]
     *	.[eE][digits]
     *
     *	The ENTIRE string must be valid so 'c' should = NULL
     *
     * All else is assumed valid
     */
    if ((decimal && !integer && !frac) ||
	    (integer && !decimal && !e) ||
	    (e && !exp) || (c != '\0')) {
	rtnval = 0;		/* bad fp number */
    } else {
	rtnval = 1;		/* valid fp number */
    }
    DBUG_RETURN (rtnval);
}


/*
 *
 *	round double fp number to single by:
 *		- get high 3 bits of second word, these will go into
 *			new single word since the exponent shrinks to 8 bits
 *		- get guard bits (next 2 bits after high 3 bits)
 *		- if both guard bits equal 1 then add 1 to hi3bits
 *			note: if this add produces a carry it should
 *				promulgate into the mantissa however for the
 *				first cut we won't promulgate it.
 *
 */

void fpdtos (fpval)
unsigned long *fpval;
{
    long sign;
    long exp;
    long mantissa;
    long hi3bits;
    long guard;

    DBUG_ENTER ("fpdtos");
    sign = fpval[0] & (1L << 31);
    exp = (fpval[0] >> 20) & 0x7ffL;
    if (exp != 0L) {
	exp = (exp - (long) DOUBLEBIAS) + (long) SINGLEBIAS;
    }
    DBUG_PRINT ("flt", ("sign = %lx, old = %lx, new exp = %lx", sign,
	    ((fpval[0] >> 20) & 0x7ffL), exp));
    mantissa = fpval[0] & 0xfffffL;
    hi3bits = (fpval[1] >> 29) & 0x7L;
    guard = (fpval[1] >> 27) & 0x3L;
/* Comment out.  kss  9/26/88 */
/*  if (guard & 0x3L) {	
	if (!(hi3bits & 0x7L)) {	
	    hi3bits += 1;
	} 
*/
/* Replace with ...   kss 9/26/88 */
    if (guard & 0x3L) {		/* round up */
	    hi3bits += 1;
    }

    DBUG_PRINT ("flt", ("hi3bits = %lx, guard = %lx", hi3bits, guard));
    DBUG_PRINT ("flt", ("old man = %lx, new man = %lx", 
	    mantissa, (mantissa << 3) | hi3bits));
    mantissa = (mantissa << 3) | hi3bits;
    fpval[0] = 0L;
    fpval[0] = sign | (exp << 23) | mantissa;
    DBUG_PRINT ("flt", ("new expval = %lx", fpval[0]));
    DBUG_VOID_RETURN;
}

/*
 *
 *	This function transfares ascii decimal integer to 
 *	packed decimal in the format of the MAU triple word.
 *
 */

void atodi (str, fpval)
char *str;
unsigned long *fpval;
{
    register char *p;
    register int shift;
    register int digit;
    register int i;

    DBUG_ENTER ("atodi");
    fpval[0] = 0;
    fpval[1] = 0;
    fpval[2] = 0;

    p = str;
    while (*(++p)) {;}
    p--;
    switch (*str) {
	case '-': 
	    fpval[2] = 0xb;
	    str++;
	    break;
	case '+': 
	    fpval[2] = 0xa;
	    str++;
	    break;
	default: 
	    fpval[2] = 0xa;
    }
    for (digit = 1; p >= str; ++digit) {
	i = 2 - (digit / 8);	/* offset into fpval */
	shift = 4 * (digit % 8);/* shift to nybble */
	fpval[i] |= (((*p--) - '0') << shift);
    }
    DBUG_VOID_RETURN;
}
#endif
