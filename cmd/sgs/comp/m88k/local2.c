/*	@(#)local2.c	1.5	*/
# include <stdio.h>
# include "mfile2"
# include "ctype.h"
/* a lot of the machine dependent parts of the second pass */

# define BITMASK(n) ((1L<<n)-1)
#ifndef R13
#define R13	13
#endif

extern int calltmp;
extern int pushcnt;
extern int leaffunc;
extern int gdebug;
int usedregs;
int varargflag;
int safedbl;
int targetreg = '1';
int instructarg;
short usebcnd, bcndreg;
int cpuflags;

where(c){
	fprintf( stderr, "%s, line %d: ", filename, lineno );
	}

lineid( l, fn ) char *fn; {
	/* identify line l and file fn */
	printf( "#	line %d, file %s\n", l, fn );
	}

#ifdef FORT
static int lastusedregs, lastftnno, lastmaxtreg;
extern maxtreg, reg_use;
#endif

eobl2(){
	OFFSZ spoff;	/* offset from stack pointer */
	register int misc;
#ifdef FORT
	spoff = maxoff;
	/*if( spoff >= AUTOINIT ) spoff -= AUTOINIT;*/
	spoff /= SZCHAR;
	SETOFF(spoff,8);
	if (calltmp)			/* space for last calltmp */
		calltmp += 8;
	SETOFF(calltmp,8);
	misc = spoff + calltmp;
	if ((misc + 256) >= 0x10000) {
		fprintf(stderr, "size = %d\n", misc + 256);
		cerror("stackframe size");
	}
	/* next 8 lines for fortran entry statements */
	if ( ftnno == lastftnno ) {
		usedregs = lastusedregs;
		maxtreg = lastmaxtreg;
	}
	else {
		printf("\tdef\tCTMP%d,ESP%d+%d\n", ftnno, ftnno, spoff);
		printf("\tdef\tFRAMESZ%d,CTMP%d+%d\n", ftnno, ftnno, calltmp);
	}

	lastftnno = ftnno;
	lastusedregs = usedregs;
	lastmaxtreg = maxtreg;

#else
	extern int ftlab1, ftlab2, regsvsz;

	spoff = maxoff;
	/*if( spoff >= AUTOINIT ) spoff -= AUTOINIT;*/
	spoff /= SZCHAR;
	SETOFF(spoff,8);
	if (calltmp)			/* space for last calltmp */
		calltmp += 8;
	SETOFF(calltmp,8);
	misc = spoff + calltmp;
		
	if (gdebug)
		usedregs |= ( 1 << R30 );	/* hammer arg for stack trace */
	restregs(misc);
	funchead();
	saveregs(misc);
	if ( calltmp ) {
		printf("\tdef\tCTMP%d,%d\n", ftnno, spoff);
	}
	functail();
#endif
	if ( pushcnt )
		cerror ("push count not zero\n");
	maxargs = -1;
	usedregs = 0;
	leaffunc = 1;
	calltmp = 0;
	}

int lasthopcode;
char *lasthopstr;

struct hoptab { int opmask; char * opstring; char *altop } ioptab[] = {

	{ ASG PLUS, "addu", "Dadd" },
	{ ASG MINUS, "subu", "Dsub", },
	{ ASG MUL, "mul", },
	{ ASG DIV, "div", },
	{ ASG OR, "or", },
	{ ASG ER,	"xor", },
	{ ASG AND, "and", "Wmask", },
	{ PLUS,	"addu", "Dadd" },
	{ MINUS,	"subu", "Dsub", },
	{ MUL,	"mul", },
	{ DIV,	"div", },
	{ OR,	"or", },
	{ ER,	"xor", },
	{ AND,	"and", "Wmask", },
	{-1, "", }    };

hopcode( f, o )
register f, o; 
{
	/* output the appropriate string from the above table */

	register struct hoptab *q;
	register *c;

	for( q = ioptab;  q->opmask>=0; ++q ){
		if( q->opmask == o ){
			if ( (f == 'W' || f == 'D') && q->altop &&
					q->altop[0] == f)
				lasthopstr = & (q->altop[1]);
			else
				lasthopstr = q->opstring;
			lasthopcode = o;
			fputs( lasthopstr, stdout );
			return;
			}
		}
	cerror( "no hoptab for %s", opst[o] );
	}

char *
rnames[] = {  /* keyed to register number tokens */

	/* temps */
	"r0", "r1", "r2", "r3", "r4",
	"r5", "r6", "r7", "r8", "r9", 
	"r10", "r11", "r12", 
	/* very temp reg */
	"r13",
	/* register vars */
	"r14", "r15", "r16", "r17", "r18", 
	"r19", "r20", "r21", "r22", "r23",
	"r24", "r25",
	/* linker registers */
	"r26", "r27", "r28", "r29",
	"r30", "r31",
	};

int rstatus[] = {
	SAREG,        SAREG,        SAREG|STAREG, SAREG|STAREG, SAREG|STAREG,
	SAREG|STAREG, SAREG|STAREG, SAREG|STAREG, SAREG|STAREG, SAREG|STAREG, 
	SAREG|STAREG, SAREG|STAREG, SAREG|STAREG, 
	SAREG,
	SAREG, SAREG, SAREG, SAREG, SAREG,
	SAREG, SAREG, SAREG, SAREG, SAREG,
	SAREG, SAREG, 
	SAREG, SAREG, SAREG, SAREG,
	SAREG, SAREG
	};

tlen(p) register NODE *p;
{
	switch(p->in.type) {
		case CHAR:
		case UCHAR:
			return(1);

		case SHORT:
		case USHORT:
			return(2);

		case DOUBLE:
			return(8);

		default:
			return(4);
		}
}

mixtypes(p, q) NODE *p, *q;
{
	register tp, tq;

	tp = p->in.type;
	tq = q->in.type;

	return( (tp==FLOAT || tp==DOUBLE) !=
		(tq==FLOAT || tq==DOUBLE) );
}

prtype(n, where) NODE *n; register where;
{
			/* calls to printf changed to putchar */
	switch (n->in.type)
		{
		case DOUBLE:
			if (where) {
				putchar('.');
			}
			putchar('d');
			return;

		case FLOAT:
			if (!where) {
				putchar('s');
			}
			return;

		case LONG:
		case ULONG:
		case INT:
		case UNSIGNED:
			if (!where) {
				putchar('w');
			}
			return;

		case SHORT:
		case USHORT:
			if (where) {
				putchar('.');
			}
			putchar('h');
			return;

		case CHAR:
		case UCHAR:
			if (where) {
				putchar('.');
			}
			putchar('b');
			return;

		default:
			if ( !ISPTR( n->in.type ) ) cerror("zzzcode- bad type");
			else {
				if (!where) {
					putchar('w');
				}
				return;
				}
		}
}

