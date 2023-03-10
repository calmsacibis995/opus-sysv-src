/*	@(#)order.c	1.4	*/
# include "mfile2"

int maxargs = { -1 };

stoasg( p, o ) register NODE *p; {
	/* should the assignment op p be stored,
	   given that it lies as the right operand of o
	   (or the left, if o==UNARY MUL) */
/*
	if( p->in.op == INCR || p->in.op == DECR ) return;
	if( o==UNARY MUL && p->in.left->in.op == REG && !isbreg(p->in.left->tn.rval) ) SETSTO(p,INAREG);
 */
	}

deltest( p ) register NODE *p; {
	/* should we delay the INCR or DECR operation p */

	p = p->in.left;
/* the OREG case is wrong try 'r += (*r)--;'
	OREGs of type pointer are ok but why check for that case alone
	return( p->in.op == REG || p->in.op == NAME || p->in.op == OREG );
*/
	return( p->in.op == REG || p->in.op == NAME );
	}

mkadrs(p) register NODE *p; {
	register o;

	o = p->in.op;

	if( asgop(o) ){
		if( p->in.left->in.su >= p->in.right->in.su ){
			if( p->in.left->in.op == UNARY MUL ){
				SETSTO( p->in.left->in.left, INTEMP );
				}
			else if( p->in.left->in.op == FLD && p->in.left->in.left->in.op == UNARY MUL ){
				SETSTO( p->in.left->in.left->in.left, INTEMP );
				}
			else { /* should be only structure assignment */
				SETSTO( p->in.left, INTEMP );
				}
			}
		else SETSTO( p->in.right, INTEMP );
		}
	else {
		if( p->in.left->in.su > p->in.right->in.su ){
			SETSTO( p->in.left, INTEMP );
			}
		else {
			SETSTO( p->in.right, INTEMP );
			}
		}
	}

notoff( t, r, off, cp) CONSZ off; char *cp; {
	/* is it legal to make an OREG or NAME entry which has an
	/* offset of off, (from a register of r), if the
	/* resulting thing had type t */

/*	if( r == R0 ) return( 1 );  /* NO */
/*	if (*cp || off < 0 || off > 0xffff) return( 1 ); /* NO */ 
	if (*cp) return( 1 ); /* NO */ 
	return(0);  /* YES */
	}

# define max(x,y) ((x)<(y)?(y):(x))

