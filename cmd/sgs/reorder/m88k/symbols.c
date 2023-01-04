#ident "@(#) $Header: symbols.c 4.1 88/04/26 17:22:10 root Exp $ RISC Source Base"
/****************************************************************************/
/*      symbols.c       This file contains the symbol table used to unify   */
/*                      the flow of information around the program graph    */
/****************************************************************************/
/*                      The symbol table is hash based with linear colision */
/*                      all similar spellings are grouped under a cluster   */
/****************************************************************************/
/*      Author  WGF1                                                        */
/****************************************************************************/
/*      The Text and algorithms contained herein are the sole unpublished   */
/*      property of Motorola Inc.  Copyright  1987  All Rights Reserved     */
/****************************************************************************/

# include "types.h"

/****************************************************************************/
/*      hash            compute hash function based on spelling             */
/*                      from P.J.Weinberger's C compiler                    */
/****************************************************************************/
/*      These Magic Constants Seem to work well on 32 bit machines          */
/*--------------------------------------------------------------------------*/
# define OVRF   4                       /* number of bits    */
# define BITS   (32-OVRF)               /* shift count       */
# define TABLESIZE 509                  /* some prime number */

/*--------------------------------------------------------------------------*/
/*      The size of the hash table is a comprimize between memory size and  */
/*      search time.  Since the Reorganizer is not Symbol Table Bound it    */
/*      makes sense to use about one Page of memory.                        */
/*--------------------------------------------------------------------------*/
static  Symbol  *hash_table[TABLESIZE];

static
unsigned long hash( p )
char               *p;
{
	unsigned long h;

	h = *p++;
	while( *p )
	{       /* accumulate the nect character into the hash */
		h =   ( h << OVRF ) + *p++;
		h = ( ( h << OVRF ) >> OVRF ) ^ ( ( h >> BITS ) << OVRF );
	}
	return h % TABLESIZE;
}

void
print_symbol( s )
Symbol       *s;
{
	if( s != 0 )
		printf( "type %2.2x value %8.8x '%s'\n",s->type, s->def.value, s->spelling );
}

/****************************************************************************/
/*      lookup  compute hash and chase hast table to entry                  */
/****************************************************************************/
/*      The type parameter is an array of bits so that the table can be     */
/*      searched for specific or unified objects in the table               */
/****************************************************************************/
/*      Notice that if the object is not found--it is created               */
/*      Durring its creation, the size of the structure is adjusted to its  */
/*      actual size needs                                                   */
/****************************************************************************/

Symbol *lookup( p, type )
char           *p;
Type               type;
{
	unsigned long h;
	Symbol       *s;

	h = hash( p );
	for( s = hash_table[h]; s ; s = s->next )
	{
		if( ( s->type & type ) &&
		    strcmp( p, s->spelling ) == EQUAL )
			return s;
	}

	/* insert new entry */
	s = (Symbol *)malloc( sizeof( Symbol ) - sizeof( s->spelling ) + 1 + strlen( p ) );
	if( s )
	{
		s->type       = type;           /* maintain type */
		s->state      = NULL;		/* no state      */
		s->next       = hash_table[h];  /* frontal insert*/
		hash_table[h] = s;              /* into table    */
		s->def.label  = 0;              /* union of defs */
		strcpy( s->spelling, p );       /* hold spelling */
	}
	else
		error("malloc failed in lookup\n");
	return s;
}

void
dump_symbols()
{
	unsigned long i;
	Symbol       *s;

	for( i = 0; i < TABLESIZE; i++ )
	{
		printf( "hash[%d] =\t", i );
		for( s = hash_table[i]; s; s = s->next )
			printf("%s %x\t", s->spelling, s->type );
		putchar( '\n' );
	}
}

/****************************************************************************/
/*      init_symbols    called at each new function                         */
/****************************************************************************/
/* dont remove       register names   or  instructions from symbol table    */
/****************************************************************************/

extern   Symbol instructions[],
	       *last_instruction;

# define hardwired(s) (s >= instructions && s < last_instruction)

void
init_symbols()
{
	Symbol       *s,
		     *snext;
	unsigned long i;

	for( i = 0; i < TABLESIZE; i++ )
	{
		for( s = hash_table[i]; s && !hardwired(s); s = snext )
		{
			snext = s->next;
			free( s );      /* free any attached nodes */
		}
		hash_table[i] = s;      /* and note in table       */
	}
}

/****************************************************************************/
/*      insert  compute hash and insert into table as type                  */
/****************************************************************************/
/*      This function is used only at initialization time and is called     */
/*      only from table.c to install the reserved words into the table      */
/****************************************************************************/
void
insert( p, type, s )
char   *p;
Type       type;
Symbol          *s;
{
	unsigned long h;

	h = hash( p );
	/* insert new entry */
	s->type       = type;           /* maintain type */
	s->state      = NULL;		/* no state      */
	s->next       = hash_table[h];  /* frontal insert*/
	s->def.label  = 0;              /* union of defs */
	hash_table[h] = s;
	strcpy( s->spelling, p );       /* hold spelling */
}