zzzcode( p, c ) register NODE *p; {
	register m;
	CONSZ val;
	switch( c ){

	case 'N':  /* logical ops, turned into 0-1 */
		/* use register given by register 1 */
		cbgen( 1, m=getlab(), 'I' );
		imove(getlr(p, '1')->tn.rval, 1);
		deflab( p->bn.label );
		imove(getlr(p, '1')->tn.rval, 0);
		deflab( m );
		return;

	case 'I':
	case 'P':
		cbgen( p->in.op, p->bn.label, c );
		return;

	case 'A':
		{
		register NODE *l, *r;
		register NODE *tmp;
		TWORD regtype;

		if (xdebug) eprint(p, 0, &val, &val);
		r = getlr(p, 'R');
		tmp = getlr(NULL, '1');
		if (optype(p->in.op) == LTYPE || p->in.op == UNARY MUL)
			{
			l = resc;
			if(r->in.type==FLOAT || r->in.type==DOUBLE)
				l->in.type = r->in.type;
			else
				l->in.type = INT;
			}
		else
			l = getlr(p, 'L');

		regtype = l->in.type;

		if (l->in.op == REG)
			{
			switch(regtype){
				case FLOAT:
				case DOUBLE:
				case UNSIGNED:
				case ULONG:
					break;
				case UCHAR:
				case USHORT:
					l->in.type = UNSIGNED;
					break;
				default:
					l->in.type = INT;
				}
			}

		if (r->in.op == ICON  && r->in.name[0] == '\0') {
			if ( l->in.op == REG )
				load(r, l);
			else if (r->tn.lval) {
				load(r, tmp);
				stor(tmp, l);
			}
			else {
				NODE reg0;
				reg0.in.op = REG;
				reg0.tn.rval = 0;
				stor(&reg0, l);
			}
			return;
		}

		if (tlen(l) == tlen(r)) {
			if ( l->in.op == REG || r->in.op == REG ) {
				if ( l->in.op == REG ) {
					load( r, l);
					goto done;
				}
				stor(r, l);
				return;
			}
			/* memory to memory */
			load(r, tmp);
			stor(tmp, l);
			return;
		}
		if ( l->in.op == REG && r->in.op == REG ) {
			rmove(l->tn.rval, r->tn.rval, INT);
			goto done;
		}
		if ( tlen(l) < tlen(r) ) {
			/* lhs cannot be a register */
			if ( r->in.op == REG ) {
				stor(r, l);
				return;
			}
			/* memory to memory */
			load(r, tmp);
			stor(tmp, l);
			return;
		}
		/* tlen(l) > tlen(r) */
		if ( l->in.op != REG && r->in.op != REG ) {
			load(r, tmp);
			stor(tmp, l);
			return;
		}
		if ( l->in.op == REG )
			load(r, l);
		else {
			/* rhs must be a reg */
			if ( r->in.op != REG )
				cerror ( "rhs not a register");
			rmove(tmp->tn.rval, r->tn.rval, INT);
			extend( tmp->tn.rval, r->in.type );
			stor(tmp, l);
			return;
		}
	done:
		l->in.type = regtype;
		if( l->in.op == REG && tlen(l) < tlen(r) && regtype != FLOAT)
			extend( l->tn.rval, l->in.type );
		return;
		}

	case 'C':	/* num words pushed on arg stack */
		{
		extern int gc_numbytes;
		extern int xdebug;

		if (xdebug) printf("->%d<-",gc_numbytes);

		if ( gc_numbytes ) {
			printf("\taddu	r31,r31,%d\n", gc_numbytes);
		}
		return;
		}

	case 'Y':
		{
		char c;
		NODE *src;
		int dst;
		if ( p->in.type == FLOAT || p->in.op != REG )
		{
			src = getlr(NULL, '1');
			src->in.off = p->in.off;
			src->in.arg = p->in.arg;
			src->in.dst = p->in.dst;
			src->in.alink = p->in.alink;
			src->in.type = p->in.type;
		}
		else
			src = getlr(p, 'L');
		dst = p->in.dst;
		if ( p->in.dst != SP )
		{
			if (p->in.off)
			{
				if (p->in.type==FLOAT || p->in.type==DOUBLE)
					fputs("\tst.d	", stdout);
				else
					fputs("\tst	", stdout);
				regput(src);
				putchar(',');
				printf("r31,CTMP%d+%d\n", 
					ftnno, p->in.off+pushcnt );
				USAGE(FP);
			}
			else
			{
				if ( src->tn.rval != dst )
					rmove(dst, src->tn.rval, 
						p->in.type == FLOAT ?
							DOUBLE : p->in.type);

				/* mark the register unavailable */
				rstatus[dst] &= ~(STBREG|STAREG);
				if (p->in.type == DOUBLE || 
						p->in.type == FLOAT) {
					rstatus[dst+1] &= ~(STBREG|STAREG);
				}
			}
		}
		else
			push(src);
		if (p->in.arg == 0 && p->in.alink)
			reloadregs(p->in.alink);
		}
		break;
		
		
	case 'D':	/* INCR and DECR */
		{
		register NODE *l;
		register inc;

		zzzcode(p->in.left, 'A');
		l = p->in.left;
		inc = p->in.right->tn.lval;
		if ( l->in.op != REG ) {
			l = getlr( NULL, '2' );

			if ( inc < 0 || inc > 0xffff )
				imove( R13, inc);
			if ( p->in.op == DECR )
				fputs("\tsubu	", stdout);
			else
				fputs("\taddu	", stdout);
			regput(l);
			putchar(',');
			regput(getlr(NULL, '1'));
			putchar(',');
			if ( inc < 0 || inc > 0xffff )
				fputs("r13", stdout);
			else
				conput(p->in.right);
			putchar('\n');
			stor(l, p->in.left);
			}
		else {
			if ( inc < 0 || inc > 0xffff )
				imove(R13, inc);
			fputs(p->in.op == INCR ? "\taddu\t" : "\tsubu\t", 
				stdout);
			regput(l);
			putchar(',');
			regput(l);
			putchar(',');
			if ( inc < 0 || inc > 0xffff )
				fputs("r13", stdout);
			else
				conput(p->in.right);
			putchar('\n');
			switch(l->in.type) {
				case UCHAR:
				case USHORT:
					extend(l->tn.rval, l->in.type);
					break;
				default:
					break;
			}
			}
		return;
		}


	case 'E':	/* INCR and DECR, FOREFF */
		{
		register NODE *l;
		register inc;

		l = p->in.left;
		inc = p->in.right->tn.lval;
		if ( inc < 0 || inc > 0xffff ) {
			imove(R13, inc);
		}
		if ( l->in.op != REG )
			load(p->in.left, l = getlr( NULL, '1'));
		fputs( p->in.op == INCR ? "\taddu\t" : "\tsubu\t", stdout );
		regput(l);
		putchar(',');
		regput(l);
		putchar(',');
		if ( inc < 0 || inc > 0xffff )
			fputs("r13", stdout);
		else
			conput(p->in.right);
		putchar('\n');
		if ( l != p->in.left )
			stor(l, p->in.left);
		return;
		}

	case 'L':	/* type of left operand */
	case 'R':	/* type of right operand */
		{
		register NODE *n;
		extern int xdebug;

		n = getlr ( p, c);
		if (xdebug) printf("->%d<-", n->in.type);

		prtype(n, 0);
		return;
		}

	case '1':
		targetreg = '1';
		break;

	case '2':
		targetreg = '2';
		break;

	case 'l':
		load( getlr(p, 'L'), getlr( NULL, targetreg));
		return;

	case 'r':
		load( getlr(p, 'R'), getlr( NULL, targetreg));
		return;

	case 's':
		stor( getlr( NULL, targetreg), getlr(p, 'L'));
		return;

	case 't':
		stor( getlr( NULL, targetreg), getlr(p, 'R'));
		return;

	case 'n':
		set_r13(NULL);
		return;

	case 'Q':	/* Logical or Arithmetic shift */
		if ( ISUNSIGNED(p->in.left->in.type) ) {
			putchar('u');
		}
		return;
	case 'X':			/* shift count */
		{
		register NODE *r;
		r = getlr(p, 'R');
		if (r->in.op == REG)
			regput(r);
		else {
			conput(r);
		}
		}
		return;

	case 'U':
		cmpinst(p);
		return;

	case 'V':
		if (p->in.op != REG)
			p = getlr( NULL, '1');
		usebcnd = 1;
		bcndreg = p->tn.rval;
		return;

	case 'W':
		/* no unsigned multiply instruction */
		if (p->in.op == MUL || p->in.op == ASG MUL)
			break;
		/* fall thru */
	case 'M':
		if ( ISUNSIGNED(p->in.type) ) {
			putchar('u');
		}
		return;

	case 'G':
		{
		register mask;
		register char *rn;
		val = getlr(p, 'R')->tn.lval;
		mask = 0;
		for (m = 0; m < fldsz ; m++) mask |= (1 << m);
		val &= mask;
		val <<= fldshf;
		mask <<= fldshf;
		m = getlr(NULL, '1')->tn.rval;
		rn = rnames[m];
		USAGE(m);
		if (mask != val)
			printf("	clr	%s,%s,%d<%d>\n", rn, rn,
				fldsz, fldshf);
		if (val) {
			if (val & 0xffff0000)
				printf("	or.u	%s,%s,0x%x\n", rn, rn,
					(val>>16) & 0xFFFF);
			if (val & 0xffff)
				printf("	or	%s,%s,0x%x\n", rn, rn, 
					val&0xFFFF);
		}
		return;
		}
			

	case 'Z':
		{
		register incr, type;
		register NODE *l;
		switch(p->in.type) {
			case CHAR:
			case UCHAR:
				if (p->in.left->in.type == SHORT || 
					p->in.left->in.type == USHORT)
					incr = 1;
				else
					incr = 3;
				break;
			case SHORT:
			case USHORT:
				incr = 2;
				break;
			default:
				cerror("zzcode case Z");
		}
		l = p->in.left;
		if (l->in.op == OREG && R2TEST(l->tn.rval)) {
			load(l, getlr(NULL, '1'));
			extend(getlr(NULL, '1')->tn.rval, p->in.type);
			break;
		}
		type = l->in.type;
		l->in.type = p->in.type;
		l->tn.lval += incr;

		load(l, getlr(NULL, '1'));

		l->in.type = type;
		l->tn.lval -= incr;
		break;
		}

	case 'T':	/* rounded structure length for arguments */
		{
		int size;

		size = p->stn.stsize;
		SETOFF( size, 4);
		if ((( pushcnt + size ) & 7) != (p->in.off & 7)) {
			size += 4;
		}
		pushcnt += size;
		printf("%d", size);
		instructarg = size;
		return;
		}

	case 'S':  /* structure assignment */
		{
			register NODE *l, *r, *l1, *r1;
			register size;
			register char *s1, *s2;
			NODE rn, ln;

			if( p->in.op == STASG ){
				l = p->in.left;
				r = p->in.right;

				}
#if 0
			else if( p->in.op == STARG ){  /* store an arg into a temporary */
				l = getlr( p, '3' );
				r = p->in.left;
				}
#endif
			else cerror( "STASG bad" );

			if( r->in.op == ICON ) r->in.op = NAME;
			else if( r->in.op == REG ) r->in.op = OREG;
			else if( r->in.op != OREG ) cerror( "STASG-r" );

			size = p->stn.stsize;

			if( size <= 0 || size > 65535 )
				cerror("structure size <0=0 or >65535");

			val = p->stn.stalign;
			if (size % val) cerror("structure alignment");
			if ( val == 1 )
				s1 = ".b";
			else if (val == 2) {
				s1 = ".h";
				size >>= 1;
			} else if (val == 8) {
				s1 = ".d";
				size >>= 3;
			} else {
				s1 = "";
				size >>= 2;
			}
#define INLINE	2
			if (size > INLINE || r->in.op == NAME ||
				(r->in.op == OREG && R2TEST(r->tn.rval)) ||
				(r->in.op == OREG && r->tn.lval < 0) ||
				(r->in.op == OREG && r->tn.lval > 0xffff)) {
				loada(12, r);	/* R12 */
				rn.tn.op = OREG;
				rn.tn.rval = 12;	
				rn.tn.lval = 0;
				rn.tn.name = "";
				r1 = &rn;
			}
			else
				r1 = r;

			if (instructarg ||
				size > INLINE || l->in.op == NAME ||
				(l->in.op == OREG && R2TEST(l->tn.rval)) ||
				(l->in.op == OREG && l->tn.lval < 0) ||
				(l->in.op == OREG && l->tn.lval > 0xffff)) {

				instructarg ? rmove(11, 31, INT) : loada(11, l);

				ln.tn.op = OREG;
				ln.tn.rval = 11;	/* R11 */
				ln.tn.lval = 0;
				ln.tn.name = "";
				l1 = &ln;
			}
			else
				l1 = l;

			if (size <= INLINE) {
				int rlval, llval;
				if (l1->in.op != OREG || r1->in.op != OREG)
					cerror("STASG OREG");
				rlval = r1->tn.lval;
				llval = l1->tn.lval;
				for (m = 0; m < size ; m++) {
					l1->tn.lval += val * m;
					r1->tn.lval += val * m;
					printf("\tld%s\tr13,",s1);
					adrput(r1);
					printf("\n\tst%s\tr13,",s1);
					adrput(l1);
					putchar('\n');
				}
				r1->tn.lval = rlval;
				l1->tn.lval = llval;
			}
			else {
				imove(4, size - 1);
				m = getlab();
				deflab(m);
				printf("\tld%s\tr13,r12[r4]\n",s1);
				printf("\tst%s\tr13,r11[r4]\n",s1);
				printf("\tbcnd.n\tne0,r4,@L%d\n", m);
				fputs ("\tsubu\tr4,r4,1\n", stdout);

			}
			set_r13(NULL);
			if( r->in.op == NAME ) r->in.op = ICON;
			else if( r->in.op == OREG ) r->in.op = REG;

			if (instructarg && p->in.arg == 0 && p->in.alink)
				reloadregs(p->in.alink);
			instructarg = 0;
		}
		break;

	case 'K':
		extend( p->in.left->tn.rval, p->in.type );
		break;
	case 'O':
		{
		register NODE *t;

		t = getlr(NULL, '1');
		if ( p->in.left->in.op != REG 
			|| p->in.left->tn.rval != t->tn.rval ) {
			if (p->in.left->in.op == REG) {
				rmove(t->tn.rval, p->in.left->tn.rval, LONG);
			}
			else {
				load(p->in.left, t);
			}
		}

		if ( p->in.op == SCONV )
			extend (t->tn.rval, p->in.type);
		else if ( p->in.op == PCONV )
			extend (t->tn.rval, p->in.left->in.type);
		else 
			cerror("case O for [PS]CONV");
		break;
		}

	case '3':		/* CPU BUG WITH FLOATING POINT ??.MV */
		if ((cpuflags & 8) == 0 &&
			(p->in.op == MUL || p->in.op == ASG MUL))
			fputs("	tb1	0,r0,511\n", stdout);
		break;

	case 'H':
		if (p->in.type == DOUBLE) {
			putchar('d');
		}
		else if (p->in.type == FLOAT) {
			putchar('s');
		}
		else
			cerror("bad case H in zzzcode");
		break;

	default:
		cerror( "illegal zzzcode" );
		}
	}

