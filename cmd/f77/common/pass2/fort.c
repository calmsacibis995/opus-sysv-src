/*	@(#)fort.c	1.2	*/

/* this forces larger trees, etc. */

# include "mfile2"
# include "fort.h"

/*	masks for unpacking longs */

# ifndef FOP
# define FOP(x) (int)((x)&0377)
# endif

# ifndef VAL
# define VAL(x) (int)(((x)>>8)&0377)
# endif

# ifndef REST
# define REST(x) (((x)>>16)&0177777)
# endif

short debugflag = 0;

FILE * lrd;  /* for default reading routines */
# ifndef NOLREAD
long lread(){
	static long x;
	if( fread( (char *) &x, 4, 1, lrd ) <= 0 ) cerror( "intermediate file read error" );
	return( x );
	}
# endif

# ifndef NOLOPEN
lopen( s ) char *s; {
	/* if null, opens the standard input */
	if( *s ){
		lrd = fopen( s, "r" );
		if( lrd == NULL ) cerror( "cannot open intermediate file %s", s );
		}
	else  lrd = stdin;
	}
# endif

# ifndef NOLCREAD
lcread( cp, n ) char *cp; {
	if( n > 0 ){
		if( fread( cp, 4, n, lrd ) != n ) cerror( "intermediate file read error" );
		}
	}
# endif
#ifdef FLEXNAMES
#ifndef TSTRSZ 
#define TSTRSZ 2048
#endif
char *
readname( )  { /* read in a string to the name field */
	char *cp;
	char temp[TSTRSZ];
	register ct = 0, ch;
	register char *tptr = temp;

	do  {
		*tptr++ = ch = getc(lrd);
		ct++;
	} while (ch > 0);
	if (ch == -1) tptr[-1] = '\0';
	if (ct == 1) {
		/* This is a special case with a flag in in.name[2] */
		/* fake out tstr */
		cp = tstr( "123" );
		cp[0] = '\0';
		cp[2] = '\0';
	} else {
		cp = tstr( temp );
	}
	/* round out to a multiple of 4 */
	if (ct&3) fread(temp, 1, 4-(ct&3), lrd);
	return cp;
}
#endif

# ifndef NOLCCOPY
lccopy( n ) register n; {
	register i;
	static char fbuf[128];
	if( n > 0 ){
		if( n > 32 ) cerror( "lccopy asked to copy too much" );
		if( fread( fbuf, 4, n, lrd ) != n ) cerror( "intermediate file read error" );
		for( i=4*n; fbuf[i-1] == '\0' && i>0; --i ) { /* VOID */ }
		if( i ) {
			if( fwrite( fbuf, 1, i, stdout ) != i ) cerror( "output file error" );
			}
		}
	}
# endif

/*	new opcode definitions */

# define FORTOPS 200
# define FTEXT 200
# define FEXPR 201
# define FSWITCH 202
# define FLBRAC 203
# define FRBRAC 204
# define FEOF 205
# define FARIF 206
# define LABEL 207

/*	stack for reading nodes in postfix form */

# define NSTACKSZ 250

NODE * fstack[NSTACKSZ];
NODE ** fsp;  /* points to next free position on the stack */

