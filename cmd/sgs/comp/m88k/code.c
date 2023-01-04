/*	@(#)code.c	1.9	*/
#include <stdio.h>
#include "mfile1"
#include "storclass.h"
#include <signal.h>
#include "messages.h"

/* static char ID[] = "@(#)code.c	1.9"; */
extern char uxrel[];

int proflg = 0;	/* are we generating profiling code? */
int gdebug = 0;	/* sdb flag	*/
int strftn = 0;	/* is the current function one which returns a value */
int fdefflag;	/* are we within a function definition ? */
char NULLNAME[8];
int labelno;
extern calltmp;

# define LINENO  lineno-startln+1	/* local line number, opening { = 1 */

int startln;		/* the editor line number of the opening '{' } */
int oldln;		/* remember the last line number printed */
char startfn[100] = "";	/* the file name of the opening '{' */

int bb_flags[BCSZ];	/* remember whether or not bb is needed */

int reg_use = MAXRVAR;
extern	char *rnames[];

/* tempfile is used to hold initializing strings that cannot be output
 * immediately -- they are read back in at the end and appended to
 * the intermediate text
 */
FILE *outfile = stdout;
FILE *tempfile;
FILE *datafile;
extern char *mktemp();

#ifndef R2
#define R2	2
#define R9	9
#endif

branch( n ){
	/* output a branch to label n
	 */
	printf( "	br 	@L%d\n", n );
	}

int lastloc = { -1 };

defalign(n) register n;{
	/* cause the alignment to become a multiple of n
	 */
	n /= SZCHAR;
	if( lastloc != PROG && n > 1 )
		fprintf( outfile, "	align	%d\n", (n + 1) & ~1 );
	}

static FILE jksave;
static int jkinit, havetext;
locctr( l ){
	/* output the location counter
	 */
	register temp;
	/* l is PROG, ADATA, DATA, STRNG, ISTRNG, or STAB */

	if( l == lastloc ) return(l);
	temp = lastloc;
	lastloc = l;
	switch( l ){

	case PROG:
		if (jkinit) {
			*datafile = *stdout;
			*stdout = jksave;
			jkinit = 0;
		}
		outfile = stdout;
		if( havetext == 0 )
			fputs( "\ttext\n", stdout );
		havetext = 1;
		break;

	case DATA:
	case ADATA:
		if ( jkinit == 0 ) {
			jksave = *stdout;
			*stdout = *datafile;
			jkinit = 1;
		}
		outfile = stdout;
		if( temp != DATA && temp != ADATA )
			fputs( "\tdata\n", stdout );
		break;

	case STRNG:
	case ISTRNG:
		/* output string initializers to a temporary file for now
		 * don't update lastloc
		 */
		if (jkinit) {
			*datafile = *stdout;
			*stdout = jksave;
			jkinit = 0;
		}
		outfile = tempfile;
		break;

	case STAB:
		cerror( "locctr: STAB unused" );
		break;

	default:
		cerror( "illegal location counter" );
		}

	return( temp );
	}

deflab( n ){
	/* output something to define the current position as label n
	 */
	fprintf( outfile, "@L%d:\n", n );
	set_r13(NULL);
	}

getlab(){
	/* return a number usable for a label
	 */
	static int crslab = 10;

	return( ++crslab );
	}

defnam( p ) register struct symtab *p; {
	/* define the current location as the name p->sname
	 * first give the debugging info for external definitions
	 */
	if( p->slevel == 0 )	/* make sure its external */
		prdef(p,0);
	if( p->sclass == EXTDEF )
#ifdef FLEXNAMES
		printf( "	global	%s\n", exname( p->sname ) );
#else
		printf( "	global	%.8s\n", exname( p->sname ) );
#endif
	if( p->sclass == STATIC && p->slevel > 1 )
		deflab( p->offset );
	else
#ifdef FLEXNAMES
		printf( "%s:\n", exname( p->sname ) );
#else
		printf( "%.8s:\n", exname( p->sname ) );
#endif

		}

