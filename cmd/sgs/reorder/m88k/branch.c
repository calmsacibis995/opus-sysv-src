#ident "@(#) $Header: branch.c 4.1 88/04/26 17:21:40 root Exp $ RISC Source Base"
/****************************************************************************/
/*                              BRANCH.C                                    */
/****************************************************************************/
/*      This file contains the functions  and data structures which         */
/*      Optimize the flow structure of the function being reorganized       */
/*--------------------------------------------------------------------------*/
/*      Branch instructions are chained to their final target               */
/*              by skipping labels with no code attached                    */
/*              and by chaining forward if the label is attached to         */
/*                      a direct branch instruction                         */
/*      Branch to the next Label are also removed when the branch is the    */
/*              last instruction in the extended basic block                */
/****************************************************************************/
/*      The Text and algorithms contained herein are the sole unpublished   */
/*      property of Motorola Inc.  Copyright  1987  All Rights Reserved     */
/****************************************************************************/

# include "types.h"

extern  Symbol  *lookup();
extern  Symbol  *extu,  /* lookup( "extu", INSTRUCTION ); */
		*eq0,   /* lookup( "eq0",  CONDITION   ); */
		*ne0,   /* lookup( "ne0",  CONDITION   ); */
		*gt0,   /* lookup( "gt0",  CONDITION   ); */
		*ge0,   /* lookup( "ge0",  CONDITION   ); */
		*lt0,   /* lookup( "lt0",  CONDITION   ); */
		*le0,   /* lookup( "le0",  CONDITION   ); */
		*bb0,   /* lookup( "bb0",  INSTRUCTION ); */
		*bb1,   /* lookup( "bb1",  INSTRUCTION ); */
		*bcnd;  /* lookup( "bcnd" ,INSTRUCTION ); */

# define extract_unsigned(i) (i->opcode == extu)

static
long    found,          /* instructions which LOOK like branches */
	removed,        /* branches removed by flow minimization */
	extract;        /* extu---bcnd changed to bb             */
long                    /* external reference in machine.c       */
	filled;         /* branches which have delay slot filled */

init_branches()
{
	found   = 0;
	filled  = 0;
	removed = 0;
	extract = 0;
}

print_branches()
{
	long    branch_count = found - removed;

	if( branch_count )
		printf( "; branches\tfound = %d\tremoved = %d\t| %d %%\n;\t    remaining = %d\t filled = %d\t| %d %% \n",
			found, removed, 100*removed/found, branch_count, filled, 100*filled/branch_count );
# ifdef EXTRACT
	printf( "; extracts  converted = %d\n", extract );
# endif
}

/****************************************************************************/
/*      target_label    skip forward over labels without code attached      */
/****************************************************************************/

static
Label *
target_label( l )
Label        *l;
{
	while( l           &&   /* label exists        */
	       l->inst == 0 )   /* and no instructions */
	       l = l->next;     /* skip forward        */

	return l;
}

/****************************************************************************/
/*      reverse_branch  This routine takes a conditional branch and reverses*/
/*                      the take branch selection process                   */
/*--------------------------------------------------------------------------*/
/*      bb0[.n] bit,reg,label   ->      bb1[.n] bit,reg,label               */
/*      bb1[.n] bit,reg,label   ->      bb0[.n] bit,reg,label               */
/*      bcnd    cnd,reg,label   ->      bcnd   ~cnd,reg,label               */
/*      bcnd   ~cnd,reg,label   ->      bcnd    cnd,reg,label               */
/****************************************************************************/

static
void
reverse_branch( i, l )
Inst           *i;
Symbol            *l;
{
	Opcode *op;
	Label  *target;

	if( i                     &&    /* protection   */
	    ( op = isopcode( i ) ) )    /* access table */
	{       /* the table of information exists      */

		if( op->spelling[2] == '0' )
			i->opcode += bb1 - bb0;
		else if( op->spelling[2] == '1' )
			i->opcode += bb0 - bb1;
		else if( op->spelling[2] == 'n' )
		{
			if( i->operand[0] == eq0 )
				i->operand[0] = ne0;
			else if( i->operand[0] == ne0 )
				i->operand[0] = eq0;
			else if( i->operand[0] == gt0 )
				i->operand[0] = le0;
			else if( i->operand[0] == ge0 )
				i->operand[0] = lt0;
			else if( i->operand[0] == lt0 )
				i->operand[0] = ge0;
			else if( i->operand[0] == le0 )
				i->operand[0] = gt0;
			else
			{
				char *p = i->operand[0]->spelling;
				if( *p == '~' )
					i->operand[0] = lookup( p + 1, CONDITION );
				else
				{
					char    c = *--p;
					*p = '~';
					i->operand[0] = lookup( p, CONDITION );
					*p = c;
				}
			}
		}
		else    error( "reversing non-conditional branch\n" );
		/* attach to the new target label */
		i->operand[2] = l;
	}
}

