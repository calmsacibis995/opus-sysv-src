/****************************************************************************/
/*      graph.c this file builds and maintains the flow graph of the text   */
/****************************************************************************/
/*      The flow graph is a linked list of labels.                          */
/*      Each label is linked to its symbol table entry and                  */
/*                    may have a linked list of instructions attached.      */
/*      Each instruction is linked to its symbol table entry and            */
/*              each operand is linked to its symbol table entry            */
/*      Operands are either Labels or registers                             */
/*              Labels are raw spellings                                    */
/*              Registers are r0 to r31 inclusive                           */
/****************************************************************************/
/*      The Text and algorithms contained herein are the sole unpublished   */
/*      property of Motorola Inc.  Copyright  1987  All Rights Reserved     */
/****************************************************************************/

# include "types.h"

# ifdef SV      /* System 5 */
# define static
# endif

extern  Symbol  *lookup();      /* found in symbols.c */

static  Label   first;
static  Label   *current_label;
static  Inst    *current_inst;


# ifdef G1
void
init_graph()
{
	if( current_label != (Label *)&first.next )
	{       /* must deallocate current stuff */
		Label   *l,
			*nextl;
		Inst    *i,
			*nexti;

		for( l = (Label *)first.next; l; l = nextl )
		{
			nextl = l->next;        /* cannot dereference after free */
			for( i = l->inst; i ; i = nexti )
			{
				nexti = i->next;/* cannot dereference after free */
				free( i );
			}
			free( l );
		}
	}
	current_label = (Label *)&first.next;
	current_inst  = (Inst  *)&first.inst;
	init_branches();
	init_symbols();
	init_statistics();
}

# endif

# ifdef G2
/****************************************************************************/
/*  Operands    this routine is called to parse Operands from the operands  */
/*              This routine would be in the input section except for its   */
/*              single use in operator, and the need to transfer data to    */
/*              operator through the regs[]  array  which can now be static */
/****************************************************************************/

static
void
Operands( op, inst, operands )
Opcode    *op;
Inst          *inst;            /* inst must not be zero */
char                *operands;
{
	char     c,
		*spelling = &inst->spelling[0],
		*operand;
	unsigned i,
		 oprnd;
	Type     t;

	inst->operand[0] = 0;
	inst->operand[1] = 0;
	inst->operand[2] = 0;

	if( op == 0 )
	{       /* unrecognized instruction is dangerous */
		if( operands != 0 )
			inst->operand[0] = lookup( operands, ANYTHING );
	}
	else    /* normal operand parsing                */
	{
		for( i = 0; i < 3; i++ )
		{       /* there are only 3 operand fields in M88000 instructions   */
			if( op->regs[i] == NOT_USED )
				break;
			else
			{       /* attach symbol to symbol table of field type */
				operand = operands++;
				/* skip to delimiter */
				while( ! delimiter( *operands ) )
					operands++;
				/* copy delimiter to spelling */
				*spelling++ = *operands;
				*operands++ = '\0';
				if( op->regs[i] > REGISTER )
					t = op->regs[i];
				else if( i == 2 )
					t = REGISTER | LABEL_FIELD | CONDITION;
				else    t = REGISTER;

				oprnd = op->regs[i] < NOT_USED ? op->regs[i] : i;

				inst->operand[ oprnd ] = lookup( operand, t );

				if( inst->operand[ oprnd ]->type == ( REGISTER | LABEL_FIELD | CONDITION ) )
					/* lookup installed new name */
					inst->operand[ oprnd ]->type = LABEL_FIELD;
			}
		}
	}
	*spelling++ = '\0';
}

# endif

# ifdef G7
/****************************************************************************/
/*      print_inst      Output a visible form of this instruction           */
/****************************************************************************/