rmove( rt, rs, t ){
	printf( "	or	%s,r0,%s\n", rnames[rt], rnames[rs]);
	USAGE(rs);
	USAGE(rt);
	if (rt == R13)
		set_r13(NULL);
	if (t == DOUBLE) {
		printf( "	or	%s,r0,%s\n",rnames[rt+1],rnames[rs+1]);
		USAGE(rs+1);
		USAGE(rt+1);
		if ((rt+1) == R13)
			set_r13(NULL);
	}
	}

imove( rt, val ) 
register rt;
long val;
{
	if ( val < 0 && val > 0xffff0000) {
		val = -val;
		printf("	subu	%s,r0,0x%x\n", rnames[rt], val&0xFFFF);
	}
	else if (val & 0xffff0000) {
		printf("	or.u	%s,r0,0x%x\n", rnames[rt],
							(val>>16) & 0xFFFF);
		if (val & 0xffff)
			printf("	or	%s,%s,0x%x\n", rnames[rt], 					rnames[rt], val&0xFFFF);
	}
	else {
		printf("	addu	%s,r0,0x%x\n", rnames[rt], val&0xFFFF);
	}
	USAGE(rt);
	if (rt == R13)
		set_r13(NULL);
}

struct respref
respref[] = {
	INTAREG|INTBREG,	INTAREG|INTBREG,
	INAREG|INBREG,	INAREG|INBREG|SOREG|STARREG|STARNM|SNAME|SCON,
	INTEMP,	INTEMP,
	FORARG,	FORARG,
	INTEMP,	INTAREG|INAREG|INTBREG|INBREG|SOREG|STARREG|STARNM,
	0,	0 };

