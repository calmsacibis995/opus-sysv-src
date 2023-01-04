#ifndef	__Inan
#define	__Inan

typedef union ieee_bits {
	struct	{
		unsigned sign:1;
		unsigned exp:11;
		unsigned m1:20;
		unsigned m2:32;
	} b;
	double d;
} *dptr, dnan;

#define IsNANorINF(f)	(((dptr)&(f))->b.exp == 0x7ff)

#define IsINF(f)	(((dptr)&(f))->b.m1 == 0 && ((dptr)&(f))->b.m2 == 0)

#define IsNegNAN(f)	(((dptr)&(f))->b.sign)

#define IsPosNAN(f)	(!IsNegNAN(f))

#define GETNaNPC(f)	(((dptr)&(f))->b.m1 << 12 | ((dptr)&(f))->b.m2 >> 20) 

#define KILLFPE()	(void) raise(8)

#define NaN(f)		IsNANorINF(f)

#define KILLNaN(X)      if (NaN(X)) KILLFPE()

#endif

