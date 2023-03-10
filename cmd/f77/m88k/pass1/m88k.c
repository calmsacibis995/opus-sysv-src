/*	@(#)m88k.c	1.8	*/
#include "defs"

#ifdef SDB
#	include <a.out.h>
extern int types2[];
#endif

#include "pccdefs"

/*
	M88K - SPECIFIC ROUTINES
*/

extern int f_lineno;
extern int fnstline;
extern int FLAG_BF;

int maxregvar = MAXREGVAR;
int regnum[] =  { 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14 } ;
int maxargoffset;
static unsigned ldstoff;


ftnint intcon[14] =
	{ 2, 2, 2, 2,
	  15, 31, 24, 56,
	  -128, -128, 127, 127,
	  32767, 2147483647 };

#if HERE == VAX
	/* then put in constants in octal */
long realcon[6][2] =
	{
		{ 0200, 0 },
		{ 0200, 0 },
		{ 037777677777, 0 },
		{ 037777677777, 037777777777 },
		{ 032200, 0 },
		{ 022200, 0 }
	};
#else
double realcon[6] =
	{
	2.9387358771e-39,
	2.938735877055718800e-39,
	1.7014117332e+38,
	1.701411834604692250e+38,
	5.960464e-8,
	1.38777878078144567e-17,
	};
#endif




prsave()
{
int i;
if(profileflag)
	{
		i = newlabel();
		p2pass ("	subu	r31,r31,8");
		p2pass ("	st	r1,r31,4");
		p2pass ("	st	r2,r31,0");
		p2pi   ("	or.u	r2,r0,hi16(@L%d)", i);
		p2pass ("	bsr.n	mcount");
		p2pi   ("	or	r2,r2,lo16(@L%d)", i);
		p2pass ("	ld	r2,r31,0");
		p2pass ("	ld	r1,r31,4");
		p2pass ("	addu	r31,r31,8");
		/*fputs("\talign\t4\n", asmfile);*/
		fprintf(asmfile, "@L%d:	word	0\n", i);
	}
}



goret(type, argvec)
int type;
Addrp  argvec;
{
register int pop, i, max, min;

char lbuf[256];
min = regnum[highregvar] + 1;
max = regnum[0] + 1;
pop = (autoleng * sizeof(char) + 7)& ~7;
ldstrst();
for (i = min; i < max ; i++, pop += 4) {
	ldst("ld", i, 31, pop);
}
ldst ("ld", 30, 31, pop);
pop += 4;
ldst ("ld", 1, 31, pop);
if (pop > 0xffff) {
p2pi  (		"	or.u	r13,r0,hi16(FRAMESZ%d)", procno);
p2pi  (		"	or	r13,r13,lo16(FRAMESZ%d)", procno);
p2pass(		"	jmp.n	r1");
p2pass(		"	addu	r31,r31,r13");
}
else {
p2pass(		"	jmp.n	r1");
p2pi(		"	addu	r31,r31,FRAMESZ%d", procno);
}
if (procclass == CLPROC && proctype != TYSUBR)
	{
	if (proctype == TYDREAL)
		;
	else if (proctype != TYERROR && proctype != TYUNKNOWN)
		;
	else
		badthing("procedure type", proctype, "goret");
	}
if (((cdbflag || sdbflag) || optimflag) && (FLAG_BF > 0))
{
	FLAG_BF = 0;
	p2pi("\ttext\n\tsdef\t.ef\n\tval\t.\n\tscl\t101\n\tline\t%d\n\tendef",
	f_lineno - fnstline);
}
}