setregs(){ /* set up temporary registers */
	fregs = 10;
	}

szty(t){ /* size, in registers, needed to hold thing of type t */
	return( (t==DOUBLE) ? 2 : 1 );
	}

rewfld( p ) NODE *p; {
	return(1);
	}

callreg(p) NODE *p; {
	return( R2 );
	}

int nor2reg;

base( p, ty1 ) register NODE *p; int ty1; {

	if( nor2reg ) return ( -1 );
	if( p->in.op==REG ) return( p->tn.rval );
	return( -1 );
	}

offset( p, tyl ) register NODE *p; int tyl; {

	if( tyl==1 && p->in.op==REG) 
		return( p->tn.rval );
	if( (p->in.op==LS && p->in.left->in.op==REG &&
	      (p->in.right->in.op==ICON && p->in.right->in.name[0]=='\0')
	      && (1<<p->in.right->tn.lval)==tyl))
		return( p->in.left->tn.rval );
	return( -1 );
	}

r2regs( p, q ) register NODE *p, *q; { /* p is U* q is a +/- on its left */
	register NODE *ql, *qr;
	register i, r;

	ql = q->in.left;
	qr = q->in.right;

	if( q->in.op == PLUS) {
		if( (r=base(ql, tlen(p)))>=0 && (i=offset(qr, tlen(p)))>=0) {
			makeor2(p, ql, r, i);
			return(1);
		} else if( (r=base(qr, tlen(p)))>=0 &&
			   (i=offset(ql, tlen(p)))>=0) {
			makeor2(p, qr, r, i);
			return(1);
			}
		}
	return(0);

	}

makeor2( p, q, b, o) register NODE *p, *q; register int b, o; {
	register NODE *t;
	register int i;
	NODE *f;

	if (tdebug)
		printf( "makeor2(p=0%o, q=0%o, b=%d, o=%d)\n", p, q, b, o );
	p->in.op = OREG;
	f = p->in.left; 	/* have to free this subtree later */

	/* init base */
	switch (q->in.op) {
		case ICON:
		case REG:
			t = q;
			break;

		default:
			cerror("illegal makeor2");
	}

	p->tn.lval = t->tn.lval;
#ifdef FLEXNAMES
	p->in.name = t->in.name;
#else
	for(i=0; i<NCHNAM; ++i)
		p->in.name[i] = t->in.name[i];
#endif

	/* init offset */
	p->tn.rval = R2PACK( (b & 0177), o, (b>>7) );

	tfree(f);
	return;
	}

canaddr( p ) NODE *p; {
	return(p->in.op == REG);
	}

shltype( o, p ) register o; {
	return( o== REG || o == NAME || o == ICON || o == FCON || o == OREG );
	}

flshape( p ) NODE *p; {
	register o;
	o = p->in.op;
	return( o == REG || o == NAME || o == ICON || o == OREG );
	}

shtemp( p ) register NODE *p; {
	if( p->in.op == STARG ) p = p->in.left;
	return( p->in.op==NAME || p->in.op ==ICON || p->in.op == OREG );
	}

shumul( p ) NODE *p; {
	return( 0 );
	}

adrcon( val ) CONSZ val; {
	putchar( '$' );
	printf( CONFMT, val );
	}

conput( p ) register NODE *p; {
	switch( p->in.op ){

	case ICON:
		acon( p );
		return;

	case REG:
		fputs( rnames[p->tn.rval], stdout );
		USAGE(p->tn.rval);
		return;

	default:
		cerror( "illegal conput" );
		}
	}

insput( p ) register NODE *p; {
	cerror( "insput" );
	}

upput( p ) register NODE *p; {
	register CONSZ lval;

#ifdef UPPUT
	if ( p->in.type == DOUBLE )
		switch( p->in.op ) {
			case OREG:
				if (R2TEST(p->tn.rval))
					break;
			case NAME:
				lval = p->tn.lval;
				p->tn.lval += 4;
				adrput(p);
				p->tn.lval = lval;
				return;
			default:
				break;
			}
#endif
	cerror( "upput" );
	}

regput( p ) register NODE *p; {
	if ( p->in.op == REG )
		adrput(p);
	else 
		cerror("regput");
	return;
}