commdec( id ){
	/* make a common declaration for id, if reasonable
	 */
	register struct symtab *q;
	int align;
	OFFSZ tsize();
	OFFSZ off;

	q = &stab[id];
	off = tsize( q->stype, q->dimoff, q->sizoff );
	off /= SZCHAR;
	if (q->sclass == EXTDEF) {
#ifdef FLEXNAMES
		werror("redeclaration of %s", q->sname);
#else
		werror("redeclaration of %.8s", q->sname);
#endif
		return;
	}
	if(q->slevel == 0)	/* make sure its external */
		prdef(q,0);
#ifdef FLEXNAMES
	printf( "	global	%s\n", exname(q->sname));
#else
	printf( "	global	%.8s\n", exname(q->sname));
#endif
	/** Allow multiple external definitions...
	*
	* printf( "	.bss	%.8s,", exname(q->sname));
	*/
#ifdef FLEXNAMES
	printf( "	comm	%s,", exname(q->sname));
#else
	printf( "	comm	%.8s,", exname(q->sname));
#endif
	printf( CONFMT, off);
	/** With multiple external definitions, this message is incorrect.
	*
	* if(off == 0L)
	*	uerror("declaration for %.8s allocates no space", exname(q->sname));
	*/
	/** No align for .comm
	*
	* align = talign(q->stype, q->sizoff)/SZCHAR;
	* printf( ",%d\n", align>2 ? 4 : align);
	*/
	putchar( '\n' );
	}

int ftlab1, ftlab2;
extern leaffunc;

