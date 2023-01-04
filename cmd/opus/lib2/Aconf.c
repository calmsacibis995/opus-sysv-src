static char	sccsid[] = "@(#)Aconf.c	1.5";

#include "compat.h"
#if pm100
#include "low.pm100.c"
#else
#include "low.c"
#endif

int	_nullsys();
#if pm100 || pm200 || pm300 || pm400
int	_dkstrategy(), _dkopen(), _dkclose();
int	_fdstrategy(), _fdclose();
#else
int	_gdstrategy();
int	_hmstrategy();
#ifdef vax
int	_gtstrategy(), _gtclose();
int	_tsstrategy(), _tsclose();
#else
int	_htstrategy(), _htclose();
#endif
#endif

struct devsw _devsw[] = {
#if pm100 || pm200 || pm300 || pm400
/* 0: dk */	_dkstrategy,	_dkopen,	_dkclose,
/* 1: mt */	_fdstrategy,	_nullsys,	_fdclose,
#else
/* 0: gd */	_gdstrategy,	_nullsys,	_nullsys,
#ifdef vax
/* 1: gt */	_gtstrategy,	_nullsys,	_gtclose,
/* 2: ts */	_tsstrategy,	_nullsys,	_tsclose,
#else
/* 1: ht */	_htstrategy,	_nullsys,	_htclose,
#endif
#endif
};

struct iob _iobuf[NFILES];
struct dtab _dtab[NDEV];
struct mtab _mtab[NMOUNT];
int errno;

#if vax
int	_devcnt = 3;
#else
int	_devcnt = 2;
#endif