saveregs(argvec, first)
Addrp  argvec;
{
register int pop, i, max, min;

char lbuf[256];
min = regnum[highregvar] + 1;
max = regnum[0] + 1;
pop = (autoleng * sizeof(char) + 7)& ~7;

if (pop > 0xffff) {
p2pi  (		"	or.u	r12,r0,hi16(FRAMESZ%d)", procno);
p2pi  (		"	or	r12,r12,lo16(FRAMESZ%d)", procno);
p2pass(		"	subu	r31,r31,r12");
}
else {
p2pi(		"	subu	r31,r31,FRAMESZ%d", procno);
}
ldstrst();
for (i = min; i < max ; i++, pop += 4) {
	ldst("st", i, 31, pop);
}
ldst("st", 30, 31, pop);
pop += 4;
ldst("st", 1, 31, pop);
if ( pop > 0xffff) 
p2pass(		"	addu	r30,r31,r12");
else
p2pi(		"	addu	r30,r31,FRAMESZ%d", procno);
pop += 4;
pop = (pop * sizeof(char) + 7)& ~7;

if (lastargslot > maxargoffset)
	max = lastargslot;
else
	max = maxargoffset;
max = (max / SZADDR) + 1;
if (max > 9) max = 9;

for (i = 2 ; i <= max ; i += 2) {
	if (i < max)
		ldst ("st.d", i, 30, (i-2)*4);
	else
		ldst ("st", i, 30, (i-2)*4);
}
if (!first)
	return;
fprintf(asmfile,"\tdef\tESP%d,%d\n",procno,pop);
}


/*
 * move argument slot arg1 (relative to fp)
 * to slot arg2 (relative to FRAME POINTER)
 */

mvarg(type, arg1, arg2)
int type, arg1, arg2;
{
/*
p2pi("\tld\tr13,r30,%d", arg1+ARGOFFSET);
p2pi("\tst\tr13,r31,%d", arg2+argloc);
*/
ldstrst();
ldst("ld", 12, 30, arg1+ARGOFFSET);
ldst("st", 12, 31, arg2+argloc);
}

static
ldstrst()
{
	ldstoff = 0;
}

static
ldst(str, reg, base, offset)
char *str;
unsigned offset;
{
	unsigned char buf[256];
	
	if (offset > 0xffff) {
		if ((offset - ldstoff) > 0xffff) {
			sprintf( buf,	"	or.u	r13,r0,hi16(%d)", 
				offset);
			p2pass(buf);
			sprintf( buf,	"	or	r13,r13,lo16(%d)", 
				offset);
			p2pass(buf);
			sprintf( buf,	"	addu	r13,r%d,r13", base);
			p2pass(buf);
			ldstoff = offset;
			offset = 0;
		}
		else {
			offset -= ldstoff;
		}
		sprintf( buf,	"	%s	r%d,r13,%d", 
			str, reg, offset);
	}
	else {
		sprintf (buf,	"	%s	r%d,r%d,%d", 
			str, reg, base, offset);
	}
	p2pass(buf);
}



prlabel(fp, k)
FILEP fp;
int k;
{
fprintf(fp, "@L%d:\n", k);
}



prconi(fp, type, n)
FILEP fp;
int type;
ftnint n;
{
fprintf(fp, "\t%s\t%ld\n", (type==TYSHORT ? "half" : "word"), n);
}


prnicon(fp, type, num, n)
register FILEP fp;
int type;
register int n;
ftnint num;
{
char buf[20];

sprintf(buf, "%ld", num);
fprintf(fp, "\t%s\t%s", (type==TYSHORT ? "half" : "word"), buf);
while (--n)
	fprintf(fp, ",%s", buf);
putc('\n', fp);
}



prcona(fp, a)
FILEP fp;
ftnint a;
{
fprintf(fp, "\tword\t@L%ld\n", a);
}



prnacon(fp, a, n)
register FILEP fp;
ftnint a;
register int n;
{
char buf[20];

sprintf(buf, "@L%ld", a);
fprintf(fp, "\tword\t%s", buf);
while (--n)
	fprintf(fp, ",%s", buf);
putc('\n', fp);
}

#ifndef m88k
prconr(fp, type, x)
FILEP fp;
int type;
double x;
{
fprintf(fp, "\t%s\t0f%.20e\n", (type==TYREAL ? ".float" : ".long"), x);
}

