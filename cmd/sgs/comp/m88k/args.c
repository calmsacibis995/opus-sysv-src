# include <stdio.h>
# include "mfile2"


static NODE *arglist[256];
static NODE **end;

#ifndef R13
#define R13	13
#endif

#define one	(arglist[0])
#define last	(arglist[255])
int calltmp;
int pushcnt;
extern usedregs;
extern char *rnames[];
int leaffunc	=	1;
struct alink {
	struct alink *alink;
	int val;
	short reg;
	short type;
};

#define	ALINKSZ	128		/* power of 2 */
struct alink alink[ALINKSZ];
struct alink alfake;
extern cpuflags;

static alindx;

struct alink *
newalink(al, val, reg, type)
struct alink *al;
{
	register i, j;

	for (i = alindx, j = 0;  j < ALINKSZ; j++, i = (i+1) & (ALINKSZ-1))
		if (alink[i].alink == 0)
			break;
	if (j == ALINKSZ)
		cerror("arg link table size");
	alindx = i;

	alink[i].val = val;
	alink[i].alink = al;
	alink[i].reg = reg;
	alink[i].type = type;
	return (&alink[i]);
}

reloadregs(al)
register struct alink *al;
{
	extern int rstatus[];

	if (al->alink != &alfake) {
		reloadregs(al->alink);
	}
	al->alink = (struct alink *) 0;
	if (al->type)
	printf("\tld.d\tr%d,r31,CTMP%d+%d\n", al->reg, ftnno, al->val+pushcnt);
	else
	printf("\tld\tr%d,r31,CTMP%d+%d\n", al->reg, ftnno, al->val+pushcnt);
	/* mark the register unavailable */
	rstatus[al->reg] &= ~(STBREG|STAREG);
	if (al->type)
		rstatus[al->reg+1] &= ~(STBREG|STAREG);
}


markargs(p)
NODE *p;
{
	numberargs(p);
	return(alignargs());
}

static
numberargs(p)
register NODE *p;
{
	if ( p->in.op == CM )
	{
		numberargs(p->in.left);
		p = p->in.right;
		p->in.arg = end - arglist;
		*end++ = p;
		*end = NIL;
		if ( end >= &last )
			cerror("too many args. max 255\n");
		return;
	}

	/* first argument */
	end = arglist;
	p->in.arg = 0;
	*end++ = p;
	*end = NIL;
	return;
}

static
alignargs()
{
	register NODE *p;
	register i;
	register j, r;
	register real;
	int havecalls, havestructs, begreg, endreg;
	int tmp;
	register struct alink *al;

	endreg = begreg = havestructs = havecalls = j = 0;
	al = &alfake;
	r = R2;
	resetreg();
	for (i = 0; p = arglist[i] ; i++) {
		real = p->in.type == FLOAT || p->in.type == DOUBLE;
		if (p->in.op == STARG) {
			if (p->stn.stalign == 8 && (j & 7))
				j += 4;
			p->in.off = j;
			if (j < 32 && begreg == 0) {
				begreg = R2 + (j / sizeof(int));
			}
			j += argsize(p);
			r = R2 + (j / sizeof(int));
			havestructs = 1;
		}
		else if ((havestructs == 0) && (tmp = regarg(i, r, real))) {
			if (real && (r != tmp)) {
				j += (tmp - r) * 4;
			}
			r = tmp;
			p->in.dst = r;
			if (havecalls) {
				calltmp += 4;
				p->in.rall = NOPREF;
				if (real && (calltmp&4))
					calltmp += 4;
				p->in.off = calltmp;
				al = newalink(al, calltmp, r, real);
				if (real)
					calltmp += 4;
			}
			else {
				p->in.off = 0;
				p->in.rall = MUSTDO|r;
			}
			r += (real ? 2 : 1);
			j += (real ? 8 : 4);
		}
		else {
			p->in.dst = SP;
			if (real &&  (j & 7))		/* align reals */
				j += 4;

			p->in.off = j;
			j += argsize(p);
			r = R2 + (j / sizeof(int));
		}
		havecalls += iscall(p);
		p->in.alink = (struct alink *) 0;
	}
	if (havestructs && begreg) {
		j -= 4;		/* backup to last argument pushed */
		endreg = R2 + (j / sizeof(int));
		if (endreg > R9) endreg = R9;
	}
	if (al != &alfake) {
		arglist[0]->in.alink = al;	
#ifdef FORT
		p = arglist[0];
		if (p->in.op == COMOP) {
			/* reader.c delay1 will do a *p = *(p->in.right) */
			p->in.right->in.off = p->in.off;
			p->in.right->in.dst = p->in.dst;
			p->in.right->in.arg = p->in.arg;
			p->in.right->in.alink = p->in.alink;
		}
#endif
	}
	return((begreg << 16) | endreg);
}

static reglocal;
regarg(argno, regno, real)
register argno, regno, real;
{
	if (reglocal > R9)
		return 0;
	if (real && (regno & 1) )
		regno++;
	reglocal = regno;
	if (regno <= R9)
		return (regno);
	return 0;
}

resetreg()
{
	reglocal = 0;
}

static
iscall( p )
NODE *p;
{
	extern callflag;
	register save;

	if (p->in.op == STARG)
		return(1);

	save = callflag;
	callflag = 0;
	markcall(p);

	if ( callflag ) {		/* side effect of markcall */
		callflag = save;
		return(1);
	}
	else {
		callflag = save;
		return(0);
	}
}

