/*	@(#)init.c	1.8	*/

#include "defs"

FILEP infile	= { stdin };
FILEP diagfile	= { stderr };

FILEP textfile;
FILEP asmfile;
FILEP initfile;
long int headoffset;
#if TARGET == OPUS
FILEP xreffile;
flag xrefflag = NO;
#endif

char token[200];
int toklen;
int f_lineno;
char *infname = "command line";
int needkwd;
struct Labelblock *thislabel	= NULL;
struct Labelblock *fmtlabel	= NULL;	/* Set when encounter format stmt. */
flag nowarnflag	= NO;
flag ftn66flag	= NO;
flag no66flag	= NO;
flag noextflag	= NO;
flag profileflag	= NO;
flag optimflag	= NO;
flag shiftcase	= YES;
flag undeftype	= NO;
flag shortsubs	= YES;
flag onetripflag	= NO;
flag checksubs	= NO;
flag debugflag	= NO;
flag exprflag	= NO;
int nerr;
int nwarn;
int ndata;
#if FLEXNAMES && ( TARGET == OPUS )
/* allow name longer name lengths and imbedded underscores */
flag longnames	= YES;
#endif

flag saveall;
flag substars;
int parstate	= OUTSIDE;
flag headerdone	= NO;
int blklevel;
int impltype[26];
int implleng[26];
int implstg[26];

int tyint	= TYLONG ;
int tylogical	= TYLONG;
ftnint typesize[NTYPES]
	= { 1, SZADDR, SZSHORT, SZLONG, SZLONG, 2*SZLONG,
	    2*SZLONG, 4*SZLONG, SZLONG, 1, 1, 1};
int typealign[NTYPES]
	= { 1, ALIADDR, ALISHORT, ALILONG, ALILONG, ALIDOUBLE,
	    ALILONG, ALIDOUBLE, ALILONG, 1, 1, 1};
int procno;
int proctype	= TYUNKNOWN;
char *procname;
int rtvlabel[NTYPES];
int fudgelabel;
Addrp typeaddr;
Addrp retslot;
int cxslot	= -1;
int chslot	= -1;
int chlgslot	= -1;
int procclass	= CLUNKNOWN;
int nentry;
flag multitype;
ftnint procleng;
int lastlabno	= 10;
int lastvarno;
int lastargslot;
int argloc;
ftnint autoleng;
ftnint bssleng	= 0;
int retlabel;
int ret0label;

int maxctl	= MAXCTL;
struct Ctlframe *ctls;
struct Ctlframe *ctlstack;
struct Ctlframe *lastctl;

Namep regnamep[MAXREGVAR];
int highregvar;
int nregvar;

int maxext	= MAXEXT;
struct Extsym *extsymtab;
struct Extsym *nextext;
struct Extsym *lastext;

int maxequiv	= MAXEQUIV;
struct Equivblock *eqvclass;

int maxhash	= MAXHASH;
struct Hashentry *hashtab;
struct Hashentry *lasthash;

int maxstno	= MAXSTNO;
struct Labelblock *labeltab;
struct Labelblock *labtabend;
struct Labelblock *highlabtab;

int maxlab	= MAXLABLIST;
Labelp *labarray;	/* points to array of labels for computed goto */


int maxdim	= MAXDIM;
struct Rplblock *rpllist	= NULL;
struct Chain *curdtp	= NULL;
flag toomanyinit;
ftnint curdtelt;
chainp templist	= NULL;
chainp holdtemps	= NULL;
int dorange	= 0;
struct Entrypoint *entries	= NULL;

chainp chains	= NULL;

flag inioctl;
Addrp ioblkp;
int iostmt;
int nioctl;
int nequiv	= 0;
int eqvstart	= 0;
int nintnames	= 0;

#ifdef SDB
int dbglabel	= 0;
flag sdbflag	= NO;
flag cdbflag	= NO;
int fnstline	= 0;	/* starting line number of current function */
#endif

struct Literal litpool[MAXLITERALS];
int nliterals;