prnrcon(fp, type, x, n)
register FILEP fp;
int type;
register int n;
double x;
{
char buf[40];

sprintf(buf, "0f%.20e", x);
fprintf(fp, "\t%s\t%s", (type==TYREAL ? ".float" : ".long"), buf);
while (--n)
	fprintf(fp, ",%s", buf);
putc('\n', fp);
}

prncxcon(fp, type, real, imag, n)
register FILEP fp;
int type;
register int n;
double real, imag;
{
char buf[100];

sprintf(buf, "0f%.20e,0f%.20e", real, imag);
fprintf(fp, "\t%s\t%s", (type == TYREAL ? ".float" : ".long"), buf);
while (--n)
	fprintf(fp, ",%s", buf);
putc('\n', fp);
}
#endif



#ifdef m88k
prconr(fp, type, x)
FILEP fp;
int type;
double x;
{
/* non-portable cheat to preserve bit patterns */
union { double xd; float xf[2]; long int xl[2]; } cheat;
if(type == TYREAL){
	cheat.xf[0] = x;
	fprintf(fp, "\tword\t0x%lx\n", cheat.xl[0]);
	}
else {
	cheat.xd = x;
	fprintf(fp, "\tword\t0x%lx,0x%lx\n", cheat.xl[0], cheat.xl[1]);
	}
}

prnrcon(fp, type, x, n)
register FILEP fp;
int type;
double x;
register int n;
{
char buf[50];
union {double xd; float xf[2]; long xl[2]; } cheat;

if (type == TYREAL) {
	cheat.xf[0] = x;
	sprintf(buf, "0x%lx", cheat.xl[0]);
	}
else {
	cheat.xd = x;
	sprintf(buf, "0x%lx,0x%lx", cheat.xl[0], cheat.xl[1]);
	}
fprintf(fp, "\tword\t%s", buf);
while (--n)
	fprintf(fp, ",%s", buf);
putc('\n', fp);
}

prncxcon(fp, type, real, imag, n)
register FILEP fp;
int type;
register int n;
double real, imag;
{
char buf[60];
union {double xd; float xf[2]; long xl[2];} cheat1, cheat2;

if (type == TYREAL) {
	cheat1.xf[0] = real;
	cheat2.xf[0] = imag;
	sprintf(buf, "0x%lx,0x%lx", cheat1.xl[0], cheat2.xl[0]);
	}
else {
	cheat1.xd = real;
	cheat2.xd = imag;
	sprintf(buf, "0x%lx,0x%lx,0x%lx,0x%lx", cheat1.xl[0], cheat1.xl[1],
	  cheat2.xl[0], cheat2.xl[1]);
	}
fprintf(fp, "\tword\t%s", buf);
while (--n)
	fprintf(fp, ",%s", buf);
putc('\n', fp);
}
#endif



praddr(fp, stg, varno, offset)
FILE *fp;
int stg, varno;
ftnint offset;
{
char *memname();

if(stg == STGNULL)
	fprintf(fp, "\tword\t0\n");
else
	{
	fprintf(fp, "\tword\t%s", memname(stg,varno));
	if(offset)
		fprintf(fp, "+%ld", offset);
	fprintf(fp, "\n");
	}
}




preven(k)
int k;
{
fprintf(asmfile, "\talign\t%d\n", k);
}


vaxgoto(index, nlab, labs)
expptr index;
register int nlab;
struct Labelblock *labs[];
{
register int i;
register int arrlab;
int skiplab;

putforce(TYINT, index);
p2pass(	"	subu	r2,r2,1");
p2pi(	"	cmp	r13,r2,%d", nlab - 1);
p2pi(	"	bb1	hi,r13,@L%d", skiplab = newlabel() );
p2pi(	"	or.u	r3,r0,hi16(@L%d)", arrlab = newlabel());
p2pi(	"	or	r3,r3,lo16(@L%d)", arrlab );
p2pass(	"	lda	r2,r3[r2]");
p2pass(	"	jmp	r2" );
p2pi("@L%d:", arrlab);
for(i = 0; i< nlab ; ++i)
	if( labs[i] )
		p2pij("\tbr\t@L%d", labs[i]->labelno);
p2pi("@L%d:", skiplab);
}


