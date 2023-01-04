#ident "@(#) $Header: value.c 4.1 88/04/26 17:22:32 root Exp $ RISC Source Base"
/****************************************************************************/
/*                              VALUE.C                                     */
/****************************************************************************/
/*      This file contains the functions  and data structures which         */
/*      Optimize the flow structure of the function being reorganized       */
/*      based on the values computed and the registers used                 */
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

extern  Symbol  *stack_pointer, /* lookup( "r31", REGISTER );     */
		*frame_pointer, /* lookup( "r30", REGISTER );     */
		*register_0,    /* lookup( "r0",  REGISTER );     */
		*register_1;    /* lookup( "r1",  REGISTER );     */
static
unsigned        normals,        /* computations like add r5,r6,r7 */
		stack_loads,    /* loads        like ld  r5,r31,8 */
		stack_stores,   /* stores       like st  r5,r31,8 */
		frame_loads,    /* loads        like ld  r5,r30,8 */
		frame_stores,   /* stores       like st  r5,r30,8 */
		loads,          /* loads  of other kinds          */
		stores,         /* stores of other kinds          */
		subroutines,    /* count of bsr--jsr instructions */
		unclassified;   /* memory traffic {} - r0,r30,r31 */
static
BOOLEAN         stack_aliased,
		frame_aliased;

# define MAX_MEM 32

typedef struct VALUE Value;

static struct VALUE     {       Inst    *inst;
				unsigned stores,
					 loads;
			} memory [ REG_NUM ][ MAX_MEM ];

static unsigned bases[ REG_NUM ];

void
init_statistics()
{       /* reset all counters */
	unsigned i;

	normals       = 0;
	stack_aliased = FALSE;
	stack_loads   = 0;
	stack_stores  = 0;
	frame_aliased = FALSE;
	frame_loads   = 0;
	frame_stores  = 0;
	loads         = 0;
	stores        = 0;
	subroutines   = 0;
	unclassified  = 0;
	for( i = 0; i < REG_NUM; i++ )
		bases[ i ] = 0;
}

# ifdef DEBUG
void
dump_values()
{
	unsigned i,
		 j,
		 count;

	for( i = 0; i < REG_NUM; i++ )
	{
		count = bases[ i ];
		for( j = 0; j < count; j++ )
		{
			printf( "; loads %d stores %d",
				memory[ i ][ j ].loads,
				memory[ i ][ j ].stores );
			print_inst( memory[ i ][ j ].inst );
			putchar( '\n' );
		}
	}
}
# endif

static  char   *truth[] = { "is not", "  is  "  };

void
print_statistics()
{       /* print coalessed data */
	printf( ";\tcomputational =\t%d\t  calls = %d\n;\t\tloads = %d\t stores = %d\n",
		normals, subroutines,
		stack_loads  + frame_loads  + loads,
		stack_stores + frame_stores + stores );
	printf( "; stack %s aliased--frame %s aliased\n",
		truth[ stack_aliased ],
		truth[ frame_aliased ] );
	printf( "; unique stack  %d\t unique frame %d\n; unique global %d\t unclassified %d\n",
		bases[ 31 ], bases[ 30 ], bases[ 0 ], unclassified );
# ifdef DEBUG
	dump_values();
# endif
}

static
void
memory_operation( i )
Inst             *i;
{
	unsigned g,
		 base,
		 count;

	base  = i->operand[1]->def.value;
	count = bases[ base ];

	for( g = 0; g < count; g++ )
		if( i->operand[2] == memory[ base ][ g ].inst->operand[2] )
		{
			if( isopcode( i )->format & LOAD )
				memory[ base ][ g ].loads++;
			else
				memory[ base ][ g ].stores++;
			return; /* when found */
		}
	if( count < MAX_MEM )
	{       /* remember new item */
		memory[ base ][ count ].inst    = i;
		bases [ base ]++;
		if( isopcode( i )->format & LOAD )
		{
			memory[ base ][ count ].loads  = 1;
			memory[ base ][ count ].stores = 0;
		}
		else
		{
			memory[ base ][ count ].loads  = 0;
			memory[ base ][ count ].stores = 1;
		}
	}
# ifdef DEBUG
	else
		printf(" ran over bounds in memory \n");
# endif
	if( base != 0 && base != 30 && base != 31 )
		unclassified++;
}

/****************************************************************************/
/*      instruction_statistics  This function gleans valueable information  */
/*                              from instructions durring the build of the  */
/*                              flow graph, which is used by later phases   */
/****************************************************************************/