oregput( p ) register NODE *p; {
	if( p->in.op == FLD ){
		p = p->in.left;
		}
	if ( p->in.op == OREG )
		adrput(p);
	else 
		cerror("oregput");
	return;
}

adrput( p ) register NODE *p; {
	register int r;
	/* output an address, with offsets, from p */

	if( p->in.op == FLD ){
		p = p->in.left;
		}
	switch( p->in.op ){

	case NAME:
		acon( p );
		return;

	case ICON:
		/* addressable value of the constant */
		/* putchar( '$' ); */
		acon( p );
		return;
#ifndef FORT
	case FCON:

		printf("$0%c%.20e", p->fpn.type == FLOAT?'f':'d', p->fpn.dval);
		return;
#endif

	case REG:
		fputs( rnames[p->tn.rval], stdout );
		USAGE(p->tn.rval);
		return;

	case OREG:
		r = p->tn.rval;
		if( R2TEST(r) ){ /* double indexing */
			printf( "%s", rnames[R2UPK1(r)] );
			printf( "[%s]", rnames[R2UPK2(r)] );
			USAGE(R2UPK2(r));
			USAGE(R2UPK1(r));
			return;
			}
		if (r == TMPREG)
			p->tn.lval += pushcnt;
		printf( "%s,", rnames[p->tn.rval] );
		if (r != R13 && p->in.name[0]) cerror("relocatable offset");
		if (p->in.name[0] == '\0' && 
				((p->tn.lval < 0) || p->tn.lval > 0xffff))
			cerror("bad register offset");
		acon(p);
#ifdef FORT			 /* check for "2nd pass" temp */
		if (r == TMPREG && p->in.name[2] == '\1')
			printf("+ESP%d", ftnno);     /* symbolic addressing */
#endif /*FORT*/
		USAGE(p->tn.rval);
		if (r == TMPREG)
			p->tn.lval -= pushcnt;
		return;

	default:
		cerror( "illegal address" );
		return;

		}

	}

acon( p ) register NODE *p; { /* print out a constant */

	if( p->in.name[0] == '\0' ){
		printf( CONFMT, p->tn.lval);
		}
	else if( p->tn.lval == 0 ) {
#ifdef FLEXNAMES
		printf( "%s", p->in.name );
#else
		printf( "%.8s", p->in.name );
#endif
		}
	else {
#ifdef FLEXNAMES
		printf( "%s+", p->in.name );
#else
		printf( "%.8s+", p->in.name );
#endif
		printf( CONFMT, p->tn.lval );
		}
	}

/*
aacon( p ) register NODE *p; { /* print out a constant */
/*

	if( p->in.name[0] == '\0' ){
		printf( CONFMT, p->tn.lval);
		return( 0 );
		}
	else if( p->tn.lval == 0 ) {
#ifdef FLEXNAMES
		printf( "$%s", p->in.name );
#else
		printf( "$%.8s", p->in.name );
#endif
		return( 1 );
		}
	else {
		fputs( "$(", stdout );
		printf( CONFMT, p->tn.lval );
		putchar( '+' );
#ifdef FLEXNAMES
		printf( "%s)", p->in.name );
#else
		printf( "%.8s)", p->in.name );
#endif
		return(1);
		}
	}
 */

genscall( p, cookie ) register NODE *p; {
	/* structure valued call */
	register int sz, al, ret, val;
	sz = p->stn.stsize;
	al = p->stn.stalign;
	calltmp += 4;
	if (al == 8 && (calltmp&4))
		calltmp += 4;
	val = calltmp;

	calltmp += sz;
	
	if (sz == 4 && al == 4) {
		ret = callcomm( p, cookie, 0 );
		printf("\tst	r2,r31,CTMP%d+%d\n", ftnno, val + pushcnt);
	}
	else {
		ret = callcomm( p, cookie, val );
	}
	printf("\taddu	r2,r31,CTMP%d+%d\n", ftnno, val + pushcnt);

	return (ret);
	}

gencall( p, cookie ) register NODE *p; {
	/* NON structure valued call */
	return( callcomm( p, cookie, 0 ) );
	}

int gc_numbytes;

callcomm( p, cookie, strfunc ) register NODE *p; {
	/* generate the call given by p */
	register NODE *p1, *ptemp;
	register temp, temp1;
	register m;
	int opushcnt, npushed;


	leaffunc = 0;
	if( p->in.right ) temp = argsize( p->in.right );
	else temp = 0;

	if( p->in.op == STCALL || p->in.op == UNARY STCALL ){
		/* set aside room for structure return */

		if( p->stn.stsize > temp ) temp1 = p->stn.stsize;
		else temp1 = temp;
		}

	if( temp > maxargs ) maxargs = temp;
	SETOFF(temp1,4);

	opushcnt = pushcnt;
	if( p->in.right ){ /* make temp node, put offset in, and generate args */
		ptemp = talloc();
		ptemp->in.op = OREG;
		ptemp->tn.lval = -1;
		ptemp->tn.rval = SP;
#ifdef FLEXNAMES
		ptemp->in.name = "";
#else
		ptemp->in.name[0] = '\0';
#endif
		ptemp->in.rall = NOPREF;
		ptemp->in.su = 0;
		temp = markargs(p->in.right);
		genargs( p->in.right, ptemp );
		ptemp->in.op = FREE;
		}

	p1 = p->in.left;
	if( p1->in.op != ICON ){
		if( p1->in.op != REG ){
			order( p1, INAREG );
			}
		}

	/* restore tmp status of register r2-r9 */
	for (m = R2; m < R10; m++) 
		rstatus[m] |= STAREG;
	setregs();

/*
	if( p1->in.op == REG && p->tn.rval == R5 ){
		cerror( "call register overwrite" );
		}
 */
/*	setup gc_numbytes so reference to ZC works */

	if (pushcnt) {
		npushed = pushcnt - opushcnt;
		if (temp) {
			temp1 = temp >> 16;		/* begin */
			temp = (unsigned short) temp;	/* end */
			m = (temp1 - R2) * sizeof(int);
		}
		else {
			/* align stack if required */
			if (npushed == 0 && (pushcnt & 4))
				m = 36;
			else
				m = 32;
		}
		/* new argument homing area required */
		if (m) {
			printf("\tsubu	r31,r31,%d\n", m);
			pushcnt += m;
		}
		/*pop(arguments pushed by this func+above homing area)*/
		gc_numbytes = npushed + m;
		if (temp) {
			do {
				if( p1->in.op == REG &&
					p1->tn.rval == temp1) {
						/* move to R10 */
						rfree(temp1, INT);
						rbusy(10,INT);
						rmove(10, temp1, INT);
						p1->tn.rval = 10;		
					}
				m = (temp1 - R2) * sizeof(int);
				printf("\tld	r%d,r31,%d\n", temp1, m);
				temp1 ++;
			} while (temp1 <= temp);
		}
	}
	else
		gc_numbytes = 0;

	if (strfunc) {
		printf("\taddu	r12,r31,CTMP%d+%d\n", ftnno, strfunc + pushcnt);
	}
	pushcnt = opushcnt;

	p->in.op = UNARY CALL;
	m = match( p, INTAREG|INTBREG );

	set_r13(NULL);

	return(m != MDONE);
	}