bfcode( a, n ) int a[]; {
	/* code for the beginning of a function
	 * a is an array of indices in stab for the arguments
	 * n is the number of arguments
	 */
	register i, j;
	register temp;
	register struct symtab *p;
	int off;
	int real, strun, mask;
	char *toreg();

	/* function prolog */

	locctr( PROG );
	p = &stab[curftn];
	fputs( "\talign\t4\n", stdout );
	defnam( p );
	temp = p->stype;
	temp = DECREF(temp);
	strftn = (temp==STRTY) || (temp==UNIONTY);

	retlab = getlab();

	/* routine prolog */

	if ( gdebug )
		fputs( "	ln	1\n", stdout );
	ftlab1 = getlab();
	ftlab2 = getlab();
	entrycode();
	if( proflg ) {	/* profile code */
		i = getlab();
		fputs ("	subu	r31,r31,8\n", stdout);
		fputs ("	st	r1,r31,4\n", stdout);
		fputs ("	st	r2,r31,0\n", stdout);
		printf("	or.u	r2,r0,hi16(@L%d)\n", i);
		fputs ("	bsr.n	mcount\n", stdout);
		printf("	or	r2,r2,lo16(@L%d)\n", i);
		fputs ("	ld	r2,r31,0\n", stdout);
		fputs ("	ld	r1,r31,4\n", stdout);
		fputs ("	addu	r31,r31,8\n", stdout);
		/* fputs("\tdata\n", stdout); */
		fputs("\talign\t4\n", datafile);
		fprintf(datafile, "@L%d:	word	0\n", i);
		/* fputs("\t.text\n", stdout); */
		}

	if (strftn) {
		i = tsize(STRTY, p->dimoff, p->sizoff);
		j = talign(STRTY, p->sizoff);
		if ( i != SZLONG || j != ALLONG ) {
			calltmp += 4;
			printf("\tst	r12,r31,CTMP%d+4\n", ftnno);
		}
	}

	j = R2;
	resetreg();
	off = 0;		/* stack based arg offset */
	mask = 0;
	
	for( i=0; i<n; ++i ){
		p = &stab[a[i]];

		/* varargs support */
		if (p->stype == INT && p->sname[0] == 'v' &&
			p->sname[2] == '_' && j <= R9 &&
			strcmp(p->sname, "va_alist") == 0) {
				register k;
				for ( k = j; k <= R9; k++ )
					mask |= ( 1<< k);
			}

		real = p->stype == FLOAT || p->stype == DOUBLE;
		strun = p->stype == STRTY || p->stype == UNIONTY;
		if( strun ) {
		/* most structures come on the stack */
			p->offset = NOOFFSET;
			if( oalloc( p, &off ) ) cerror( "bad argument" );
			SETOFF( off, ALSTACK );
			USAGE(ARGREG);
			temp = off - p->offset + SZINT - 1;
			temp /= SZINT;
			if (temp == 1 && j <= R9) {
				mask |= (1 << j);
				/* printf("\tst	r%d,r30,%d\n",
					j, (p->offset/SZCHAR)& ~3);*/
				USAGE(j);
			}
			j += temp;
			}
		else if (temp = regarg(i, j, real)) {
			j = temp;
			if ( p->sclass == REGISTER ) {
				/* move register arg to real register */
				if (real) {
					rmove (p->offset, j, DOUBLE);
					SETOFF( off, ALDOUBLE);
				}
				else {
					rmove (p->offset, j, p->stype);
					if ( p->stype == CHAR ||
						p->stype == UCHAR ||
						p->stype == SHORT ||
						p->stype == USHORT )
						extend(p->offset, p->stype);
				}
				USAGE(p->offset);
				/* allocate stack space for registers also */
				temp = p->offset;
				p->sclass = PARAM;
				p->offset = NOOFFSET;
				if(oalloc(p, &off)) cerror("bad reg arg");
				p->offset = temp;
				p->sclass = REGISTER;
			}
			else {
				/* move register arg to stack */
				if (real) {
					SETOFF( off, ALDOUBLE);
				}
				p->offset = NOOFFSET;
				if(oalloc(p, &off)) cerror("bad argument");
				if (real) {
					printf("\tst.d	r%d,r30,%d\n",
						j, p->offset/SZCHAR);
				}
				else {
					if ( p->stype == CHAR ||
						p->stype == UCHAR ||
						p->stype == SHORT ||
						p->stype == USHORT )
						extend(j, p->stype);
					mask |= (1 << j);
					/* printf("\tst	r%d,r30,%d\n",
						j, (p->offset/SZCHAR)& ~3);*/
				}
				USAGE(ARGREG);
			}
			USAGE(j);
			j++;
			if (real) {
				USAGE(j);
				j++;
			}
		}
		else if( p->sclass == REGISTER ){
		/* argument came on the stack,  move to a register */
			if(real) {
				SETOFF(off, ALDOUBLE);
			}
			temp = p->offset;  /* save register number */
			p->sclass = PARAM;  /* forget that it is a register */
			p->offset = NOOFFSET;
			oalloc( p, &off );
			printf( "\t%s\t%s,r30,%d\n", toreg(p->stype), 
				rnames[temp], p->offset/SZCHAR);
			p->offset = temp;  /* remember register number */
			p->sclass = REGISTER;   /* remember that it is a register */
			USAGE(temp);
			USAGE(ARGREG);
			}
		else {
		/* use off here because we are dealing with stack args */
		/* argument came on the stack, leave it there */
			if(real) {
				SETOFF(off, ALDOUBLE);
			}
			p->offset = NOOFFSET;
			if( oalloc( p, &off ) ) cerror( "bad argument" );
			USAGE(ARGREG);
			}

		}
#if RWDBL
	/* minimize stores to memory with st.d */
	for (i = R2; i <= R9; i += 2) {
		if (mask & (1 << i)) {
			if (mask & ( 1 << (i + 1)))
				printf("\tst.d\tr%d,r30,%d\n", i, (i - R2)*4);
			else
				printf("\tst\tr%d,r30,%d\n", i, (i - R2)*4);
		}
		else if (mask & ( 1 << (i + 1)))
			printf("\tst\tr%d,r30,%d\n", i+1, 4+((i - R2)*4));
	}
#else
	/* st.d has chip problems */
	for (i = R2; i <= R9; i += 1)
		if (mask & (1 << i))
			printf("\tst\tr%d,r30,%d\n", i, (i - R2)*4);
#endif
	fdefflag = 1;

	/* initialize line number counters */

	oldln = startln = lineno;
	strcpy( startfn, ftitle );

	/* do .bf symbol and .defs for parameters
	 * paramters are delayed to here to two reasons:
	 *    1: they appear inside the .bf - .ef
	 *    2: the true declarations have been seen
	 */
	if ( gdebug ) {
		int line;

		line = lineno;	/* Line number information for sdb */
		printf( "	sdef	.bf\n	val	.\n	scl	%d\n	line	%d\n	endef\n", C_FCN ,line );
		for( i=0; i<n; ++i ) {
			p = &stab[a[i]];
			prdef(p,0);
	}
		}

	}