prarif(p, neg, zer, pos)
expptr p;
int neg, zer, pos;
{
int t;

putforce(t = p->headblock.vtype, p);
if (ISINT(t)) {
	p2pi("	bcnd	eq0,r2,@L%d", zer);
	p2pi("	bb1	31,r2,@L%d", neg);
}
else {
	if ( t == TYREAL )
		p2pass("fcmp.sss	r13,r2,r0");
	else
		p2pass("fcmp.sds	r13,r2,r0");
	p2pi("\tbb1	lt,r13,@L%d", neg);
	p2pi("\tbb1	eq,r13,@L%d", zer);
}
p2pi("\tbr	@L%d", pos);
}




char *memname(stg, mem)
int stg, mem;
{
static char s[100];

switch(stg)
	{
	case STGCOMMON:
	case STGEXT:
		sprintf(s, "_%s", varstr(XL, extsymtab[mem].extname) );
		break;

	case STGBSS:
	case STGINIT:
		sprintf(s, "v.%d", mem);
		break;

	case STGCONST:
		sprintf(s, "@L%d", mem);
		break;

	case STGEQUIV:
		sprintf(s, "q.%d", mem+eqvstart);
		break;

	default:
		badstg("memname", stg);
	}
return(s);
}




prlocvar(s, len, align)
char *s;
ftnint len;
int align;
{
fprintf(asmfile, "\tbss\t%s,%ld,%d\n", s, len, align);
}



prext(name, leng, init)
char *name;
ftnint leng;
int init;
{
if(leng == 0)
	fprintf(asmfile, "\tglobal\t_%s\n", name);
else
	fprintf(asmfile, "\tcomm\t_%s,%ld\n", name, leng);
}





prendproc()
{
}




prtail()
{
}


putjump(label) 
{
	p2pi("\tbr\t@L%d", label);
}