unsigned int caloff();
extern unsigned int offsz;
mainp2( argc, argv ) char *argv[]; {
	int files;
	register long x;
	register NODE *p, *q;

	offsz = caloff();
	files = p2init( argc, argv );
	tinit();

		
	if( files ){
		while( files < argc && argv[files][0] == '-' ) {
			++files;
			}
		if( files > argc ) return( nerrors );
		lopen( argv[files] );
		}
	else lopen( "" );

	fsp = fstack;

	for(;;){
		/* read nodes, and go to work... */
		x = lread();

	if( xdebug ) fprintf( stderr, "op=%d, val = %d, rest = 0%o\n", FOP(x), VAL(x), (int)REST(x) );
		switch( (int)FOP(x) ){  /* switch on opcode */

		case 0:
			fprintf( stderr, "null opcode ignored\n" );
			continue;
		case FTEXT:
			lccopy( VAL(x) );
			printf( "\n" );
			continue;

		case FLBRAC:
			tmpoff = baseoff = lread();
			maxtreg = VAL(x);
			if( ftnno != REST(x) ){
				/* beginning of function */
				maxoff = baseoff;
				ftnno = REST(x);
				maxtemp = 0;
				}
			else {
				if( baseoff > maxoff ) maxoff = baseoff;
				/* maxoff at end of ftn is max of autos and temps 
				   over all blocks in the function */
				}
			setregs();
			continue;

		case FRBRAC:
			SETOFF( maxoff, ALSTACK );
			eobl2();
			continue;

		case FEOF:
			return( nerrors );

		case FSWITCH:
			uerror( "switch not yet done" );
			for( x=VAL(x); x>0; --x ) lread();
			continue;

		case ICON:
			p = talloc();
			p->in.op = ICON;
			p->in.type = REST(x);
			p->tn.rval = 0;
			p->tn.lval = lread();
			if( VAL(x) ){
#ifdef FLEXNAMES
				p->in.name = readname();
#else
				lcread( p->in.name, 2 );
#endif
#ifdef OPUS
#if ns32000
					q = p;
					q->in.op = NAME;
					p = talloc();
					p->in.op = UNARY AND;
					p->in.left = q;
					p->in.type = q->in.type;
					q->in.type = DECREF(q->in.type);	
					q->in.su = 0;
					q->in.rall = NOPREF;
#endif
#endif
				}
#ifdef FLEXNAMES
			else p->in.name = "";
#else
			else p->in.name[0] = '\0';
#endif

		bump:
			p->in.su = 0;
			p->in.rall = NOPREF;
			*fsp++ = p;
			if( fsp >= &fstack[NSTACKSZ] ) uerror( "expression depth exceeded" );
			continue;

		case NAME:
			p = talloc();
			p->in.op = NAME;
			p->in.type = REST(x);
			p->tn.rval = 0;
			if( VAL(x) ) p->tn.lval = lread();
			else p->tn.lval = 0;
#ifdef FLEXNAMES
			p->in.name = readname( );
#else
			lcread( p->in.name, 2 );
#endif
			goto bump;

		case OREG:
			p = talloc();
			p->in.op = OREG;
			p->in.type = REST(x);
			p->tn.rval = VAL(x);
			p->tn.lval = lread();
#ifdef FLEXNAMES
			p->in.name = readname( );
#else
			lcread( p->in.name, 2 );
#endif
			goto bump;

		case REG:
			p = talloc();
			p->in.op = REG;
			p->in.type = REST(x);
			p->tn.rval = VAL(x);
			rbusy( p->tn.rval, p->in.type );
			p->tn.lval = 0;
#ifdef FLEXNAMES
			p->in.name = "";
#else
			p->in.name[0] = '\0';
#endif
			goto bump;

		case FEXPR:
			lineno = REST(x);
			if( VAL(x) ) lcread( filename, VAL(x) );
			if( fsp == fstack ) continue;  /* filename only */
			if( --fsp != fstack ) uerror( "expression poorly formed" );
			if( lflag ) lineid( lineno, filename );
			tmpoff = baseoff;
			p = fstack[0];
			if( edebug ) fwalk( p, eprint, 0 );
# ifdef MYREADER
			MYREADER(p);
# endif

			nrecur = 0;
			delay( p );
			reclaim( p, RNULL, 0 );

			allchk();
			tcheck();
			continue;

		case LABEL:
			if( VAL(x) ){
				tlabel();
				}
			else {
				label( (int) REST(x) );
				}
			continue;

		case GOTO:
			if( VAL(x) ) {
				cbgen( 0, (int) REST(x), 'I' );  /* unconditional branch */
				continue;
				}
			/* otherwise, treat as unary */
			goto def;

		default:
		def:
			p = talloc();
			p->in.op = FOP(x);
			p->in.type = REST(x);

			switch( optype( p->in.op ) ){

			case BITYPE:
				p->in.right = *--fsp;
				p->in.left = *--fsp;
				goto bump;

			case UTYPE:
				p->in.left = *--fsp;
				p->tn.rval = 0;
				goto bump;

			case LTYPE:
				uerror( "illegal leaf node: %d", p->in.op );
				exit( 1 );
				}
			}
		}
	}

#if OPUS
/* from optim.c */
short revrel[] = { EQ, NE, GE, GT, LE, LT, UGE, UGT, ULE, ULT };

nncon( p ) register NODE *p; {
	/* is p a constant without a name */
	return( p->in.op == ICON && p->in.name[0] == '\0' );
	}

#if clipper
int ftlab1, ftlab2, reg_use, freg_use;
#endif

#endif