beg_file() {
	/* called as the very first thing by the parser to do machine
	 * dependent stuff
	 */
	extern fpe_catch();
	register char * p;
	register char * s;

			/* note: double quotes already in xtitle... */
	p = xtitle + strlen( xtitle ) - 2;
	s = p - 14;	/* max name length */
	while ( p > s && *p != '"' && *p != '/' )
		--p;
	printf( "\tfile\t\"%.15s\n", p + 1 );
	/*
	 * Catch floating exceptions generated by the constant
	 * folding code.
	 */
	(void) signal( SIGFPE, fpe_catch );
	}

fpe_catch() {
	/*
	 * Floating exception generated by constant folding.
	 */
	/* "floating point constant folding causes exception" */
	UERROR( MESSAGE( 125 ) );
	dexit();
	}

bccode() {
	/* called just before the first executable statement
	 * by now, the automatics and register variables are allocated
	 */

	SETOFF( autooff, SZINT );

	/* this is the block header:
	 * autooff is the max offset for auto and temps
	 * regvar is the least numbered register variable
	 */

# ifdef ONEPASS
	p2bbeg( autooff, regvar );
# else
	printf( "%c%d\t%d\t%d\t\n", BBEG, ftnno, autooff, regvar );
# endif
	reg_use = (reg_use > regvar ? regvar : reg_use);

	}

ejobcode( flag ){
	/* called just before final exit
	 * flag is 1 if errors, 0 if none
	 */
	}

aobeg(){
	/* called before removing automatics from stab
	 */
	}

aocode(p) struct symtab *p; {
	/* called when automatic p removed from stab
	 */
	}

aoend(){
	/* called after removing all automatics from stab
	 * print eb here if bb printed for this block
	 */
	if( gdebug && bb_flags[blevel+1] ) {
		printf( "	sdef	.eb\n	val	.\n	scl	%d\n	line	%d\n	endef\n", C_BLOCK, LINENO );
		bb_flags[blevel+1] = 0;
	}
	}