prolog(ep, argvec)
struct Entrypoint *ep;
Addrp  argvec;
{
int i, argslot;
int size;
register chainp p;
register Namep q;
register struct Dimblock *dp;
expptr tp, mkaddcon();

if(procclass == CLBLOCK)
	return;
/*p2pi("@L%d:", ep->backlabel);	/* Already done */
if(argvec)
	{
	argloc = argvec->memoffset->constblock.Const.ci;
	if(proctype == TYCHAR)
		{
		mvarg(TYADDR, 0, chslot);
		mvarg(TYLENG, SZADDR, chlgslot);
		argslot = SZADDR + SZLENG;
		}
	else if( ISCOMPLEX(proctype) )
		{
		mvarg(TYADDR, 0, cxslot);
		argslot = SZADDR;
		}
	else
		argslot = 0;

	for(p = ep->arglist ; p ; p =p->nextp)
		{
		q = (Namep) (p->datap);
		mvarg(TYADDR, argslot, q->vardesc.varno);
		argslot += SZADDR;
		}
	for(p = ep->arglist ; p ; p = p->nextp)
		{
		q = (Namep) (p->datap);
		if(q->vtype==TYCHAR && q->vclass!=CLPROC)
			{
			if(q->vleng && ! ISCONST(q->vleng) )
				mvarg(TYLENG, argslot,
					q->vleng->addrblock.memno);
			argslot += SZLENG;
			}
		}

/* When generating code for procedures with more than one entry point,
 * the arguments are restacked. Two additional lines of code are needed.
 * line 1: Set ap past argument list + 1 slot over.
 * line 2: Place # of arguments in extra slot pointed to by ap.
 *      low address                      high address
 *      ---------------------------------------------
 *      |        | # args |  arg   |  arg   |       |
 *      ---------------------------------------------
 *                   ap                         fp
 */
/*	p2pi("\taddl3\t$%d,fp,ap", argloc-ARGOFFSET);
	p2pi("\tmovl\t$%d,(ap)\n", lastargslot/SZADDR); */
	/*p2pi("\taddu\tr30,r31,%d", argloc);*/
	ldstrst();
	ldst("addu", 30, 31, argloc);
	}

for(p = ep->arglist ; p ; p = p->nextp)
	{
	q = (Namep) (p->datap);
	if(dp = q->vdim)
		{
		for(i = 0 ; i < dp->ndim ; ++i)
			if(dp->dims[i].dimexpr)
				puteq( fixtype(cpexpr(dp->dims[i].dimsize)),
					fixtype(cpexpr(dp->dims[i].dimexpr)));
		size = typesize[ q->vtype ];
		if(q->vtype == TYCHAR)
			if( ISICON(q->vleng) )
				size *= q->vleng->constblock.Const.ci;
			else
				size = -1;

		/* on VAX, get more efficient subscripting if subscripts
		   have zero-base, so fudge the argument pointers for arrays.
		   Not done if array bounds are being checked.
		*/
		if(dp->basexpr)
			puteq( 	cpexpr(fixtype(dp->baseoffset)),
				cpexpr(fixtype(dp->basexpr)));

		if(! checksubs)
			{
			if(dp->basexpr)
				{
				if(size > 0)
					tp = (expptr) ICON(size);
				else
					tp = (expptr) cpexpr(q->vleng);
				putforce(TYINT,
					fixtype( mkexpr(OPSTAR, tp,
						cpexpr(dp->baseoffset)) ));
				p2pij("\tld\tr13,r30,%d\n\tsubu\tr13,r13,r2\n\tst\tr13,r30,%d",
					p->datap->nameblock.vardesc.varno +
						ARGOFFSET,
					p->datap->nameblock.vardesc.varno +
						ARGOFFSET);
				}
			else if(dp->baseoffset->constblock.Const.ci != 0)
				{
				char buff[200];
				if(size > 0)
					{
					sprintf(buff, "\tld\tr13,r30,%d\n\tsubu\tr13,r13,%ld\n\tst\tr13,r30,%d",
						p->datap->nameblock.vardesc.varno +
							ARGOFFSET,
						dp->baseoffset->constblock.Const.ci * size,
						p->datap->nameblock.vardesc.varno +
							ARGOFFSET);
					}
				else	{
					putforce(TYINT, mkexpr(OPSTAR, cpexpr(dp->baseoffset),
						cpexpr(q->vleng) ));
						sprintf(buff, "\tld\tr13,r30,%d\n\tsubu\tr13,r13,r2\n\tst\tr13,r30,%d",
						p->datap->nameblock.vardesc.varno +
							ARGOFFSET,
						p->datap->nameblock.vardesc.varno +
							ARGOFFSET);
					}
				p2pass(buff);
				}
			}
		}
	}

if(typeaddr)
	puteq( cpexpr(typeaddr), mkaddcon(ep->typelabel) );
/* replace to avoid long jump problem
putgoto(ep->entrylabel);
*/
}




prhead(fp)
FILEP fp;
{
#if (!defined(FONEPASS)) && (FAMILY == PCC)
	p2triple(P2LBRACKET, ARGREG-highregvar, procno);
	p2word(0L);
	p2flush();
#endif
}



prdbginfo()
{
}

