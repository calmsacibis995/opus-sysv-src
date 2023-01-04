#ifndef __Imath
#define __Imath

#define HUGE_VAL	__HUGE_VAL

extern double __HUGE_VAL;

double	acos(double),
	asin(double),
	atan(double),
	atan2(double, double),
	cos(double),
	sin(double),
	tan(double),
	cosh(double),
	sinh(double),
	tanh(double),
	exp(double),
	frexp(double, int *),
	ldexp(double, int),
	log(double),
	log10(double),
	modf(double, double *),
	pow(double, double),
	sqrt(double),
	ceil(double),
	fabs(double),
	floor(double),
	fmod(double, double),
	erf(double),
	erfc(double),
	gamma(double),
	hypot(double, double),
	j0(double), j1(double), jn(int, double),
	y0(double), y1(double), yn(int, double);

extern int errno;
extern int signgam;

int matherr(struct exception *);

struct exception {
	int type;
	char *name;
	double arg1;
	double arg2;
	double retval;
};

#define DOMAIN		1
#define SING		2
#define OVERFLOW	3
#define UNDERFLOW	4
#define TLOSS		5
#define PLOSS		6

#define M_E		2.7182818284590452353603
#define M_LOG2E		1.4426950408889634073599
#define M_LOG10E	0.4342944819032518276511
#define M_LN2		0.6931471805599453094172
#define M_LN10		2.3025850929940456840180
#define M_PI		3.1415926535897932384626
#define M_PI_2		1.5707963267948966192313
#define M_PI_4		0.7853981633974483096157
#define M_1_PI		0.3183098861837906715377
#define M_2_PI		0.6366197723675813430755
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2		1.4142135623730950488016
#define M_SQRT1_2	0.7071067811865475244008

#endif
