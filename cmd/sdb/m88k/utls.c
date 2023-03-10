/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)sdb:u3b2/utls.c	1.3"

#include	<stdio.h>
#include	"dis.h"
#include	"sgs.h"


#define WORDSIZE	(sizeof(int))
union	word
	{
		char		b[WORDSIZE];
		unsigned short	s[WORDSIZE/2];
		unsigned int	w;
		unsigned long	l;
		float		f;
	};


/*
 *	compoff (lng, temp)
 *
 *	This routine will compute the location to which control is to be
 *	transferred.  'lng' is the number indicating the jump amount
 *	(already in proper form, meaning masked and negated if necessary)
 *	and 'temp' is a character array which already has the actual
 *	jump amount.  The result computed here will go at the end of 'temp'.
 *	(This is a great routine for people that don't like to compute in
 *	hex arithmetic.)
 */

compoff(lng, temp)
long	lng;
char	*temp;
{
	extern	int	oflag;	/* from _extn.c */
	extern	long	loc;	/* from _extn.c */

	lng += loc;
	if (oflag)
		sprintf(temp,"%s <%lo>",temp,lng);
	else
		sprintf(temp,"%s <%lx>",temp,lng);
	return(lng);		/* Return address calculated.	*/
}
/*
 *	convert (num, temp, flag)
 *
 *	Convert the passed number to either hex or octal, depending on
 *	the oflag, leaving the result in the supplied string array.
 *	If  LEAD  is specified, preceed the number with '0' or '0x' to
 *	indicate the base (used for information going to the mnemonic
 *	printout).  NOLEAD  will be used for all other printing (for
 *	printing the offset, object code, and the second byte in two
 *	byte immediates, displacements, etc.) and will assure that
 *	there are leading zeros.
 */

convert(num,temp,flag)
unsigned	num;
char	temp[];
int	flag;

{
	extern	int	oflag;		/* in _extn.c */

	if (flag == NOLEAD)
		(oflag) ?	sprintf(temp,"%06o",num):
				sprintf(temp,"%04x",num);

	if (flag == LEAD)
		(oflag) ?	sprintf(temp,"0%o",num):
				sprintf(temp,"0x%x",num);
}


/*
 *	dfpconv(fpword1, fpword2, fpdoub, fpshort)
 *
 *	This routine will convert the 2 longs (64 bit) "fpword1 fpword2" double  
 *	precision floating point representation of a number into its decimal 
 *	equivalent. The result will be stored in *fpdoub. The routine will
 *	return a value indicating what type of floating point number was
 *	converted.
 *	*NOTE*	The conversion routine will calculate a decimal value
 *		if the disassembler is to run native mode on the 3B.
 *		If the 3B disassembler is to be run on a DEC processor
 *		(pdp11 or vax) the routine will store the exponent in
 *		*fpshort. The mantissa will be stored in the form:
 *		"T.fraction" where "T" is the implied bit and the
 *		fraction is of radix 2. The mantissa will be stored
 *		in *fpdoub. This is due to the difference in range
 *		of floating point numbers between the 3B and DEC
 *		processors.
 */
