extern nodev(), nulldev();
extern true(), false();
extern fsnull(), fsstray();
extern nopkg(), noreach();


extern dkopenb(), dkopenc(), dkclosec(), dkcloseb(), dkread(),
       dkwrite(), dkstrategy(), dkioctl(), dkprint();
extern conopen(), conclose(), conread(), conwrite(), conioctl();
extern struct tty con_tty[];
extern asopen(), asclose(), asread(),aswrite(),asioctl();
extern lpopen(),lpclose(),lpwrite(),lpioctl();
extern prfread(), prfwrite(), prfioctl();
extern sxtopen(), sxtclose(), sxtread(), sxtwrite(), sxtioctl();
extern dosopen(), dosclose(), dosioctl();
extern vdiopen(), vdiclose(), vdiioctl();
extern pclopen(), pclclose(), pclread(), pclwrite(), pclioctl();
extern pclcopen(), pclcclose(), pclcread(), pclcioctl();
extern gnopenc(), gnopenb(), gnclosec(), gncloseb(), gnread(),
       gnwrite(), gnioctl(), gnstrategy(), gnprint();
extern gnfopenc(), gnfopenb(), gnfclosec(), gnfcloseb(), gnfread(),
       gnfwrite(), gnfioctl(), gnfstrategy(), gnfprint();
extern ptcopen(), ptcclose(), ptcrsrv(),ptcwput();
extern ptsopen(), ptsclose(), ptsread(),ptswrite(),ptsioctl();
extern PTcopen(), PTcclose();
extern PTcread(),PTcwrite(),PTcioctl();
extern PTsopen(), PTsclose(), PTsread(),PTswrite(),PTsioctl();
extern logopen(), logclose(), logread(), logwrite(), logioctl(), logselect();

extern Xdispopen(), Xdispclose(), Xdispioctl();
extern Xsigopen(), Xsigclose(), Xsigrsrv();
extern conxopen(), conxclose(), conxrsrv();
extern mtopenb(), mtopenc(), mtclose(), mtread(),
       mtwrite(), mtioctl(), mtstrategy(), mtprint();
extern xttyopen(), xttyclose(), xttyread(), xttywrite(), xttyioctl(),
	exopen(), exclose(), exread(), exwrite(), exioctl(),
	xmread(), xmwrite(), xmioctl(),
	xsoopen(), xsoclose(), xsoread(), xsowrite(), xsoioctl();
extern cptopen(), cptclose(), cptioctl();
extern duinit(), duiput(), duiread(), duiupdat(), dureadi(),
duwritei(), duitrunc(), dustatf(), dunamei(), dumount(),
duumount(), dugetinode(), duopeni(), duclosei(), duupdate(),
dustatfs(), duaccess(), dugetdents(), duallocmap(), dufreemap(),
dureadmap(), dusetattr(), dunotify(), dufcntl(), duioctl();


extern sxtioctl(), sxtopen(), sxtclose(), sxtread(), sxtwrite();

extern struct streamtab loginfo;
extern struct streamtab spinfo;
extern struct streamtab clninfo;
extern nfsdioctl();
extern nfsdstrategy();

extern sockinit(),  sockreadi(), sockwritei(), sockstatf(), sockclosei(),
socknotify(), sockfcntl(), sockioctl();
extern mmread(), mmwrite();
extern syioctl(), syopen(), syread(), sywrite();

struct bdevsw bdevsw[] = {
/* 0*/	dkopenb,	dkcloseb,	dkstrategy,	dkprint,
/* 1*/	nodev,          nodev,  	nodev,		nodev,
/* 2*/	gnopenb,	gncloseb,	gnstrategy,	gnprint,
/* 3*/	mtopenb,	mtclose,	mtstrategy,	mtprint,
/* 4*/	nodev,		nodev,		nodev,		nodev,
/* 5*/	nodev,		nodev,		nodev,		nodev,
/* 6*/	nodev,		nodev,		nodev,		nodev,
#ifdef LAINFS
/* 7*/	nodev,		nodev,		nfsdstrategy,	nodev,
#endif
};


struct cdevsw cdevsw[] = {
/*0*/  conopen, conclose,  conread,  conwrite,  conioctl,  con_tty,  0,0,
/*1*/  nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*2*/  syopen,  nulldev,   syread,   sywrite,   syioctl,   0,        0,0,
/*3*/  nulldev, nulldev,   mmread,   mmwrite,   nodev,     0,        0,0,
/*4*/  dkopenc, dkclosec,  dkread,   dkwrite,   dkioctl,   0,        0,0,
/*5*/  nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*6*/  nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*7*/  nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*8*/  nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*9*/  asopen,  asclose,   asread,   aswrite,   asioctl,   0,	     0,0,
/*10*/ lpopen,  lpclose,   nodev,    lpwrite,   lpioctl,   0,        0,0,
/*11*/ nulldev, nulldev,   prfread,  prfwrite,  prfioctl,  0,        0,0,
/*12*/ sxtopen, sxtclose,  sxtread,  sxtwrite,  sxtioctl,  0,        0,0,
/*13*/ dosopen, dosclose,  nodev,    nodev,     dosioctl,  0,        0,0,
/*14*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*15*/ gnopenc, gnclosec,  gnread,   gnwrite,   gnioctl,   0,        0,0,
/*16*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*17*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*18*/ mtopenc, mtclose,   mtread,   mtwrite,   mtioctl,   0,        0,0,
/*19*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*20*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*21*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*22*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*23*/ cptopen, cptclose,  nulldev,  nulldev,   cptioctl,  0,        0,0,
/*24*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*25*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*26*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*27*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*28*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*29*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*30*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*31*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*32*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
#ifdef LAINFS
/*33*/ nulldev, nulldev,   nulldev,  nulldev,   nfsdioctl, 0,        0,0,
#else
/*33*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
#endif
/*34*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*35*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*36*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
#if BSD
/*37*/ PTcopen, PTcclose,  PTcread,  PTcwrite,  PTcioctl,  0,        0,0,
/*38*/ PTsopen, PTsclose,  PTsread,  PTswrite,  PTsioctl,  0,        0,0,
#else
/*37*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
/*38*/ nodev,   nodev,     nodev,    nodev,     nodev,     0,        0,0,
#endif
/*39*/ logopen, logclose,  logread,  logwrite,  logioctl,  0,       0,logselect,
};

#define SIZE(X)		sizeof(X) / sizeof(struct X)

int	bdevcnt = SIZE( bdevsw );
int	cdevcnt = SIZE( cdevsw );
int	memmajor = 3;	/* major number of mem & kmem */