sucomp( p ) register NODE *p; {

	/* set the su field in the node to the sethi-ullman
	   number, or local equivalent */

	register o, ty, sul, sur, r;

	o = p->in.op;
	ty = optype( o );
	p->in.su = 1;			/* 1 always */

	if( ty == LTYPE ){
		if( o == OREG ){
			r = p->tn.rval;
			/* oreg cost is (worst case) 1 + number of temp registers used */
			if( R2TEST(r) ){
				if( R2UPK1(r)!=R2NOBASE && istreg(R2UPK1(r)) ) ++p->in.su;
				if( istreg(R2UPK2(r)) ) ++p->in.su;
				}
			else {
				if( istreg( r ) ) ++p->in.su;
				}
			}
		if( p->in.su == 1 &&
		   (p->in.op!=REG || !istreg(p->tn.rval)) &&
#ifdef FORT
		   (p->in.type==INT || p->in.type==UNSIGNED) )
#else
		   (p->in.type==INT || p->in.type==UNSIGNED || p->in.type==DOUBLE) )
#endif
			p->in.su = 0;
		return;
		}

	else if( ty == UTYPE ){
		switch( o ) {
		case UNARY CALL:
		case UNARY STCALL:
			p->in.su = fregs;  /* all regs needed */
			return;

		/*********
		case UNARY MUL:
			p->in.su = p->in.left->in.su +
				( szty( p->in.type ) > 1 ? 1 : 0 );
			return;
		*********/

		default:
			p->in.su =  p->in.left->in.su + (szty( p->in.type ) > 1 ? 2 : 0) ;
			return;
			}
		}


	/* If rhs needs n, lhs needs m, regular su computation */

	sul = p->in.left->in.su;
	sur = p->in.right->in.su;

	if( o == ASSIGN ){
		/* computed by doing right, then left (if not in mem), then doing it */
		/* p->in.su = max(sur,sul+1); */
		p->in.su = max( sur, sul + szty( p->in.right->in.type ) );
		return;
		}

	if( o == CALL || o == STCALL ){
		/* in effect, takes all free registers */
		p->in.su = fregs;
		return;
		}

	if( o == STASG ){
		/* right, then left */
		p->in.su = max( max( 1+sul, sur), fregs );
		return;
		}

	/*********
	if ( o == INCR || o == DECR )
	{
		p->in.su = sul + 1;
		return;
	}
	*********/

	if( asgop(o) ){
		/* computed by doing right, doing left address, doing left, op, and store */
		/* p->in.su = max(sur,sul+2); */
		p->in.su = max( sur, sul + 1 + szty( p->in.right->in.type ) );
/* don't ever need both the address and the value...
		p->in.su = max( sur, sul + szty( p->in.right->in.type ) );
*/
/*
		if( o==ASG MUL || o==ASG DIV || o==ASG MOD) p->in.su = max(p->in.su,fregs);
 */
		return;
		}

	switch( o ){
	case ANDAND:
	case OROR:
	case QUEST:
	case COLON:
	case COMOP:
		p->in.su = max( max(sul,sur), 1);
		return;

	case AND:
	case MUL:
	case PLUS:
	case OR:
	case ER:
		/* commutative ops; put harder on left */
		if( p->in.right->in.su > p->in.left->in.su && !istnode(p->in.left) ){
			register NODE *temp;
			temp = p->in.left;
			p->in.left = p->in.right;
			p->in.right = temp;
			}
		break;
		}

	/* binary op, computed by left, then right, then do op */
	p->in.su = max(sul,szty(p->in.right->in.type)+sur);
/*
	if( o==MUL||o==DIV||o==MOD) p->in.su = max(p->in.su,fregs);
 */

	}

int radebug = 0;

rallo( p, down ) NODE *p; {
	/* do register allocation */
	register o, type, down1, down2, ty;

	if( radebug ) printf( "rallo( %o, %d )\n", p, down );

	down2 = NOPREF;
	p->in.rall = down;
	down1 = ( down &= ~MUSTDO );

	ty = optype( o = p->in.op );
	type = p->in.type;


/*this is not needed on the VAX
	if( type == DOUBLE || type == FLOAT ){
		if( o == FORCE ) down1 = F0|MUSTDO;
		}
	else
*/
	switch( o ) {
	case ASSIGN:	
		down1 = NOPREF;
		down2 = down;
		break;

/*
	case MUL:
	case DIV:
	case MOD:
		down1 = R3|MUSTDO;
		down2 = R5|MUSTDO;
		break;

	case ASG MUL:
	case ASG DIV:
	case ASG MOD:
		p->in.left->in.rall = down1 = R3|MUSTDO;
		if( p->in.left->in.op == UNARY MUL ){
			rallo( p->in.left->in.left, R4|MUSTDO );
			}
		else if( p->in.left->in.op == FLD  && p->in.left->in.left->in.op == UNARY MUL ){
			rallo( p->in.left->in.left->in.left, R4|MUSTDO );
			}
		else rallo( p->in.left, R3|MUSTDO );
		rallo( p->in.right, R5|MUSTDO );
		return;
 */

	case CALL:
	case STASG:
	case EQ:
	case NE:
	case GT:
	case GE:
	case LT:
	case LE:
	case NOT:
	case ANDAND:
	case OROR:
		down1 = NOPREF;
		break;

	case FORCE:	
		down1 = R2|MUSTDO;
		if( (type == DOUBLE || type == FLOAT) && p->in.left->in.op == ICON)
			p->in.left->in.type = type; /* it may be an ICON 0 */
		break;

		}

	if( ty != LTYPE ) rallo( p->in.left, down1 );
	if( ty == BITYPE ) rallo( p->in.right, down2 );

	}

