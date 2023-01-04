#ident "@(#) $Header: machine.c 4.1 88/04/26 17:21:56 root Exp $ RISC Source Base"
/****************************************************************************/
/*                              MACHINE.C                                   */
/****************************************************************************/
/*      This file contains the routines which simulate the clock by clock   */
/*      behavior of the M88000 processor chip.                              */
/****************************************************************************/
/*      Only the subroutine 'choose_next' is visible to the external world  */
/*                                                                          */
/*      This routine simulates the behavior of the machine based on:        */
/*              The M88000's register scoreboard and                        */
/*                           its function unit busy                         */
/*--------------------------------------------------------------------------*/
/*      Each Instruction is attached to a table which defines its resource  */
/*      and timing requirements, and the inner loop takes care of some      */
/*      special sequencing for branch, subroutine, and unrecognized code    */
/****************************************************************************/
/*      The subroutines *_write_slot                                        */
/*              Simulate the write slot arbitration of the M88000 processor */
/*              Simulate the function unit return to non busy               */
/*              and keep track of the clock cycles                          */
/****************************************************************************/
/*      The Text and algorithms contained herein are the sole unpublished   */
/*      property of Motorola Inc.  Copyright  1987  All Rights Reserved     */
/****************************************************************************/

# include "types.h"

/*--------------------------------------------------------------------------*/
/*      The write slot is managed by a circular buffer of sufficient size   */
/*      that double use cannot occur.  Since the longest instruction on the */
/*      M88000 Processor is 65 clocks, a buffer of 128 elements sufices     */
/*      and provides a simple means to perform wraparound (e.g. and 127)    */
/*--------------------------------------------------------------------------*/
# define WRITE_SLOTS 128
# define WRITE_MASK  (WRITE_SLOTS-1)

static Write_slot writeslot[WRITE_SLOTS];
static unsigned   write_slot;

# define EMPTY 32       /* NOTE   must be larger than any opcode_table.unit */
# define occupied(s) (writeslot[s].prio < EMPTY)
/*--------------------------------------------------------------------------*/
/*      Subret holds the last instructions to be issued before transfer of  */
/*      control is passed to the 'called' function.  Following the issue of */
/*      this instruction the write slot array is reinitialized (which also  */
/*      causes the scoreboard and the function unit busy data to be cleared */
/*      The reorganizer has no knowledge of the state of the machine as     */
/*      control is transfered back from the called routine, so starting     */
/*      starting with a clean slate seened to be the most rational thing.   */
/*--------------------------------------------------------------------------*/
static Inst      *subret;
static BITMASK    scoreboard,   /* 32 bit register stale scoreboard         */
		  fctn_busy;    /*  5 bit function unit busy                */
static unsigned   clock;        /* 32 bit counter                           */

# undef  branch           /* code reads better if this macro is overloaded  */
# define branch(op) (op->format & BR)
# define modifies_register(op) (op->dwrite)

/****************************************************************************/
/*      dump_write_slot  This routine dumps all scheduled write slots       */
/****************************************************************************/

static
void
dump_write_slot( )
{
	unsigned i,
		 j = write_slot + WRITE_SLOTS;

	for( i = write_slot; i < j; i++ )
		if( occupied(i&WRITE_MASK) )
			printf("write_slot[%d] dest %8.8x prio %d unit %2.2x\n",
				i - write_slot, writeslot[i&WRITE_MASK].dest,
				writeslot[i&WRITE_MASK].prio, writeslot[i&WRITE_MASK].unit);
}

/****************************************************************************/

static
void
init_write_slot()
{
	unsigned i;

	for( i = 0; i < WRITE_SLOTS; i++ )
	{       /* mark as empty */
		writeslot[i].dest = 0;
		writeslot[i].unit = 0;
		writeslot[i].prio = EMPTY;
	}
	/* initialize global bointer */
	write_slot = 0;
	scoreboard = 0;
	fctn_busy  = 0;
	subret     = 0;
}

/****************************************************************************/
/*      insert_write_slot this routine inserts the write slot data and      */
/*                        reorganizes the writes slots as needed            */
/****************************************************************************/

static
void
insert_write_slot( dest, prio, target )
BITMASK            dest;
PRIO                     prio;
unsigned                       target;
{
	BITMASK    ndest;
	PRIO       nprio;

	for(;;) /* while I'm holding an instruction to be inserted */
	{
		target &= WRITE_MASK;
		if( ! occupied( target ) )
		{       /* Insert me here                 */
			writeslot[target].dest  = dest;
			writeslot[target].prio  = prio;
			return;
		}
		else if ( writeslot[target].prio >= prio )
		{       /* I can be inserted here         */
			/* pick up what's already here    */
			ndest = writeslot[target].dest;
			nprio = writeslot[target].prio;
			/* Insert me here                 */
			writeslot[target].dest  = dest;
			writeslot[target].prio  = prio;
			/* change Me into what was there  */
			dest  = ndest;
			prio  = nprio;
		}
		/* go to next write slot */
		target++;
	}
}