char *
ccbranches[] = {
	"eq", 
	"ne", 
	"le", 
	"lt", 
	"ge", 
	"gt", 
	"ls", 
	"lo", 
	"hs", 
	"hi", 
	};

cbgen( o, lab, mode ) { /*   printf conditional and unconditional branches */

	if( o == 0 ) printf( "	br     @L%d\n", lab );
	else if( o == 1 ) printf( "	br.n     @L%d\n", lab );
	else {
		if( o > UGT ) cerror( "bad conditional branch: %s", opst[o] );
		if (usebcnd) {
			printf( "	bcnd	%s0,%s,@L%d\n", 
				ccbranches[o-EQ], rnames[bcndreg], lab);
			USAGE(bcndreg);
			usebcnd = 0;
			bcndreg = 0;
		}
		else {
			printf( "	bb1	%s,r13,@L%d\n", 
				ccbranches[o-EQ], lab);
			set_r13(NULL);
			}
		}
	}

nextcook( p, cookie ) NODE *p; {
	/* we have failed to match p with cookie; try another */
	if( cookie == FORREW ) return( 0 );  /* hopeless! */
	if( !(cookie&(INTAREG|INTBREG)) ) return( INTAREG|INTBREG );
	if( !(cookie&INTEMP) && asgop(p->in.op) ) return( INTEMP|INAREG|INTAREG|INTBREG|INBREG );
	return( FORREW );
	}

lastchance( p, cook ) NODE *p; {
	/* forget it! */
	return(0);
	}

optim2( p ) NODE *p; {
	/* do local tree transformations and optimizations */
}

NODE * addroreg(l)
				/* OREG was built in clocal()
				 * for an auto or formal parameter
				 * now its address is being taken
				 * local code must unwind it
				 * back to PLUS/MINUS REG ICON
				 * according to local conventions
				 */
{
	cerror("address of OREG taken");
}



# if !defined( ONEPASS ) && !defined( FONEPASS )

main( argc, argv ) char *argv[]; {
	return( mainp2( argc, argv ) );
	}

# endif


myreader(p) register NODE *p; {
	canon( p );		/* expands r-vals for fileds */
	}

special( p, shape ) register NODE *p; {

	register lval, icon;
	icon = p->in.op == ICON && p->in.name[0] == '\0';
	lval = p->tn.lval;

	switch( shape ){

	case SSCON:
		if( icon && lval > -32769 && lval < 32768 )
			return(1);
		break;
	case SCCON:
		if( icon && lval > -129 && lval < 128 )
			return(1);
		break;
	case SPCON16:
		if( icon && lval >= 0 && lval < 0x10000 )
			return(1);
		break;
	case SMULCON:
		if( icon && lval >= 0 && lval < 0x10000 ) {
			/* work around for imm. div / divu bug */
			if ((lval & 0x7800) == 0x3800)
				return (0);
			return(1);
		}
		break;
	case SICON:
		if (icon)
			return (1);
		break;
	default:
		cerror( "bad special shape" );

		}

	return(0);
	}


cmpinst(p)
register NODE *p;
{
	register NODE *l, *r;
	register c;

	l = p->in.left;
	r = p->in.right;
	c = '1';

	if ( l->in.op != REG )
	{
		load(p->in.left, l = getlr(NULL, c));
		c++;
	}
	if ( r->in.op != REG && r->in.op != ICON )
	{
		load(p->in.right, r = getlr(NULL, c));
	}
	else if ( r->in.op == ICON ) {
		register lval;
		lval = r->tn.lval;
		if (r->in.name[0] || lval < 0 || lval > 0xffff)
			load(p->in.right, r = getlr(NULL, c));
	}
	if ( r->in.op == ICON && r->tn.lval == 0 && p->in.op >= EQ &&
			p->in.op <= GT ) {
		usebcnd = 1;
		bcndreg = l->tn.rval;
		return;
	}
	else {
		usebcnd = 0;
		bcndreg = 0;
	}

	if (( c = p->in.left->in.type ) == DOUBLE)
		c = 'd';
	else if ( c == FLOAT )
		c = 's';
	else 
		c = 'w';

	if (c == 'w') {
		printf("	cmp	r13,");
		regput(l);
		putchar(',');
		if (r->in.op == ICON)
			conput(r);
		else
			regput(r);
	}
	else {
		/* fputs( "	tb1	0,r0,511\n", stdout);*/
		printf("	fcmp.s%c%c	r13,", c, c);
		regput(l);
		putchar(',');
		regput(r);
	}
	putchar('\n');
	set_r13(NULL);
}

gencmp(val)
register val;
{
	if (val & 0xffff0000) {
		imove(3, val);
		fputs( "\tcmp\tr13,r2,r3\n", stdout );
	}
	else {
		printf( "\tcmp\tr13,r2,%ld\n", val );
	}
	set_r13(NULL);
}

struct load_str {TWORD fromtype; char *tostrng;} load_strs[] =
	{
	CHAR,   "	ld.b	",
	SHORT,  "	ld.h	",
	FLOAT,  "	ld	",
	DOUBLE, "	ld.d	",
	UCHAR,	"	ld.bu	",
	USHORT,	"	ld.hu	",
	0,      "	ld	"
};

int laddrflt;
int saddrflt;