void
instruction_statistics( i )
Inst                   *i;
{
	Opcode *op;

	if( op = isopcode( i ) )
	{
		i->src[0]  = 0;	/* clear current scoreboards */
		i->src[1]  = 0;
		i->dest[0] = 0;
		i->dest[1] = 0;
		if(      op->format & NORM  )
		{
			/****************************************************/
			/*      if the stack pointer or the frame pointer   */
			/*      are used to compute anything other than     */
			/*      themselves--assume that the stack/frame     */
			/*      area is comprimized by aliasing             */
			/****************************************************/
			if( i->operand[1] == stack_pointer ||
			    i->operand[2] == stack_pointer )
			{
				if( i->operand[0] != stack_pointer &&
				    i->operand[0] != frame_pointer )
					stack_aliased = TRUE;
			}
			if( i->operand[1] == frame_pointer ||
			    i->operand[2] == frame_pointer )
			{
				if( i->operand[0] != stack_pointer &&
				    i->operand[0] != frame_pointer )
					frame_aliased = TRUE;
			}
			source( i, i->operand[1], op->src1  , TRUE);
			source( i, i->operand[2], op->src2  , TRUE);
			result( i, i->operand[0], op->dwrite, TRUE);
			normals++;
		}
		else if( op->format & STORE )
		{
			if(      i->operand[1] == stack_pointer )
				stack_stores++;
			else if( i->operand[1] == frame_pointer )
				frame_stores++;
			else
				stores++;
			source( i, i->operand[0], op->dused , TRUE);
			source( i, i->operand[1], op->src1  , TRUE);
			source( i, i->operand[2], op->src2  , TRUE);
			memory_operation( i );
		}
		else if( op->format & LOAD  )
		{
			if(      i->operand[1] == stack_pointer )
				stack_loads++;
			else if( i->operand[1] == frame_pointer )
				frame_loads++;
			else
				loads++;
			source( i, i->operand[1], op->src1  , TRUE);
			source( i, i->operand[2], op->src1  , TRUE);
			result( i, i->operand[0], op->dwrite, TRUE);
			memory_operation( i );
		}
		else if( op->format & C     )
			source( i, i->operand[1], 1, TRUE );
		else if( op->format & JMP   )
			source( i, i->operand[1], 1, TRUE );
		if(      op->format & SUB   )
		{
			subroutines++;
			result( i, register_1 , 1, TRUE );
		}
	}
}

void
build_scoreboards( i )
Inst                   *i;
{
	Opcode *op;

	if( op = isopcode( i ) )
	{
		i->src[0]  = 0;	/* clear current scoreboards */
		i->src[1]  = 0;
		i->dest[0] = 0;
		i->dest[1] = 0;
		if(      op->format & ( NORM | LOAD )  )
		{
			source( i, i->operand[1], op->src1  , FALSE);
			source( i, i->operand[2], op->src2  , FALSE);
			result( i, i->operand[0], op->dwrite, FALSE);
		}
		else if( op->format & STORE )
		{
			source( i, i->operand[0], op->dused , FALSE);
			source( i, i->operand[1], op->src1  , FALSE);
			source( i, i->operand[2], op->src2  , FALSE);
		}
		else if( op->format & C     )
			source( i, i->operand[1], 1, FALSE);
		else if( op->format & JMP   )
			source( i, i->operand[1], 1, FALSE);
		if(      op->format & SUB   )
			result( i, register_1 ,   1, FALSE);
	}
}

hazard( s )
Symbol *s;
{	/* return the hazard associated with this base register */
	if( s == stack_pointer )
		return stack_aliased;
	if( s == frame_pointer )
		return frame_aliased;
	return TRUE;	/* be conservative */
}

/****************************************************************************/
/*      attempt to push the stores down the instruction stream              */
/*--------------------------------------------------------------------------*/
/*      be very careful about load/store boundaries                         */
/****************************************************************************/
void
push_stores( prev, i )
Inst        *prev,
		  *i;
{
	BOOLEAN	exchange;

	if( i->next && store( i->next ) )
		push_stores( i, i->next );
	while( i->next )
	{       /* pull past multiple stores in  line       */
		exchange = FALSE;
		/* push stores toward the end of the block  */
		if( load( i->next )		           &&
		    ! ( sources( i ) & results( i->next ) )  )	/* a load instruction   */
		{
			if( i->operand[1] == i->next->operand[1] &&     /* with same base       */
			    constant( i->operand[2] )            &&     /* and constant offsets */
			    constant( i->next->operand[2] )         )   /* in both instructions */
			{
				if( i->operand[2] == i->next->operand[2] )
				{       /* loads what is stored */
				}       /* leave alone for now  */
				else
				{       /* move store around load */
					exchange = TRUE;
				}
			}
			else if( hazard( i->operand[1] )      &&
				 hazard( i->next->operand[1] ) )
			{	/* don't interchange */
			}
			else
				exchange = TRUE;
		}
		else if( computational( i->next )              &&
			 ! ( sources( i ) & results( i->next ) ) )
		{       /* move computational above store */
			exchange = TRUE;
		}
		if( exchange )
		{
			prev->next       = i->next;
			i->next          = i->next->next;
			prev->next->next = i;
			prev             = prev->next;
		}
		else	break;
	}
}

void
align_memory( l )
Label        *l;
{
	Inst    *i,
		*inext;
	Label   *target;

	for( ; l ; l = l->next )
		if( l->use )
		{
		}
		else if( local_label( l ) )
		{       /* remove dead code */
			for( i = l->inst; i; i = inext )
			{
				if( branch( i ) )
				{
					if( target = i->operand[0]->def.label )
						target->use--;
				}
				else if( conditional( i ) )
				{
					if( target = i->operand[2]->def.label )
						target->use--;
				}
				inext = i->next;
				free( i );
			}
			l->inst = 0;
		}
}

value_optimizations( first )
Label               *first;
{
	align_memory( first );
}