void
print_inst( i )
Inst       *i;
{
	unsigned r;
	char    *sp;

	if( i )
	{
		sp = i->spelling;
		printf( "\t%s\t", i->opcode->spelling );

		for( r = 0; r < 3 ; r++)
			if( i->operand[r] )
			{       /* print field and delimiter as needed */
				printf( "%s" , i->operand[r]->spelling );
				if( i->spelling[r] )
					putchar( i->spelling[r] );
			}
	}
}

# endif

# ifdef G3

/****************************************************************************/
/*      source          symbol s is used as a source operand                */
/*                      track refs, defs, and uses before defs              */
/****************************************************************************/

void
source( i, s, c, track )
Inst   *i;
Symbol    *s;
BITMASK      *c;
BOOLEAN          track;
{
	BITMASK src1,
		src2;

	if( s && s->type & REGISTER )
	{       /*      Build scoreboards            */
		src1       = (  (int)c       << s->def.value ) & ~ 1;
		src2       = ( ((int)c & 2 ) << s->def.value ) & ~ 1;
		i->src[0] |= src1 | src2;
		i->src[1] |= src2;
		if( track )
		{       /*      track ref's and def's        */
			current_label->refs |= src1 | src2;
			if( src1 & ~ current_label->defs )
				/* ref before def */
				current_label->before |= src1;
			if( src2 & ~ current_label->defs )
				/* ref before def */
				current_label->before |= src2;
		}
	}
}

/****************************************************************************/
/*      result          symbol s is used as a result                        */
/*                      track defs                                          */
/****************************************************************************/

void
result( i, s, c, track )
Inst   *i;
Symbol    *s;
BITMASK      *c;
BOOLEAN          track;
{
	if( s->type & REGISTER )
	{       /*      Build scoreboards            */
		i->dest[0] = ((int)c & 1 ) << s->def.value;
		i->dest[1] = ((int)c & 2 ) << s->def.value;
		/*      track defs                   */
		if( track )
			current_label->defs |= i->dest[0] | i->dest[1];
	}
	else    error( "result is not a register\n" );
}

/****************************************************************************/
/*      label   this routine is called each time a label is parsed in the   */
/*              source input--its responsibility is to build a new label    */
/*              structure and concatenate it onto the current label list    */
/****************************************************************************/

void label( buffer )
char       *buffer;
{
	Label *newlab;
	extern	char    *malloc();

	newlab = (Label *)malloc( sizeof( Label ) );
	if( newlab )
	{
		newlab->next   = 0;
		newlab->inst   = 0;
		newlab->use    = 0;
		newlab->defs   = 0;
		newlab->refs   = 0;
		newlab->before = 0;
		current_label  = current_label->next = newlab;
		current_inst   = (Inst *)&newlab->inst;
		newlab->symbol = lookup( buffer, LABEL_FIELD );
		newlab->symbol->def.label = newlab;
	}
	else
		error("malloc failed in label\n" );
}

# endif

# ifdef G6
/****************************************************************************/
/*    operator  This routine is called each time an operator is parsed in   */
/*              the source.   Its responsibility is to build a new operator */
/*              structure and concatenate it onto the current operator list */
/****************************************************************************/

void
operator( opcode, operands, opercnt )
unsigned                    opercnt;
char     *opcode,
		 *operands;
{
	extern	char    *malloc();
	Inst    *inst;
	Symbol  *op;

	if( (strcmp( opcode, "def" ) == EQUAL) ||
	    (strcmp( opcode, "global" ) == EQUAL) )
		printf("\t%s\t%s\n",opcode,operands);
	else
	{
		op = lookup( opcode, INSTRUCTION );
		inst = (Inst *)malloc( sizeof( Inst ) );
		if( opercnt == 0 )
			operands = 0;

		if( inst )
		{       /* parse register information and build instruction */
			inst->next       = 0;
			inst->opcode     = op;
			Operands( op->def.opcode, inst, operands );
		}
		else
			error("malloc failed on operator\n");

		instruction_statistics( inst );
		current_inst->next = inst;
		current_inst = inst;
	}
}

# endif

# ifndef SV     /* System 5 */