#ifdef SDB
prstab(name, val, stg, type)
char *name, *val;
int stg, type;
{
	char buf[BUFSIZ];

	if ((type & 0x0f) == T_ENUM)	/* got to be complex */
		sprintf(buf, "\tsdef\t%s\n\tval\t%s\n\tscl\t%d\n\ttype\t010\n\ttag\tComplex\n\tsize\t8\n\tendef",
		    name, val, stg);
	else if ((type & 0x0f) == T_UCHAR)	/* got to be double complex */
		sprintf(buf, "\tsdef\t%s\n\tval\t%s\n\tscl\t%d\n\ttype\t010\n\ttag\tDcomplex\n\tsize\t16\n\tendef",
		    name, val, stg);
	else
	if (cdbflag) {
	   if (stg == C_EXT && type == (T_INT|(DT_FCN << 4))) {
	   sprintf(buf,"\tsdef\t_%s\n\tval\t%s\n\tscl\t%d\n\ttype\t0x%x\n\tendef"
		, name, val, stg, type);
	   } else {
	   sprintf(buf,"\tsdef\t%s\n\tval\t%s\n\tscl\t%d\n\ttype\t0x%x\n\tendef"
		, name, val, stg, type);
	   }
	} else {
	   sprintf(buf,"\tsdef\t%s\n\tval\t%s\n\tscl\t%d\n\ttype\t0x%x\n\tendef"
		, name, val, stg, type);
	}
	p2pass(buf);
}


prarstab(name, val, stg, type, np)
char *name, *val;
int stg, type;
register Namep np;
{
	register int i, ts;
	char buf[BUFSIZ];

	if (cdbflag) {
		sprintf(buf, "\tsdef\t%s\n\tval\t%s\n\tscl\t%d\n\ttype\t",
	    	name, val, stg);
	    	ts = typesize[(int) np->vtype];
	} else {
		sprintf(buf, "\tsdef\t%s\n\tval\t%s-0x%x\n\tscl\t%d\n\ttype\t",
	    	name, val, np->vdim->baseoffset->constblock.Const.ci *
	    	(ts = typesize[(int) np->vtype]), stg);
	}
	if ((i = type & 0x0f) == T_ENUM )
		strcat(buf, "010\n\ttag\tComplex;");
	else if (i == T_UCHAR)
		strcat(buf, "010\n\ttag\tDcomplex;");
	else
		sprintf(buf + strlen(buf), "0x%x;", type);
	sprintf(buf + strlen(buf), "\tsize\t0x%x\n\tdim\t",
	    np->vdim->nelt->constblock.Const.ci * ts);
	for (i = 0; i != np->vdim->ndim; i++)
		sprintf(buf + strlen(buf), "%d,",
		    np->vdim->dims[i].dimsize->constblock.Const.ci);
	sprintf(buf + strlen(buf) - 1, "\n\tendef");
	p2pass(buf);
}


prststab(name, val, stg, type, tag, size)
char *name, *val, *tag;
int stg, type, size;
{
	char buf[BUFSIZ];

	sprintf(buf, "\tsdef\t%s\n\tval\t%s\n\tscl\t%d\n\ttype\t%d\n\ttag\t%s\n\tsize\t%d\n\tendef",
	    name, val, stg, type, tag, size);
	p2pass(buf);
}



stabtype(p)
register Namep p;
{
int d[7];		/* Type Word |d6|d5|d4|d3|d2|d1|typ|		 */
register int *pd;	/* Pointer to d. d fields = 2 bits, typ = 4 bits */
register int type;
register int i;

d[1] = d[2] = d[3] = d[4] = d[5] = d[6] = 0;
pd = &d[1];

if (p->vtype == TYCOMPLEX)
	d[0] = T_ENUM;
else if (p->vtype == TYDCOMPLEX)
	d[0] = T_UCHAR;
else
	d[0] = types2[p->vtype];

/* For each dimension of an array, fill a d slot with 3 (array).
 * If array is argument, fill 1 less slot. Later on ptr value will be added.
 */
if (p->vdim) {
	i = (p->vstg == STGARG) ? 2 : 1;
	for (; i <= p->vdim->ndim; i++, pd++)
		*pd = 3;	/* 3 = array	*/
	}

if(p->vstg == STGARG)
	*pd++ = 1;		/* 1 = pointer	*/
if (p->vclass == CLPROC)
	*pd++ = 2;		/* 2 = function	*/

for (type = 0, pd--; pd > &d[0]; pd--)
	type = (type << 2) | *pd;
type = (type << 4) | d[0];
return(type);
}
#endif