offstar( p, cook ) register NODE *p; {
	if( p->in.op == PLUS ){
		if(p->in.right->in.op==ICON && p->in.right->in.name[0]=='\0'){
			p = p->in.left;
			order( p , INTAREG|INAREG);
			return;
			}
		}

	if( p->in.op == UNARY MUL ) {
		offstar( p->in.left, cook );
		return;
	}

	order( p, INTAREG|INAREG );
	}

setincr( p, cook ) register NODE *p; {
	p = p->in.left;
	if( p->in.op == UNARY MUL ){
		offstar( p, cook );
		return( 1 );
		}
	return( 0 );
	}

setbin( p, cook ) register NODE *p; {
	register ro, rt;

	rt = p->in.right->in.type;
	ro = p->in.right->in.op;

	if( canaddr( p->in.left ) && !canaddr( p->in.right ) ) { /* address rhs */
		if( ro == UNARY MUL ) {
			offstar( p->in.right->in.left, cook );
			return(1);
		} else {
			order( p->in.right, INAREG|INTAREG|SOREG|INTBREG|INBREG );
			return(1);
		}
	}
	if( !istnode( p->in.left) ) { /* try putting LHS into a reg */
/*		order( p->in.left, logop(p->in.op)?(INAREG|INBREG|INTAREG|INTBREG|SOREG):(INTAREG|INTBREG|SOREG) );*/

		if( p->in.left->in.op == UNARY MUL ) {
			offstar( p->in.left, cook );
			return(1);
			}
		else {
			order( p->in.left, INAREG|INTAREG|INBREG|INTBREG|SOREG );
			return(1);
			}
		}
	else if( ro == UNARY MUL && rt != CHAR && rt != UCHAR ){
		offstar( p->in.right->in.left, cook );
		return(1);
		}
	else if( rt == CHAR || rt == UCHAR || rt == SHORT || rt == USHORT || (ro != REG &&
			ro != NAME && ro != OREG && ro != ICON ) ){
		order( p->in.right, INAREG|INBREG );
		return(1);
		}
/*
	else if( logop(p->in.op) && rt==USHORT ){  /* must get rhs into register */
/*
		order( p->in.right, INAREG );
		return( 1 );
		}
 */
	return(0);
	}

setstr( p, cook ) register NODE *p; { /* structure assignment */
	if( p->in.right->in.op != REG ){
		order( p->in.right, INTAREG );
		return(1);
		}
	p = p->in.left;
	if( p->in.op != NAME && p->in.op != OREG ){
		if( p->in.op != UNARY MUL ) cerror( "bad setstr" );
		order( p->in.left, INTAREG );
		return( 1 );
		}
	return( 0 );
	}

setasg( p, cook ) register NODE *p; {
	/* setup for assignment operator */

	register NODE *r;

	r = p->in.right;
	if ( r->in.op != REG && r->in.op != ICON ) {
		if( r->in.op == UNARY MUL )
			offstar(r->in.left, cook);
		else
			order( r, INAREG|INBREG );
		return(1);
		}
	if( p->in.left->in.op == UNARY MUL ) {
		offstar( p->in.left->in.left, cook );
		return(1);
		}
	if( p->in.left->in.op == FLD && p->in.left->in.left->in.op == UNARY MUL ){
		offstar( p->in.left->in.left->in.left, cook );
		return(1);
		}
/* FLD patch */
	if( p->in.left->in.op == FLD && !(p->in.right->in.type==INT || p->in.right->in.type==UNSIGNED)) {
		order( p->in.right, INAREG);
		return(1);
		}
/* end of FLD patch */
	return(0);
	}