static  unsigned char   do_branches = TRUE,     /* defaults */
			do_reorder  = TRUE,
			do_values   = TRUE,
			show_before = FALSE,
			show_final  = FALSE,
			show_stats  = FALSE,
			show_asm    = TRUE;

# else
extern  unsigned char   do_branches,            /* defaults */
			do_reorder,
			do_values,
			show_before,
			show_final,
			show_stats,
			show_asm;
# endif

# ifdef G8
/****************************************************************************/
/*  dumpgraph   This routine is called to dump the current flowgraph        */
/****************************************************************************/

void
dumpgraph()
{
	Inst    *i;
	Label   *l;

	for( l = first.next; l; l = l->next )
	{       /* traverse all labels */

		if( show_stats )
			printf( "%s:\t;use %d in %8.8x sources %8.8x results %8.8x\n",
			l->symbol->spelling, l->use, l->before, l->refs, l->defs );
		else    printf( "%s:\n", l->symbol->spelling );

		for( i = l->inst; i; i = i->next )
		{
			print_inst( i );
			putchar( '\n' );
		}
	}
}

# endif

# ifdef G9
/****************************************************************************/
/*      incorrect process usage message                                     */
/****************************************************************************/

static  char    usage[] =
"usage:\treorder <stdin >stdout options\n\
      option   default  useage\n\
\t-b\ttrue\tbranch optimizations\n\
\t-r\ttrue\treorganize optimizations\n\
\t-v\tfalse\tvalue optimizations--not yet implimented\n\
\t-sb\tfalse\tshow cycle count before reorganization\n\
\t-sf\tfalse\tshow cycle count after  reorganization\n\
\t-sa\ttrue\tshow final assembly code\n\
\t-ss\tfalse\tshow statistics\n\
use of an option inverts its true/false state\n\
example:\treorder <file.s >file.sc -sfsb\n";

void
options( argc, argv )
unsigned argc;
char          *argv[];
{
	unsigned i;
	char    *p,
		 c;

	for(  i = 0; i < argc; i++ )
	{
		p = argv[i];
		if( ( c = *p++ ) == '-' )
			while( c = *p++ )
				switch( c )
				{
				case 'b':       /* branch  optimizations */
					do_branches = ! do_branches;
					break;
				case 'r':       /* reorder optimizations */
					do_reorder  = ! do_reorder;
					break;
				case 'v':       /* reorder optimizations */
					do_values   = ! do_values;
					break;
				case 's':
					if( ( c = *p++ ) == 'b' )
						show_before = ! show_before;
					else if( c == 'f' )
						show_final  = ! show_final;
					else if( c == 'a' )
						show_asm    = ! show_asm;
					else if( c == 's' )
						show_stats  = ! show_stats;
					else
						p--;
				}
		else if( c == '<' )
			freopen( p, "r", stdin );
		else if( c == '>' )
			if( *p == '>' )
				freopen( ++p, "a", stdout );
			else    freopen(   p, "w", stdout );
	}

	if( isatty( 0 ) )
		error( usage );
}
# endif

# ifdef G0
# ifdef SV      /* System 5 */

unsigned char   do_branches = TRUE,     /* defaults */
		do_reorder  = TRUE,
		do_values   = TRUE,
		show_before = FALSE,
		show_final  = FALSE,
		show_stats  = FALSE,
		show_asm    = TRUE;

# endif

void
reorder()
{
	if( show_before )
		compute_cycles( first.next, TRUE, FALSE );
	if( do_branches )
		optim_branches( first.next );
	if( do_values )
		value_optimizations( first.next );
	if( do_reorder )
		compute_cycles( first.next, FALSE, TRUE );
	if( show_final )
		compute_cycles( first.next, TRUE, FALSE );
	if( show_stats )
	{
		if( do_branches )
			print_branches();
		print_statistics();
	}
	if( show_asm )
		dumpgraph();
	init_graph();
}
# endif