efcode(){
	/* code for the end of a function
	 */

	deflab( retlab );

	if( strftn ){  /* copy output (in R2) to caller */
		register NODE *l, *r;
		register struct symtab *p;
		register TWORD t;
		register int j;
		int i;

		p = &stab[curftn];
		t = p->stype;
		t = DECREF(t);

#if 1
		i = tsize(STRTY, p->dimoff, p->sizoff);
		j = talign(STRTY, p->sizoff);
		printf(";\tsize = %d align = %d\n", i / SZCHAR, j / SZCHAR);
		if ( i == SZLONG && j == ALLONG ) {
			printf("\tld	r2,r2,0\n");
			USAGE(R2);
			reached = 1;
			strftn = 0;
			goto strout;
		}
		printf("\tld	r5,r31,CTMP%d+4\n", ftnno);
#else
		i = getlab();	/* label for return area */
		/* fputs("\t.data\n", stdout); */
		fputs("\talign\t8\n", datafile);
		fprintf(datafile, "@L%d:	zero	%d\n", 
			i, tsize(t, p->dimoff, p->sizoff)/SZCHAR );
		/* fputs("\t.text\n", stdout); */
		printf("	or.u	r5,r0,hi16(@L%d)\n", i);
		printf("	or	r5,r5,lo16(@L%d)\n", i);
#endif
		USAGE(5);
		USAGE(R2);

		reached = 1;
		l = block( REG, NIL, NIL, PTR|t, p->dimoff, p->sizoff );
		l->tn.rval = 5;  /* R5 */
		l->tn.lval = 0;  /* no offset */
		r = block( REG, NIL, NIL, PTR|t, p->dimoff, p->sizoff );
		r->tn.rval = 2;  /* return reg is R2 */
		r->tn.lval = 0;
		l = buildtree( UNARY MUL, l, NIL );
		r = buildtree( UNARY MUL, r, NIL );
		l = buildtree( ASSIGN, l, r );
		l->in.op = FREE;
		ecomp( l->in.left );
		/*
		printf("	or.u	r2,r0,hi16(@L%d)\n", i);
		printf("	or	r2,r2,lo16(@L%d)\n", i);
		*/
		/* turn off strftn flag, so return sequence will be generated */
		strftn = 0;
		}
strout:

	/* print end-of-function pseudo and its line number */

	if ( gdebug ){
		printf( "	sdef	.ef\n	val	.\n	scl	%d\n	line	%d\n	endef\n", C_FCN, LINENO );
		if( LINENO > 1 )
			printf( "	ln	%d\n", LINENO );
		}
	p2bend();
	reg_use = MAXRVAR;
	fdefflag = 0;
	}

	/* print end-of-function pseudo and its line number */

bycode( t, i ){
	/* put byte i+1 in a string */

	i &= 07;
	if( t < 0 ){ /* end of the string */
		if( i != 0 ) putc( '\n', outfile );
		}

	else { /* stash byte t into string */
		if( i == 0 ) fputs( "\tbyte\t", outfile );
		else putc( ',', outfile );
		fprintf( outfile, "0x%x", t );
		if( i == 07 ) putc( '\n', outfile );
		}
	}

zecode( n ){
	/* n integer words of zeros */
	OFFSZ temp;
	if( n <= 0 ) return;
	printf( "	zero	%d\n", (SZINT/SZCHAR)*n );
	temp = n;
	inoff += temp*SZINT;
	}

fldal( t ) unsigned t; { /* return the alignment of field of type t */
	uerror( "illegal field type" );
	return( ALINT );
	}

fldty( p ) struct symtab *p; { /* fix up type of field p */
	;
	}

where(c){ /* print location of error  */
	/* c is either 'u', 'c', or 'w' */
	/* GCOS version */
	fprintf( stderr, "%s, line %d: ", ftitle, lineno );
	}


/* toreg() returns a pointer to a char string
		  which is the correct  "register move" for the passed type 
 */
struct type_move {TWORD fromtype; char tostrng[8];} toreg_strs[] =
	{
	CHAR, "ld.b",
	SHORT, "ld.h",
	FLOAT, "ld.d",
	DOUBLE, "ld.d",
	UCHAR,	"ld.bu",
	USHORT,	"ld.hu",
	0, "ld"
	};

char *
toreg(type) register TWORD type; {
	register struct type_move *p;

	for ( p=toreg_strs; p->fromtype > 0; p++)
		if (p->fromtype == type) break;

	return(p->tostrng);
}

struct sw heapsw[SWITSZ];	/* heap for switches */