fileinit(dfl)
int dfl;
{
procno = 0;
lastlabno = 10;
lastvarno = 0;
nliterals = 0;
nerr = 0;
ndata = 0;

ctls = ALLOCN(maxctl, Ctlframe);
extsymtab = ALLOCN(maxext, Extsym);
eqvclass = ALLOCN(maxequiv, Equivblock);
hashtab = ALLOCN(maxhash, Hashentry);
labeltab = ALLOCN(maxstno, Labelblock);
labarray = ALLOCN(maxlab, Labelblock *);

ctlstack = ctls - 1;
lastctl = ctls + maxctl;
nextext = extsymtab;
lastext = extsymtab + maxext;
lasthash = hashtab + maxhash;
labtabend = labeltab + maxstno;
highlabtab = labeltab;

#ifdef FONEPASS			/* "2nd pass" initialization stuff */
offsz = caloff();
tinit();
allo0();
mkdope();
setrew();
setregs();

if (dfl)			/* debugging flags for "2nd pass" */
	{
	xdebug = 1;
	edebug = 1;
	rdebug = 1;
	odebug = 1;
	}

#endif /*FONEPASS*/
}





procinit()
{
register Namep p;
register struct Dimblock *q;
register struct Hashentry *hp;
register struct Labelblock *lp;
struct Chain *cp;
int i;

pruse(asmfile, USECONST);
#if FAMILY == PCC
	p2pass(USETEXT);
#endif
parstate = OUTSIDE;
headerdone = NO;
blklevel = 1;
saveall = NO;
substars = NO;
nwarn = 0;
thislabel = NULL;
needkwd = 0;

++procno;
proctype = TYUNKNOWN;
procname = "MAIN_    ";
procclass = CLUNKNOWN;
nentry = 0;
multitype = NO;
typeaddr = NULL;
retslot = NULL;
cxslot = -1;
chslot = -1;
chlgslot = -1;
procleng = 0;
blklevel = 1;
lastargslot = 0;
#if TARGET==PDP11
	autoleng = 6;
#else
#if TARGET == OPUS && m88k
	autoleng = 32;
	maxargoffset = 0;
#else
	autoleng = 0;
#endif
#endif

for(lp = labeltab ; lp < labtabend ; ++lp)
	lp->stateno = 0;

for(hp = hashtab ; hp < lasthash ; ++hp)
	if(p = hp->varp)
		{
		frexpr(p->vleng);
		if(q = p->vdim)
			{
			for(i = 0 ; i < q->ndim ; ++i)
				{
				frexpr(q->dims[i].dimsize);
				frexpr(q->dims[i].dimexpr);
				}
			frexpr(q->nelt);
			frexpr(q->baseoffset);
			frexpr(q->basexpr);
			ckfree( (ptr) q);
			}
		if(p->vclass == CLNAMELIST)
			frchain( &(p->varxptr.namelist) );
		ckfree( (ptr) p);
		hp->varp = NULL;
		}
nintnames = 0;
highlabtab = labeltab;

ctlstack = ctls - 1;
for(cp = templist ; cp ; cp = cp->nextp)
	ckfree( (ptr) (cp->datap) );
frchain(&templist);
holdtemps = NULL;
dorange = 0;
nregvar = 0;
highregvar = 0;
entries = NULL;
rpllist = NULL;
inioctl = NO;
ioblkp = NULL;
eqvstart += nequiv;
nequiv = 0;

for(i = 0 ; i<NTYPES ; ++i)
	rtvlabel[i] = 0;
fudgelabel = 0;

if(undeftype)
	setimpl(TYUNKNOWN, (ftnint) 0, 'a', 'z');
else
	{
	setimpl(TYREAL, (ftnint) 0, 'a', 'z');
	setimpl(tyint,  (ftnint) 0, 'i', 'n');
	}
setimpl(-STGBSS, (ftnint) 0, 'a', 'z');	/* set class */
setlog();
}




setimpl(type, length, c1, c2)
int type;
ftnint length;
int c1, c2;
{
int i;
char buff[100];

if(c1==0 || c2==0)
	return;

if(c1 > c2)
	{
	sprintf(buff, "characters out of order in implicit:%c-%c", c1, c2);
	err(buff);
	}
else
	if(type < 0)
		for(i = c1 ; i<=c2 ; ++i)
			implstg[i-'a'] = - type;
	else
		{
		type = lengtype(type, (int) length);
		if(type != TYCHAR)
			length = 0;
		for(i = c1 ; i<=c2 ; ++i)
			{
			impltype[i-'a'] = type;
			implleng[i-'a'] = length;
			}
		}
}

#if clipper
vaxalign()
{
	/* change double alignment to vax/ns32000 double alignment */
	register i;
	for ( i = 0; i < NTYPES; i++ )
		if (typealign[i] ==  ALIDOUBLE)
			typealign[i] = ALILONG;
}
#endif