push(p)
register NODE *p;
{
	register inc, real;

	if ( p->in.op != REG )
		cerror("cannot push\n");

	if (p->in.type == FLOAT || p->in.type == DOUBLE) {
		real = 1;
		inc = 8;
	} else {
		real = 0;
		inc = 4;
	}

	/* 
	 * if the result of this push is not the alignment we want,
	 * adjust the r31 before the push
	 */
	if ((( pushcnt + inc ) & 7) != (p->in.off & 7)) {
		inc += 4;
	}
	printf("\tsubu\tr31,r31,%d\n\tst%s\t%s,r31,0\n", inc, 
		real ? ".d" : "" , rnames[p->tn.rval]);
	pushcnt += inc;
}

extern int reg_use;
int regsvsz;
#ifndef FORT
extern int freg_use;
extern	ftlab1, ftlab2;

restregs(locals)
register locals;
{
	register framesz, u;
	register i, r, j, sp;

	u = usedregs;
	r = reg_use + 1;
	regsvsz = 0;
	for (i = r; i <= R30; i++) {
		if (u & (1 << i))
			regsvsz += 4;
	}
	if (!leaffunc)
		regsvsz += 4;	/* account for r1 */

	if (regsvsz&4)		/* align */
		regsvsz += 4;
	
	framesz = locals + regsvsz;

	if (framesz >= 0x10000) {
		imove(R13, locals);
		fputs("\taddu\tr31,r31,r13\n", stdout);
		j = 0;
	}
	else {
		j = locals;
	}
	sp = 31;
	for (i = r; i <= R30; i++) {
		if (u & (1 << i)) {
#if RWDBL
			if (((j&7) == 0) && (u&(1<<(i+1))) && i!=R30){
				printf("\tld.d\tr%d,r%d,%d\n", i, sp, j);
				j += 8;
				u &= ~(1 << (i+1));
				continue;
			}
#endif
			printf("\tld\tr%d,r%d,%d\n", i, sp, j);
			j += 4;
		}
	}
	if (!leaffunc)
		printf("\tld\tr1,r%d,%d\n", sp, j);
	if (framesz) {
		if (framesz >= 0x10000) {
			fputs ("	jmp.n	r1\n", stdout);
			printf("	addu	r31,r31,%d\n", regsvsz);
		}
		else {
			fputs ("	jmp.n	r1\n", stdout);
			printf("	addu	r31,r31,%d\n", framesz);
		}
	}
	else 
		cerror("framesz == 0");

}
#endif

#ifndef FORT
saveregs(locals)
register locals;
{
	register i, r, j;
	register framesz, u, sp;
	int tb1;

	r = reg_use + 1;	/* actual register number */
	u = usedregs;
	tb1 = 0;

#ifndef FORT
	if (isatty(fileno(stdout)))
		printf("@L%d:\n", ftlab1);
#endif
	

	framesz = locals + regsvsz;
	if (framesz) {
		if (framesz >= 0x10000) {
			printf("	subu	r12,r31,%d\n", regsvsz);
			imove(R13, framesz);
			fputs ("	subu	r31,r31,r13\n", stdout);
			sp = 12;
			j = 0;		/* starting offset from sp */
		}
		else {
			printf("	subu	r31,r31,%d\n", framesz);
			sp = 31;
			j = locals;	/* starting offset from sp */
		}
	}
	else {
		cerror("framesz == 0");
	}
	if (cpuflags & 1) {
		if (regsvsz)
			printf("\tst	r0,r%d,%d\n", sp, j);
		else
			fputs ("\tst	r0,r31,0\n", stdout);
		fputs("\ttb1\t0,r0,511\n", stdout);
	}
	for (i = r; i <= R30; i++) {
		if (u & (1 << i)) {
#if RWDBL
			if (((j&7) == 0) && (u&(1<<(i+1))) && i!=R30){
				printf("\tst.d\tr%d,r%d,%d\n", i, sp, j);
				j += 8;
				u &= ~(1 << (i+1));
				continue;
			}
#endif
			printf("\tst\tr%d,r%d,%d\n", i, sp, j);
			tb1 ++;
			if ((cpuflags & 1) && (tb1 & 1))
				fputs("\ttb1\t0,r0,511\n", stdout);
			j += 4;
		}
	}
	if (!leaffunc) {
		printf("\tst\tr1,r%d,%d\n", sp,  j);
		tb1 ++;
		if ((cpuflags & 1) && (tb1 & 1))
			fputs("\ttb1\t0,r0,511\n", stdout);
	}
	if ((cpuflags & 1) && (tb1 == 0))
		fputs("\ttb1\t0,r0,511\n", stdout);
	if (u & (1 << R30)) {
		if (sp == 31)
			printf("\taddu\tr30,r31,%d\n", locals + regsvsz);
		else
			fputs ("\taddu\tr30,r31,r13\n", stdout);
	}
#ifndef FORT
	if (isatty(fileno(stdout))) 
		printf( "	br 	@L%d\n", ftlab2);
#endif
}
#endif

static long entrybeg, entryend, endoff;
entrycode()
{
#ifndef FORT
	if (isatty(fileno(stdout))) {
 		printf( "	br 	@L%d\n", ftlab1);
 		printf( "@L%d:\n", ftlab2);
	}
	else {
		entrybeg = ftell(stdout);
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		fputs(";                                  \n", stdout);	
		entryend = ftell(stdout);
	}
#endif
}

funchead()
{
	if (isatty(fileno(stdout)))
		return;
	fflush(stdout);
	endoff = ftell(stdout);	
	if (fseek(stdout, entrybeg, 0))
		cerror("fseek on entry");
}

functail()
{
	long off;
	if (isatty(fileno(stdout)))
		return;
	fflush(stdout);
	off = ftell(stdout);
	if (off >= entryend)
		cerror("entry buffer overwrite");
	if (fseek(stdout, endoff, 0))
		cerror("fseek on end");
}