/****************************************************************************/
/*      steal_write_slot  This routine steals the current write slot moving */
/*                        others  down as needed                            */
/****************************************************************************/

static
void
steal_write_slot( new_dest, unit )
BITMASK           new_dest;
PRIO                        unit;
{
	BITMASK dest = writeslot[write_slot].dest;
	PRIO    prio = writeslot[write_slot].prio;

	/* steal this write slot              */
	writeslot[write_slot].dest = new_dest;
	writeslot[write_slot].prio = unit;
	/* and move what was here down to its rightful place */
	if( prio != EMPTY )
		insert_write_slot( dest, prio, write_slot + 1 );
}

/****************************************************************************/
/*      new_write_slot  This routine inserts the 'commit'ted instruction    */
/*                      on the write slot queue, rescheduling as needed     */
/****************************************************************************/
static
void
new_write_slot( dest, inst, op, target )
BITMASK         dest;
Inst                 *inst;
Opcode                     *op;
unsigned                        target;
{

	scoreboard |= dest;

	if( op )
	{       /* instruction is understood -- use table to evaluate   */
		if( op->busy )
		{       /* mark function unit busy and set unbusy point */
			writeslot[(write_slot+op->busy)&WRITE_MASK].unit |= op->unit;
			fctn_busy                                        |= op->unit;
		}
		/* find first write slot available */
		while( writeslot[target&WRITE_MASK].prio <= op->unit )
			target++;
		/* and insert this on the write slot queue              */
		insert_write_slot( dest, op->unit, target );
	}
}

# define subroutine(i,busy) (subret = (busy) ? i : i->next)

/****************************************************************************/
/*      use_write_slot                                                      */
/****************************************************************************/

static
void
use_write_slot( i, show )
Inst           *i;
BOOLEAN            show;
{
	if( show )
	{
		if( occupied( write_slot ) )
			printf(" %8.8x", writeslot[write_slot].dest );
		else    printf(" --------" );
		print_inst( i );
		printf( i && ! isopcode( i ) ? "\t; is not understood\n" : "\n" );
	}
	/* update scoreboard */
	scoreboard &= ~writeslot[write_slot].dest;
	/* update function unit */
	fctn_busy  &= ~writeslot[write_slot].unit;
	/* remove from list */
	writeslot[write_slot].prio = EMPTY;
	writeslot[write_slot].dest = 0;
	writeslot[write_slot].unit = 0;
	/* update pointer */
	write_slot  = (write_slot + 1) & WRITE_MASK;
	clock++;
	if( i && i == subret )
		init_write_slot();
}

/****************************************************************************/
/*      choose_next     choose best instruction to execute                  */
/*--------------------------------------------------------------------------*/
/*	NOTE		sometimes the schedule does not fill delay slots    */
/*			when possible, but these have been found to take    */
/*			the same number of clocks with and with/out this    */
/*			delay slot utilization				    */
/****************************************************************************/

static
Inst * choose_next( i, sb, fu, reorder )
Inst               *i;         /* points to last instuction issued */
BITMASK                sb;     /* holds current scoreboard         */
UNIT                       fu; /*       current function unit busy */
BOOLEAN                        reorder;
{
	/********************************************************************/
	/* NOTE i may NOT be an (Inst *), but i->next ALWAYS IS             */
	/*         it may be an (Inst *)&l->inst                            */
	/********************************************************************/

	Inst    *select,
		*inst,
		*issue  = i,
		*choice = 0;    /* REMEMBER choice points to the instruction*/
				/* just prior to the actual choice          */
	BITMASK  source = 0,
		 block  = BR;   /* block all rearangements around branches  */
	Opcode  *op;

	if( reorder )
	{       /***********************************************************/
		/*              DELAYED BRANCH CODE SCHEDULING             */
		/*         examine only the first two instructions         */
		/***********************************************************/
		if( ( inst = i->next )                        && /* instruction exists           */
		    ! branches( inst )                        && /* I'm not a branch             */
		    inst->next                                && /* and not last                 */
		    branches( inst->next )                    && /* and next is branch           */
		    delayed( inst->next )                     && /* with waive off               */
		    ! ( sources(inst) & results(inst->next) ) && /* he doen't modify my sources  */
		    ! ( results(inst) & sources(inst->next) ) && /* and I don't modify his       */
		    ! ( sb & sources( inst->next ) )         )   /* and it can be issued NOW     */
		{       /******************* Put Me into Its Delay Slot **************************/
			extern  long    filled;               /* found in branch.c               */

			choice = inst;                        /* choose the branch instruction   */
			filled++;
			inst->next->opcode++;                 /* br -> br.n                      */
		}
		else
		/***********************************************************/
		/*              GENERAL CODE SCHEDULING SECTION            */
		/* attempt to reorder only non branch instructions         */
		/*         note--dangerous instructions could be a branch  */
		/*               or use the machine in unpredictable ways  */
		/*               therefore--dont touch'em                  */
		/***********************************************************/
		for(; ( inst = i->next        ) && /* visible instructions */
		      ( op = isopcode( inst ) ) && /* which are understood */
		      ! branch( op )               /* and do not branch    */
		    ; i = inst )
		{       /* inst QUALIFIES for rearangement issue */
			if( ! ( source & results( inst ) ||     /* the result does not write over any current source */
				sb     & sources( inst ) ||     /* and the registers are available                   */
				fu     & op->unit        ||     /* and the function unit is not busy                 */
				block  & op->format        ) )  /* nor is the instruction blocked from aliasing      */
			{       /* inst is ISSUABLE */
				if( choice == 0 ||
				    op->latency > isopcode( choice->next )->latency )
					/* inst is CHOSEN */
					choice = i;
			}
			source |=  sources( inst );
			sb     |=  results( inst );
			if(      op->format == LOAD  )
 				block |= STORE;
			else if( op->format == STORE )
 				block |= LOAD|STORE;

		} /* for each instruction */
		/************************************************************/
		/*      If a choice other than next instruction is made     */
		/*      relink pointers to effect this decision             */
		/************************************************************/
		if( choice &&  choice != i )
		{       /* rearange pointers to place inst at top */
			select       = choice->next;
			choice->next = select->next;
			i            = issue->next;
			issue->next  = select;
			select->next = i;
		}
	}
	return issue->next;
}