# ifdef EXTRACT
/****************************************************************************/
/*      bit_branch      This routine performs the following rewriting       */
/*--------------------------------------------------------------------------*/
/*      extu    rd,rs,1<?>      |                                           */
/*      bcnd    eq0,rd,label    |->     bb0     ?,rs,label                  */
/****************************************************************************/

static
BOOLEAN
bit_branch( bcnd, extu ,rs )
Inst       *bcnd,
		 *extu;
BITMASK                 rs;
{
	char    buffer[4],      /* big enough */
		*p;             /* chaser     */

	if( extu->operand[2]->type & CONDITION )
	{       /* this constant field is used as a conditional */

		p = extu->operand[2]->spelling;
		if( *p == '1' && *++p == '<' )
		{       /* change '1<n>' in extu to 'n' in bb */
			buffer[0] = *++p;
			if( *++p != '>' )
				buffer[1] = *p++;
			else
				buffer[1] = '\0';
			if( *p != '>' )
				return FALSE;
			buffer[2] = '\0';
			p = buffer;
		}
		if( bcnd->operand[0] == eq0 )
		{       /* bcnd -> bb0  */
			bcnd->src[0]     = rs;
			bcnd->opcode     = bb0;
			bcnd->operand[0] = lookup( p, CONDITION );
			bcnd->operand[1] = extu->operand[1];
			return TRUE;
		}
		if( bcnd->operand[0] == ne0 )
		{       /* bcnd -> bb1  */
			bcnd->src[0]     = rs;
			bcnd->opcode     = bb1;
			bcnd->operand[0] = lookup( p, CONDITION );
			bcnd->operand[1] = extu->operand[1];
			return TRUE;
		}
	}       /* otherwise I can't change this pair           */
	return FALSE;
}

# endif
/****************************************************************************/
/*	chain_labels	chase labels to their final destination             */
/****************************************************************************/
Symbol *
chain_labels( sym )
Symbol       *sym;
{
	Label	*target;

	if( sym                      &&
	    sym->type == LABEL_FIELD &&
	    ! chained( sym )         )
	{	/* chain this sub portion once */
		target = target_label( sym->def.label );
		if( target )
		{
			sym = target->symbol;
			if( target->inst               &&
		    	target->inst->next == NULL &&
		    	direct( target->inst )     )
			{	/* chain forward       */
				sym->state = CHAINED;
				target->inst->operand[0] = sym = chain_labels( target->inst->operand[0] );
			}
		}
	}
	return sym;
}

/****************************************************************************/
/*      optim_branches                                                      */
/*                      do branch chaining optimizations                    */
/*              and     do code hoisting of used once labels                */
/****************************************************************************/

