#ifndef __Iieeefp
#define __Iieeefp

typedef	enum fp_rnd {
	FP_RN = 0x0000,
	FP_RZ = 0x4000,
	FP_RP = 0xc000,
	FP_RM = 0x8000,
} fp_rnd;

#define	fp_except	int

#define	FP_DISABLE	0
#define	FP_ENABLE	1
#define	FP_CLEAR	0
#define	FP_SET		1

#define	FP_X_INV	0x10
#define	FP_X_OFL	0x02
#define	FP_X_UFL	0x04
#define	FP_X_DZ		0x08
#define	FP_X_IMP	0x01

#define isnanf(x)	(((*(int *)&(x) & 0x7f800000L)==0x7f800000L)&& \
			 ((*(int *)&(x) & 0x007fffffL)!=0x00000000L) )

#endif