/****************************************************************************/
/*      compute cycles          mimic instruction initiation and termination*/
/****************************************************************************/

static  char    Format[] = "%4d %8.8x %2.2x";

void
compute_cycles( l, show, reorder )
Label          *l;
BOOLEAN            show,
			 reorder;
{
	Inst    *i,     /* current candidate for issue */
		*prev;  /* last instruction issued     */
	Opcode  *op;
	unsigned target;
	BITMASK  operands;

	/********************************************************************/
	/*      The flow of this routine is somewhat convoluted because     */
	/*      some of the pipeline is being mimicked and                  */
	/*      the printing of the I/O (show != 0) is for human consumption*/
	/********************************************************************/

	for( ; l; l = l->next )
	{
		if( show )
			printf( "%s:\nclock source  FU   dest\n", l->symbol->spelling );
		clock      = 1;
		init_write_slot();
		i = choose_next( prev = (Inst *)&l->inst, scoreboard, fctn_busy, reorder );
		while( i )
		{
			operands = i->src[0] | i->dest[0] | i->dest[1] ;
			if( show )
				printf( Format, clock, scoreboard, fctn_busy );
			if( ( op = isopcode( i ) ) == 0 )
			{       /* dangerous instruction        */
				/* wait for scoreboard to clear */
				if( scoreboard != 0 )
					i = 0;          /* do not issue */
				else    prev = i;       /* do     issue */
			}
			else if( ( scoreboard & operands   ||   /* scoreboard fails */
				   fctn_busy  & op->unit   ) )  /* of unit is busy  */
				i = 0;                  /* do not issue */
			else
			{       /* issue instruction */
				if( op->latency )
				{       /* scoreboarded instruction */
					if( i->src[1] )
					{       /* takes two operand cycles */
						while( scoreboard & i->src[1] )
						{       /* wait until second operands arrive */
							use_write_slot( 0, show );
							if( show )
								printf( Format, clock, scoreboard, fctn_busy );
						}
						use_write_slot( i, show );
						if( show )
							printf( Format, clock, scoreboard, fctn_busy );
					}
					/* insert write slot request(s)          */
					/* busy is the number of pipeline stalls */
					/* latency is the length of the pipe     */
					target = write_slot + op->busy + op->latency - 1;
					new_write_slot( i->dest[0], i, op, target );
					if( i->dest[1] )
						new_write_slot( i->dest[1], i, op, target + 1 );
				}
				else    /* non-scoreboarded */
				{
					if( modifies_register( op ) )
						steal_write_slot( i->dest[0], op->unit );
					if( store_double( op ) )
					{       /* steal second core cycle */
						use_write_slot( i, show );
						if( show )
							printf("%4d -------- --", clock );
						steal_write_slot( i->dest[1], op->unit );
					}
					if( calls( op ) )
						subroutine( i, op->busy );
				}
				prev = i;               /* do     issue */
			}
			/****************************************************/
			/*      if( i == 0 ) instruction is not issued      */
			/*                   and prev is not updated        */
			/****************************************************/
			use_write_slot( i, show );
			if( prev != (Inst *)&l->inst &&   /* prev exists    */
			    branches( prev )         &&   /* and            */
			    ! delayed( prev )           ) /* delayed branch */
				i = prev->next;         /* choose delay slot*/
			else    i = choose_next( prev, scoreboard, fctn_busy, reorder );
		}
	}
}