load(src, dst)
register NODE *src;
register NODE *dst;
{
	register TWORD type;
	register struct load_str *p;
	register unsigned long ulval;
	int fld = 0;
	int off, mask;

	if ( dst->in.op != REG )
		cerror("cannot load rhs\n");

	type = src->in.type;
	if (src->in.op == FLD) {
		src = src->in.left;
		/* sanity checks on fields */
		if (src->in.op == NAME) {
			if (src->tn.lval & 3)
				werror("field alignment (NAME)");
		}
		else if (src->in.op == OREG) {
			if (!R2TEST(src->tn.rval)) {
				if (src->tn.lval & 3)
					werror("field alignment (OREG)");
			}
		}
		else
			cerror("field address type");
		fld = 1;
	}
	if ( src->in.op == REG ) {
		if ( src->tn.rval != dst->tn.rval )
			rmove(dst->tn.rval, src->tn.rval, type );
		if ( type == CHAR || type == UCHAR ||
			type == SHORT || type == USHORT )
			extend(dst->tn.rval, type);
		return;
	}
	if ( src->in.op == ICON && src->in.name[0] == '\0' ) {
		register lval;
		lval = src->tn.lval;
		switch(type) {
			case CHAR:
				lval = ( char ) lval;
				break;
			case UCHAR:
				lval = ( unsigned char ) lval;
				break;
			case SHORT:
				lval = ( short ) lval;
				break;
			case USHORT:
				lval = ( unsigned short ) lval;
				break;
		}
		imove(dst->tn.rval, lval);
		return;
	}
	if ( src->in.op == ICON )
		/* pointer type ICON's */
		switch( type ) {
			case CHAR:
			case UCHAR:
			case SHORT:
			case USHORT:
			case DOUBLE:
			case FLOAT:
				cerror("cannot load ICON");
				break;
			default:
				loadlong(src, dst->tn.rval, "	addu	");
				return;
		}
	else {
		if (fld)
			type = INT;
		for ( p=load_strs; p->fromtype ; p++)
			if (p->fromtype == type) break;

	}

	mask = tlen(src) - 1;
	ulval = src->tn.lval;
	if (src->in.op == NAME) {
		if (cpuflags & 16) {
			if (ulval & mask) {
				if (ulval > 0xfff8) {
					loada( R13, src);
				}
				else {
					fputs("	or.u	r13,r0,hi16(", stdout);
					acon(src);
					fputs(")\n\tor	r13,r13,lo16(", stdout);
					acon(src);
					fputs(")\n", stdout);	
				}
				mxmload(R13, 0, dst->tn.rval, type, ulval);
				return;
			}
		}
		loadlong(src, dst->tn.rval, p->tostrng);
	}
	else {
		NODE fake;

		if (cpuflags & 16 && src->in.op == OREG) {
			if (src->tn.rval == FP || src->tn.rval == R30)
			{
				if (ulval & mask) {
					int reg;
					if (ulval > 0xfff8) {
						loada( R13, src);
						reg = R13;
						off = 0;
					}
					else {
						reg = src->tn.rval;
						off = ulval;
					}
					mxmload(reg, off, dst->tn.rval, type, ulval);
					return;
				}
			}
			else {
				/* no info, assume worst case */
				int reg;

				if (ulval > 0xfff8 || R2TEST(src->tn.rval)) {
					loada( R13, src);
					reg = R13;
					off = 0;
				}
				else {
					reg = src->tn.rval;
					off = ulval;
				}
				mxmload(reg, off, dst->tn.rval, type, 
						cpuflags & 32 ? 1 : 2);
				return;
			}
		}
		if (src->in.op == OREG &&
				(ulval = src->tn.lval) > 0xffff &&
				!R2TEST(src->tn.rval)) {
			if (src->tn.rval == TMPREG)
				ulval += pushcnt;
			imove(R13, ulval);

			fake.in.op = OREG;
			fake.tn.lval = 0;
			fake.tn.rval = R13;
			fake.in.name = rnames[src->tn.rval];

			USAGE(src->tn.rval);
			USAGE(R13);
			set_r13(NULL);

			src = &fake;
		}
		fputs(p->tostrng, stdout);
		printf("%s,", rnames[dst->tn.rval]);
		oregput(src);
		putchar('\n');
		USAGE(dst->tn.rval);
	}
	if (cpuflags & 4) 
		fputs("	tb1	0,r0,511\n", stdout);
	return;
}

loada(reg, addr)
register reg;
register NODE *addr;
{
	register r, lval;
	register char *p;

	switch(addr->in.op) {
		default:
			cerror("cannot load address");
		case NAME:
			loadlong(addr, reg, "	addu	");
			return;
		case OREG:
			r = addr->tn.rval;
			lval = addr->tn.lval;
			if( R2TEST(r) )
				p = "	lda	r%d,";
			else  if (lval < 0) {
				addr->tn.lval = -lval;
				p = "	subu	r%d,";
			}
			else
				p = "	addu	r%d,";
			printf(p, reg);
			oregput(addr);
			putchar('\n');
			USAGE(reg);
			addr->tn.lval = lval;
	}
}

loadlong(src, dst, ldstr)
register NODE *src;
register dst;
char *ldstr;
{
	if (src->in.op == NAME || 
		(src->in.op == ICON && src->in.name[0]))  {
		if (! in_r13(src) ) {
			fputs("	or.u	r13,r0,hi16(", stdout);
			acon(src);
			fputs(")\n", stdout);
		}
		printf("%s%s,r13,lo16(", ldstr, rnames[dst]);
		acon(src);
		fputs(")\n", stdout);
		USAGE(dst);
		USAGE(R13);
		set_r13(src);
		return;
	}
	else
		cerror("loadlong");
}

stor(src, dst)
register NODE *src, *dst;
{
	register unsigned ulval;
	int off, mask, type;
	int fld;
	NODE fake;

	if ( src->in.op != REG )
		cerror("cannot store lhs\n");
	if( dst->in.op == FLD ){
		dst = dst->in.left;
		fld = 1;
	}
	else
		fld = 0;

	mask = tlen(dst) - 1;
	ulval = dst->tn.lval;
	type = dst->in.type;

	if (dst->in.op == OREG) {
		if (cpuflags & 16) {
			if (dst->tn.rval == FP || dst->tn.rval == R30)
			{
				if (ulval & mask) {
					int reg;
					if (ulval > 0xfff8) {
						loada( R13, dst);
						reg = R13;
						off = 0;
					}
					else {
						reg = dst->tn.rval;
						off = ulval;
					}
					mxmstor(src->tn.rval, reg, off, 
						fld ? INT : type, ulval);
					return;
				}
			}
			else {
				/* no info, assume worst case */
				int reg;

				if (ulval > 0xfff8 || R2TEST(dst->tn.rval)) {
					loada( R13, dst);
					reg = R13;
					off = 0;
				}
				else {
					reg = dst->tn.rval;
					off = ulval;
				}
				mxmstor(src->tn.rval, reg, off, 
						fld ? INT : type, 
						cpuflags & 32 ? 1 : 2);
				return;
			}
		}
		if ( (ulval = dst->tn.lval) > 0xffff &&
				!R2TEST(dst->tn.rval)) {
			if (dst->tn.rval == TMPREG)
				ulval += pushcnt;
			imove(R13, ulval);

			fake.in.op = OREG;
			fake.tn.lval = 0;
			fake.tn.rval = R13;
			fake.in.name = rnames[dst->tn.rval];
			fake.in.type = dst->in.type;

			USAGE(dst->tn.rval);
			USAGE(R13);
			set_r13(NULL);

			dst = &fake;
		}
		fputs("\tst", stdout);
		if (!fld)	/* bit fields get stored as longs */
			prtype(dst, 1);
		printf("\t%s,", rnames[src->tn.rval]);
		oregput(dst);
		putchar('\n');
	} else if (dst->in.op == NAME) {
		if (cpuflags & 16) {
			if (ulval & mask) {
				if (ulval > 0xfff8) {
					loada( R13, dst);
				}
				else {
					fputs("	or.u	r13,r0,hi16(", stdout);
					acon(dst);
					fputs(")\n\tor	r13,r13,lo16(", stdout);
					acon(dst);
					fputs(")\n", stdout);	
				}
				mxmstor(src->tn.rval, R13, 0, 
					fld ? INT : type, ulval);
				return;
			}
		}
		if ( !in_r13(dst) ) {
			fputs("\tor.u	r13,r0,hi16(", stdout);
			acon(dst);
			fputs(")\n", stdout);
		}
		fputs("\tst", stdout);
		if (!fld)	/* bit fields get stored as longs */
			prtype(dst, 1);
		printf("\t%s,r13,lo16(", rnames[src->tn.rval]);
		acon(dst);
		fputs(")\n", stdout);
		set_r13(dst);
		USAGE(R13);
	} else
		cerror("cannot store rhs\n");

	USAGE(src->tn.rval);
	if (cpuflags & 2)
		fputs("	tb1	0,r0,511\n", stdout);
}