genswitch( p, n ) register struct sw *p; {
	/*	p points to an array of structures, each consisting
	 * of a constant value and a label.
	 * The first is >=0 if there is a default label;
	 * its value is the label number
	 * The entries p[1] to p[n] are the nontrivial cases
		*/
	register i;
	register CONSZ j, range;
	register dlab, swlab;

	range = p[n].sval-p[1].sval;

	if( range>0 && range <= 3*n && n>=4 ){ /* implement a direct switch */

		swlab = getlab();
		dlab = p->slab >= 0 ? p->slab : getlab();

		if ( p[1].sval ) {
			if (p[1].sval & 0xffff0000) {
				imove( 3, p[1].sval);
				fputs("	subu	r2,r2,r3\n", stdout);
			} else {
				printf("	subu	r2,r2,");
				printf(CONFMT, p[1].sval);
				putchar('\n');
			}
		}
		/* already in r2 */
		gencmp(range);
		printf("	bb1	hi,r13,@L%d\n", dlab);
		printf("	or.u	r3,r0,hi16(@L%d)\n", swlab);
		printf("	or	r3,r3,lo16(@L%d)\n", swlab);
		printf("	lda	r2,r3[r2]\n");
		printf("	jmp	r2\n");
		USAGE(R2);
		USAGE(3);

		printf("@L%d:\n", swlab);
		for( i=1,j=p[1].sval; i<=n; j++) {
			printf("	br	@L%d\n", (j == p[i].sval ? ((j=p[i++].sval), p[i-1].slab) : dlab),
				swlab);
			}

		if( p->slab < 0 )
			printf("@L%d:\n", dlab);
		return;

		}

	if( n>8 ) {	/* heap switch */

		heapsw[0].slab = dlab = p->slab >= 0 ? p->slab : getlab();
		makeheap(p, n, 1);	/* build heap */

		walkheap(1, n);	/* produce code */

		if( p->slab >= 0 )
			branch( dlab );
		else
			printf("@L%d:\n", dlab);
		return;
	}

	/* debugging code */

	/* out for the moment
	if( n >= 4 ) werror( "inefficient switch: %d, %d", n, (int) (range/n) );
	*/

	/* simple switch code */

	for( i=1; i<=n; ++i ){
		/* already in r2 */
		gencmp(p[i].sval);
		printf( "\n	bb1	eq,r13,@L%d\n", p[i].slab );
		}

	USAGE(R2);

	if( p->slab>=0 ) branch( p->slab );
	}

makeheap(p, m, n)
register struct sw *p;
{
	register int q;

	q = select(m);
	heapsw[n] = p[q];
	if( q>1 ) makeheap(p, q-1, 2*n);
	if( q<m ) makeheap(p+q, m-q, 2*n+1);
}

select(m) {
	register int l,i,k;

	for(i=1; ; i*=2)
		if( (i-1) > m ) break;
	l = ((k = i/2 - 1) + 1)/2;
	return( l + (m-k < l ? m-k : l));
}

walkheap(start, limit)
{
	int label;


	if( start > limit ) return;
	gencmp(heapsw[start].sval);
	printf("	bb1	eq,r13,@L%d\n", heapsw[start].slab);
	if( (2*start) > limit ) {
		printf("	br 	@L%d\n", heapsw[0].slab);
		return;
	}
	if( (2*start+1) <= limit ) {
		label = getlab();
		printf("	bb1	gt,r13,@L%d\n", label);
	} else
		printf("	bb1	gt,r13,@L%d\n", heapsw[0].slab);
	walkheap( 2*start, limit);
	if( (2*start+1) <= limit ) {
		printf("@L%d:\n", label);
		walkheap( 2*start+1, limit);
	}
}

int wloop_level = LL_BOT;	/* "while" loop test at loop bottom */
int floop_level = LL_DUP;	/* duplicate "for" loop test at top & bottom */

char *tmpname = "/tmp/pcXXXXXX";
char *dataname = "/tmp/pcdXXXXXX";