dfpconv(fpword1, fpword2, fpdoub, fpshort)
long	fpword1, fpword2;
double	*fpdoub;
short	*fpshort;
{
	unsigned short exponent;
	short	leadbit, signbit;
	double	dtemp, dec2exp, mantissa;
	long	ltemp;

	exponent = (unsigned short)((fpword1>>20) & 0x7ffL);
	/* exponent is bits 1-11 of the double	*/

	ltemp = fpword1 & 0xfffffL;	/* first 20 bits of mantissa */
	mantissa = ((double)ltemp * TWO_32) + (double)fpword2;
	/* mantissa is bits 12-63 of the double	*/

	signbit = (short)((fpword1>>31) & 0x1L);
	/* sign bit (1-negative, 0-positive) is bit 0 of double	*/

	leadbit = 1;
	/* implied bit to the left of the decimal point	*/

	if (exponent==2047)
		if (mantissa)
			return(NOTANUM);
		else
			return((signbit)? NEGINF: INFINITY);

	if (exponent==0)
		if (mantissa)
		/* This is a denormalized number. The implied bit to 
		 * the left of the decimal point is 0.
		 */
			leadbit = 0;
		else
			return((signbit)? NEGZERO: ZERO);

	/*
	 * Convert the 52 bit mantissa into "I.fraction" where
	 * "I" is the implied bit. The 52 bits are divided by
	 * 2 to the 52rd power to transform the mantissa into a
	 * fraction. Then the implied bit is added on.
	 */
	dtemp = (double)(leadbit + (mantissa/TWO_52));

	/* Calculate 2 raised to the (exponent-BIAS) power and
	 * store it in a double.
	 */
	if (exponent < DBIAS)
		for(dec2exp=1; exponent < DBIAS; ++exponent)
			dec2exp /= 2;
	else
		for(dec2exp=1; exponent > DBIAS; --exponent)
			dec2exp *= 2;

	/* Multiply "I.fraction" by 2 raised to the (exponent-BIAS)
	 * power to obtain the decimal floating point number.
	 */
	*fpdoub = dtemp *dec2exp;

	if (signbit)
		*fpdoub = -(*fpdoub);
	return(FPNUM);
}


get1byte(idsp)
	int	idsp;
{
	extern	long	loc;			/* from _extn.c */
	extern  int	trace;
	extern	unsigned short cur1byte;		/* from _extn.c */
	extern	int	chkget();		/* from access.c */
	union	word	word;

	word.w  = chkget(loc,idsp);
	cur1byte = ((unsigned short)word.b[0]) & 0xff;
	loc++;
}


/*
 *	printline ()
 *
 *	Print the disassembled line, consisting of the object code
 *	and the mnemonics.  The breakpointable line number, if any,
 *	has already been printed, and 'object' contains the offset
 *	within the section for the instruction.
 */

printline()

{
	extern	char	mnem[];

	printf("%s",mnem);
}


/*
 *	sfpconv(fprep, fpdoub)
 *
 *	This routine will convert the long "fprep" single precision 
 *	floating point representation of a number into its decimal 
 *	equivalent. The result will be stored in *fpdoub. The routine
 *	will return a value indicating what type of floating point
 *	number was converted.
 */
sfpconv(fprep, fpdoub)
long fprep;
double *fpdoub;
{
	unsigned short exponent;
	short	leadbit, signbit;
	long	mantissa;
	double	dtemp, dec2exp;

	exponent = (unsigned short)((fprep>>23) & 0xffL);
	/* exponent is bits 1-8 of the long	*/

	mantissa = fprep & 0x7fffffL;
	/* mantissa is bits 9-31 of the long	*/

	signbit = (fprep>>31) & 0x1L;
	/* sign bit (1-negative, 0-positive) is bit 0 of long	*/

	leadbit = 1;
	/* implied bit to the left of the decimal point	*/

	if (exponent==255)
		if (mantissa)
			return(NOTANUM);
		else
			return((signbit)? NEGINF: INFINITY);

	if (exponent==0)
		if (mantissa)
		/* This is a denormalized number. The implied bit to 
		 * the left of the decimal point is 0.
		 */
			leadbit = 0;
		else
			return((signbit)? NEGZERO: ZERO);

	/*
	 * Convert the 23 bit mantissa into "I.fraction" where
	 * "I" is the implied bit. The 23 bits are divided by
	 * 2 to the 23rd power to transform the mantissa into a
	 * fraction. Then the implied bit is added on.
	 */
	dtemp = (double)(leadbit + (double)mantissa/TWO_23);

	/* Calculate 2 raised to the (exponent-BIAS) power and
	 * store it in a double.
	 */
	if (exponent < BIAS)
		for(dec2exp=1; exponent < BIAS; ++exponent)
			dec2exp /= 2;
	else
		for(dec2exp=1; exponent > BIAS; --exponent)
			dec2exp *= 2;

	/* Multiply "I.fraction" by 2 raised to the (exponent-BIAS)
	 * power to obtain the decimal floating point number.
	 */
	*fpdoub = dtemp *dec2exp;

	if (signbit)
		*fpdoub = -(*fpdoub);
	return(FPNUM);
}