extend(reg, type)
register type, reg;
{
	register char *s;
	switch(type)
	{
	case UCHAR:
       		s = "\textu	r%d,r%d,8<0>\n";
		break;
	case USHORT:
       		s = "\textu	r%d,r%d,16<0>\n";
		break;
	case CHAR:
       		s = "\text	r%d,r%d,8<0>\n";
		break;
	case SHORT:
       		s = "\text	r%d,r%d,16<0>\n";
		break;
	default:
		cerror("illegal type extension");
		break;
	}
	USAGE(reg);
	printf( s, reg, reg);
}

#ifdef FLEXNAMES
static char *lastname;
static lastlval;

in_r13(p)
register NODE *p;
{
	register char *cp;

#ifndef FORT
	/* MV linpak shows a problem with this fix it later. */
	if ((cp = lastname) && p->in.name) {
		if (p->tn.lval != lastlval)
			return (0);
		if ( p->in.name[0] == cp[0] && 
			p->in.name[1] == cp[1] &&
			strcmp(p->in.name, cp) == 0)
			return (1);
	}
#endif
	return 0;
}

set_r13(p)
register NODE *p;
{
	if (p) {
		lastname = p->in.name;
		lastlval = p->tn.lval;
	}
	else {
		lastname = NULL;
		lastlval = 0;
	}
}
#endif

changespoff(p)
register NODE *p;
{
	register NODE *l, *r;
	if ( p->in.op == PLUS && 
		(l = p->in.left)->in.op == REG && l->tn.rval == TMPREG && 
		(r = p->in.right)->in.op == ICON && r->in.name[0] == '\0')
			r->tn.lval += pushcnt;
}

mxmload(src, off, dst, type, al)
{
	int tmp;

	if (type == DOUBLE) {
		/* recurse */
		if (src == dst) {
			mxmload(src, off+4, dst+1, INT, al);
			mxmload(src, off, dst, INT, al);
		}
		else {
			mxmload(src, off, dst, INT, al);
			mxmload(src, off+4, dst+1, INT, al);
		}
		return;
	}

	leaffunc = 0;
	if (type == USHORT || type == SHORT) {
		if ((al & 1) == 0) {
			if (type == USHORT)
				printf("\tld.hu	r%d,r%d,%d\n", dst, src, off);
			else
				printf("\tld.h	r%d,r%d,%d\n", dst, src, off);
			USAGE(dst);
			USAGE(src);
			return;
		}
		printf("	ld.bu	r1,r%d,%d\n", src, off + 1);
		if (type == SHORT)
			printf("	ld.b	r%d,r%d,%d\n", dst, src, off);
		else
			printf("	ld.bu	r%d,r%d,%d\n", dst, src, off);
		printf("	mak	r%d,r%d,8\n", dst, dst);
	}
	else {	/* INT, UNSIGNED, LONG, ULONG, FLOAT, PTR */
		if (al & 1) {
			if (src == dst) {
				if (dst != R13)
					tmp = R13;
				else
					cerror("mxmload: out of registers");
				USAGE(tmp);
			}
			else
				tmp = dst;
			printf("	ld.bu	r%d,r%d,%d\n", tmp, src, off);
			printf("	ld.bu	r1,r%d,%d\n", src, off + 1);
			printf("	mak	r%d,r%d,24\n", tmp, tmp);
			printf("	mak	r1,r1,16\n");
			printf("	or	r%d,r%d,r1\n", tmp, tmp);
			printf("	ld.bu	r1,r%d,%d\n", src, off + 2);
			printf("	mak	r1,r1,8\n");
			printf("	or	r%d,r%d,r1\n", tmp, tmp);
			printf("	ld.bu	r1,r%d,%d\n", src, off + 3);
			printf("	or	r%d,r%d,r1\n", dst, tmp);
			goto no_or;
		}
		else if (al & 2) {
			printf("	ld.hu	r1,r%d,%d\n", src, off + 2);
			printf("	ld.hu	r%d,r%d,%d\n", dst, src, off);
			printf("	mak	r%d,r%d,16\n", dst, dst);
		}
		else if (al & 4) {
			printf("	ld	r%d,r%d,%d\n", dst, src, off);
			goto no_or;
		}
		else
			cerror("mxmload");
	}

	printf("	or	r%d,r%d,r1\n", dst, dst);
no_or:
	USAGE(src);
	USAGE(dst);
	USAGE(1);
	return;
}

mxmstor(src, dst, off, type, al)
{
	if (type == DOUBLE) {
		/* recurse */
		mxmstor(src, dst, off, INT, al);
		mxmstor(src+1, dst, off+4, INT, al);
		return;
	}

	leaffunc = 0;
	if (type == USHORT || type == SHORT) {
		if ((al & 1) == 0) {
			printf("\tst.h	r%d,r%d,%d\n", src, dst, off);
			USAGE(dst);
			USAGE(src);
			return;
		}
		printf("\tst.b	r%d,r%d,%d\n", src, dst, off+1);
		if (src) {
			printf("\textu	r1,r%d,8\n", src);
			printf("\tst.b	r1,r%d,%d\n", dst, off);
		}
		else
			printf("\tst.b	r0,r%d,%d\n", dst, off);
	}
	else {	/* INT, UNSIGNED, LONG, ULONG, FLOAT, PTR */
		if (al & 1) {
			printf("\tst.b	r%d,r%d,%d\n", src, dst, off+3);
			if (src) {
				printf("\textu	r1,r%d,8\n", src);
				printf("\tst.b	r1,r%d,%d\n", dst, off+2);
				printf("\textu	r1,r%d,16\n", src);
				printf("\tst.b	r1,r%d,%d\n", dst, off+1);
				printf("\textu	r1,r%d,24\n", src);
				printf("\tst.b	r1,r%d,%d\n", dst, off);
			}
			else {
				printf("\tst.b	r0,r%d,%d\n", dst, off+2);
				printf("\tst.b	r0,r%d,%d\n", dst, off+1);
				printf("\tst.b	r0,r%d,%d\n", dst, off);
			}
		}
		else if (al & 2) {
			printf("\tst.h	r%d,r%d,%d\n", src, dst, off+2);
			if (src) {
				printf("\textu	r1,r%d,16\n", src);
				printf("\tst.h	r1,r%d,%d\n", dst, off);
			}
			else
				printf("\tst.h	r0,r%d,%d\n", dst, off);
		}
		else if (al & 4) {
			printf("	st	r%d,r%d,%d\n", src, dst, off);
		}
		else
			cerror("mxmstor");
	}
}