main( argc, argv ) int argc; register char *argv[]; {

	register int i;
	register int c;
	register int fdef = 0;
	register char *cp;
	register int r;
	int dexit();

	for( i=1; i<argc; ++i ){
		if( *(cp=argv[i]) == '-' && *++cp == 'X' ){
			while( *++cp ){ /* set flags and other local options */
				switch( *cp ){

				case 'r':	/* print UNIX release no. */
					fprintf( stderr, "Version: %s\n",
						uxrel );
					break;

				case 'P':	/* profiling */
					++proflg;
					break;

				case 'g':	/* SDB option */
					++gdebug;
					/*
					* Cause simple-minded loop code
					* generation.  No surprises.
					*/
					floop_level = wloop_level = LL_TOP;
					break;
					}
				}
			}
		if( *(cp=argv[i]) == '-') {
			cp++;
			switch(*cp++) {
				case 'i':
				case 'I':
					i++;
					if ( i>= argc )
						break;
					if ( *cp != '\0' )
						break;
					if(freopen(argv[i], "r", stdin)
							== NULL) {
						goto badf;
					}
					break;

				case 'o':
				case 'O':
					i++;
					if ( i>= argc )
						break;
					if ( *cp != '\0' )
						break;
					if(freopen(argv[i], "w", stdout)
							== NULL) {
						badf:
						fprintf(stderr,
							"can't open %s\n", 
							argv[i]);
						exit(1);
					}
					break;
				case 'f':
				case 'F':
					i++;
					if ( i>= argc )
						break;
					if ( *cp != '\0' )
						break;
					break;

				case 'd':
					if ( *cp == 'l' || *cp == 's' )
						argv[i] = "";
					break;
				}
				continue;
		}
		if( *(argv[i]) != '-' ) { /* open input and output files */
			if( fdef < 2 ){
#ifdef FORT
				if( freopen(argv[i], !fdef ?"r":"a", !fdef ?stdin:stdout) == NULL)
#else
				if( freopen(argv[i], !fdef ?"r":"w", !fdef ?stdin:stdout) == NULL)
#endif
				{
					fprintf(stderr, "ccom:can't open %s\n", argv[i]);
					exit(1);
				}
				++fdef;
				}
			}
		} /* end of for loop */

	mktemp(tmpname);
	mktemp(dataname);
	if(signal( SIGHUP, SIG_IGN) != SIG_IGN) signal(SIGHUP, dexit);
	if(signal( SIGINT, SIG_IGN) != SIG_IGN) signal(SIGINT, dexit);
	if(signal( SIGTERM, SIG_IGN) != SIG_IGN) signal(SIGTERM, dexit);
	tempfile = fopen( tmpname, "w" );
	datafile = fopen( dataname, "w" );

	r = mainp1( argc, argv );
	locctr(PROG);
	fputs("\tdata\n", stdout);

	if ( datafile && ferror( datafile ) )
		cerror( "Problems writing; probably out of temp-file space" );
	datafile = freopen( dataname, "r", datafile );
	if( datafile != NULL )
		while((c=getc(datafile)) != EOF )
			putchar(c);
	else cerror( "Lost temp file" );
	if (datafile)
		fclose(datafile);
	unlink(dataname);

	if ( tempfile && ferror( tempfile ) )
		cerror( "Problems writing; probably out of temp-file space" );
	tempfile = freopen( tmpname, "r", tempfile );
	if( tempfile != NULL )
		while((c=getc(tempfile)) != EOF )
			putchar(c);
	else cerror( "Lost temp file" );
	if (tempfile)
		fclose(tempfile);
	unlink(tmpname);

	if ( ferror( stdout ) )
		cerror( "Problems with output file; probably out of temp space" );
	exit( r );

	}

dexit( v ) {
	fclose(tempfile);
	fclose(datafile);
	unlink(tmpname);
	unlink(dataname);
	exit(1);
	}