setasop( p, cook ) register NODE *p; {
	/* setup for =ops */
	register rt, ro;
	register lval;

	rt = p->in.right->in.type;
	ro = p->in.right->in.op;

	if( ro == UNARY MUL ){
		offstar( p->in.right->in.left, cook );
		return(1);
		}
	if( ( rt == CHAR || rt == SHORT || rt == UCHAR || rt == USHORT ||
			( ro != REG && ro != ICON ) ) ){
		order( p->in.right, INAREG|INBREG );
		return(1);
		}
	if((p->in.op == ASG MUL || p->in.op == ASG DIV || p->in.op == ASG MOD) 
			&& ro != REG ){
		order( p->in.right, INAREG|INBREG );
		return(1);
		}
	if(ro == ICON) {
		lval = p->in.right->tn.lval;
		if(p->in.right->in.name[0] || lval<0 || lval>0xffff){
			order( p->in.right, INAREG|INBREG );
			return(1);
			}
		}


	p = p->in.left;
	if( p->in.op == FLD ) p = p->in.left;

	switch( p->in.op ){

	case REG:
	case ICON:
	case NAME:
	case OREG:
		return(0);

	case UNARY MUL:
		offstar( p->in.left, cook );
		return(1);

		}
	cerror( "illegal setasop" );
	}

int crslab = 99999;  /* Honeywell */

getlab(){
	return( crslab-- );
	}

deflab( l ){
	printf( "@L%d:\n", l );
	set_r13(NULL);
	}

genargs( p, ptemp ) register NODE *p, *ptemp; {
	register NODE *pasg;
	register align;
	register size;
	register TWORD type;
	int off;
	int dst;
	int arg;
	struct alink *al;
	extern pushcnt;
	extern changespoff();
	extern nor2reg;

	/* generate code for the arguments */

	/*  first, do the arguments on the right */
	while( p->in.op == CM ){
		genargs( p->in.right, ptemp );
		p->in.op = FREE;
		p = p->in.left;
		}

	if( p->in.op == STARG ){ /* structure valued argument */

		size = p->stn.stsize;
		align = p->stn.stalign;
		off = p->stn.off;
		dst = p->stn.dst;
		arg = p->stn.arg;
		al = p->stn.alink;

		if( p->in.left->in.op == ICON ){
			p->in.op = FREE;
			p= p->in.left;
			}
		else {
			/* make it look beautiful... */
			nor2reg = 1;
			p->in.op = UNARY MUL;
			canon( p );  /* turn it into an oreg */
			if( p->in.op != OREG ){
				offstar( p->in.left );
				canon( p );
				if( p->in.op != OREG ){
					offstar( p->in.left );
					canon( p );
					if( p->in.op != OREG ) cerror( "stuck starg" );
					}
				}
			nor2reg = 0;
			}


 		ptemp->tn.lval = 0;	/* all moves to (sp) */

		pasg = talloc();

#if 0
		if( p->in.op == OREG && R2TEST(p->tn.rval) ){
					/* double indexing problem */
			pasg->in.op = STARG;
			pasg->in.left = p;
		}
		else 
#endif
		{
			pasg->in.op = STASG;
			pasg->in.right = p;
			pasg->in.left = tcopy( ptemp );

			/* the following line is done only with the knowledge
			that it will be undone by the STASG node, with the
			offset (lval) field retained */

			if( p->in.op == OREG )
				p->in.op = REG;  /* only for temporaries */
		}
		pasg->stn.stsize = size;
		pasg->stn.stalign = align;
		pasg->stn.off = off;
		pasg->stn.dst = dst;
		pasg->stn.arg = arg;
		pasg->stn.alink = al;

 		order( pasg, FORARG );
		ptemp->tn.lval += size;
		return;
		}

	/* ordinary case */

	if (pushcnt)
		walkf(p, changespoff);

	order( p, FORARG );
	}

argsize( p ) register NODE *p; {
	register t;
	t = 0;
	if( p->in.op == CM ){
		t = argsize( p->in.left );
		p = p->in.right;
		}
	if( p->in.type == DOUBLE || p->in.type == FLOAT ){
		SETOFF( t, 8 );
		return( t+8 );
		}
	else if( p->in.op == STARG ){
 		SETOFF( t, 4 );  /* alignment */
 		return( t + ((p->stn.stsize+3)/4)*4 );  /* size */
		}
	else {
		SETOFF( t, 4 );
		return( t+4 );
		}
	}