void
optim_branches( first )
Label          *first;
{
	Label   *l,
		*target;
	Inst    *i,
		*next,
		*prev,          /* prev->next == i              */
		*back_patch = 0;/* for used once label hoisting */
	Symbol  *sym;

	for( l = first; l; l = l->next )
	{	/* a non-local label is assumed to be used */
		if( ! local_label( l )            &&
		    ( target = target_label( l ) ) )
			target->use++;

		for( prev = (Inst *)&(l->inst), i = l->inst;
		     next = i;
		     prev = next, i = prev->next )
		{
			if( branches( i ) )
			{
				found++;                /* statistics     */
				if( ! delayed( i ) )
				{
					next = i->next;	/* skip delay slot*/
					filled++;	/* statistics     */
				}
			}
			if( labeled( i ) )
			{       /*********************************************************************/
				/*      DIRECT and CONDITIONAL branching                             */
				/*-------------------------------------------------------------------*/
				/*      chase labels to their final target                           */
				/*      and perform small scale flow optimizations                   */
				/*********************************************************************/

				/*********************************************************************/
				/* branch chaining--chase the lable list until the final destination */
				/*********************************************************************/
chain_branch:                   if( branch( i ) )
					i->operand[0] = sym = chain_labels( i->operand[0] );
				else
					i->operand[2] = sym = chain_labels( i->operand[2] );  
				if( sym->def.label )
					sym->def.label->use++;

				/*****************************************************************************/
				/* branch to next instruction block                                          */
				/*****************************************************************************/
				if( target                           && /* found destination                 */
				    target == target_label( l->next ) ) /* branches to next real instruction */
				{
					if( ! i->next )                 /* last instruction in block         */
					{       /* branch to next label--remove branch                       */
						/*-----------------------------------------------------------*/
						/*      :               |        :                           */
						/*      branch-any  L0  |->                                  */
						/* L0:  :               |    L0: :                           */
						/*-----------------------------------------------------------*/
						prev->next = 0;         /* remove from chain                 */
						removed++;
						free( i );              /* remove from memory                */
						break;                  /* i->next is not a vaild instruction*/
					}       /*-----------------------------------------------------------*/
					else if( conditional( i )   &&  /* conditional branch                */
						 direct( i->next )  &&  /* followed by a direct branch       */
						 ! i->next->next     )  /* which is the last in a block      */
					 {      /*-----------------------------------------------------------*/
						/*      cbranch ?,r,L0  |                                    */
						/*      branch  L1      |->     cbranch-backwards ?,r,L1     */
						/* L0:  :               |  L0:  :                            */
						/*___________________________________________________________*/
						reverse_branch( i, i->next->operand[0] );
						found++;
						removed++;
						free( i->next );        /* remove from memory                */
						i->next = 0;            /* remove from link                  */
						goto chain_branch;
					}
				}
			} /* branches */
			/****************************************************/
			/*      attempt to push the stores down the stream  */
			/****************************************************/
			else if( store( i ) &&  /* we have a store          */
				 i->next      ) /* and code follows         */
				push_stores( prev, i );
# ifdef EXTRACT
			else if( extract_unsigned( i ) )
			{       /*****************************************************************************/
				/*      extu    rd,rs,1<?>      |       :                                    */
				/*      :                       |       :       no uses of rd                */
				/*      :       no uses of rd   |->     :                                    */
				/*      :                       |       bb0     ?,rs,label                   */
				/*      bcnd    eq0,rd,label    |                                            */
				/*****************************************************************************/
				BITMASK  rs = i->src[0],
					 rd = i->dest[0];
				Inst    *bcnd;

				if( rs != rd )          /* if rd == rs then both regs have same scoreboard   */
					rs = rs & ~rd;  /* otherwise--remove destination from sources        */

				for( bcnd = i->next; bcnd ; bcnd = bcnd->next )
					if( branches( bcnd ) )
					{
						if( isopcode( bcnd )->spelling[2] == 'n' &&
								     bcnd->src[0] == rd     )
						{       /************************ FOUND IT *******************/
							if( bit_branch( bcnd, i, rs ) )
							{       /*-------------- Remove It ------------------*/
								prev->next = i->next;/* remove extu from list*/
								extract++;           /* statistics           */
								free( i );           /* and from memory      */
							}       /*-------------------------------------------*/
						}       /*****************************************************/
						break;
					}
					else if( rd & sources( bcnd ) ||/* a  uses of result */
						 rs & results( bcnd )  )/* a change to source*/
						break;  /* interference was seen--abandon attempt----------- */
			}
# endif
			if( ! next->next ) /* last instruction special processing */
			{
				if( direct( i ) )       /* is a direct branch  */
				{       /* if label is found attempt to hoist  */
					if( i->operand[0]->def.label )
					{       /* link direct branches and back patch */
						i->operand[2] = (Symbol *)back_patch;   /* a link for ordering */
						back_patch    = i;
					}                                               /* a non direct branch */
				}
				else if( jump( i ) )
					;
				else if( target = target_label( l->next ) ) /* which did not fall off the earth  */
					target->use++;                      /* count fall throughs in label uses */
			}
		} /* for instructions */
	} /* for all labels           */
	while( back_patch )
	{       /************************************************************/
		/*      for all the direct branch instructions which        */
		/*      terminate their code block                          */
		/*----------------------------------------------------------*/
		/*      If the target label is used only once               */
		/*              hoist the code to the point of the branch   */
		/************************************************************/
		i             = back_patch;                       /*--------*/
		target        = i->operand[0]->def.label;         /* extract*/
		back_patch    = (Inst  *)i->operand[2];           /* data   */
		i->operand[2] = 0;                                /*--------*/

		if( target->use == 1 )
		{
			for( prev = target->inst; prev->next; prev = prev->next )
				;       /* find end of target insructions   */
			if( direct( prev ) )
			/* previous line changed from branch(prev) */
			{       /* this instruction block is easy to move   */
				*i = *target->inst;
				free( target->inst );
				removed++;
				target->use  = 0;
				target->inst = 0;
				l = target->next;
				*target = *l;
				free( l );
			}
		}
	}
}
