#ident "@(#) $Header: table.c 4.1 88/04/26 17:22:18 root Exp $ RISC Source Base"
/* BEWARE--some code is dependent on all operators in the table being lower */
/****************************************************************************/
/*      The three fields{1,2,3} are used to parse and arrange the actual    */
/*              operand fields with the registers--                         */
/****************************************************************************/
/*      for each field                                                      */
/*              if( not_used )          then all done                       */
/*              else if( not_register ) then copy from field i->spelling    */
/*              else                         use register r%d from i->regs[]*/
/****************************************************************************/
/*      This provides for things like --'stcr r1,crn' to know that the      */
/*      r1 register is actually the s1 operand                              */
/****************************************************************************/
/*	10/01/87	WWV7	modified last entry in opcode table for     */
/*				ld.d from 4 to 2 			    */
/****************************************************************************/
/*      The Text and algorithms contained herein are the sole unpublished   */
/*      property of Motorola Inc.  Copyright  1987  All Rights Reserved     */
/****************************************************************************/

# include "types.h"

# define NR (NOT_REG)           /* character compression */
# define NU (NOT_USED)
# define CND (CONDITION)
# define LBL (LABEL_FIELD)
# define CTL (CONTROL_REG)
# define VEC (VECTOR)

static
struct  OPCODE_TABLE
	optable[] =
{
/*                               regs[1] regs[2]  regs[0]                   */
/*          format               FctnUnit Busy Src1 Src2  DstSRC DstWRT  Latency    */
/*              field1                                  check                       */
/*                  field2                      |--- scoreboard ----|               */
/*                      field3                                                      */
"add",      NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"add.ci",   NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"add.co",   NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"add.cio",  NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"addu",     NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"addu.ci",  NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"addu.co",  NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"addu.cio", NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"and",      NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"and.c",    NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"and.u",    NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"bb0",         C, CND,  1,LBL,      1,      1,   1,   0,     0,     0,      0,
"bb0.n",       C, CND,  1,LBL,      1,      0,   1,   0,     0,     0,      0,
"bb1",         C, CND,  1,LBL,      1,      1,   1,   0,     0,     0,      0,
"bb1.n",       C, CND,  1,LBL,      1,      0,   1,   0,     0,     0,      0,
"bcnd",        C, CND,  1,LBL,      1,      1,   1,   0,     0,     0,      0,
"bcnd.n",      C, CND,  1,LBL,      1,      0,   1,   0,     0,     0,      0,
"br",          L, LBL, NU, NU,      1,      1,   0,   0,     0,     0,      0,
"br.n",        L, LBL, NU, NU,      1,      0,   0,   0,     0,     0,      0,
"bsr",     SUB+L, LBL, NU, NU,      1,      1,   0,   0,     0,     1,      0,   /* SUB creates r1 scoreboard */
"bsr.n",   SUB+L, LBL, NU, NU,      1,      0,   0,   0,     0,     1,      0,   /* SUB creates r1 scoreboard */
"cmp",      NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"clr",      NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"div",      NORM,   0,  1,  2,      4,     32,   1,   1,     1,     1,      5,
"divu",     NORM,   0,  1,  2,      4,     32,   1,   1,     1,     1,      5,
"ext",      NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"extu",     NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"fadd.sss", NORM,   0,  1,  2,      4,      0,   1,   1,     1,     1,      5,
"fadd.ssd", NORM,   0,  1,  2,      4,      0,   1,   3,     1,     1,      5,
"fadd.sds", NORM,   0,  1,  2,      4,      0,   3,   1,     1,     1,      5,
"fadd.sdd", NORM,   0,  1,  2,      4,      0,   3,   3,     1,     1,      5,
"fadd.dss", NORM,   0,  1,  2,      4,      0,   1,   1,     3,     3,      5,
"fadd.dsd", NORM,   0,  1,  2,      4,      0,   1,   3,     3,     3,      5,
"fadd.dds", NORM,   0,  1,  2,      4,      0,   3,   1,     3,     3,      5,
"fadd.ddd", NORM,   0,  1,  2,      4,      0,   3,   3,     3,     3,      5,
"fcmp.sss", NORM,   0,  1,  2,      4,      0,   1,   1,     1,     1,      5,
"fcmp.ssd", NORM,   0,  1,  2,      4,      0,   1,   3,     1,     1,      5,
"fcmp.sds", NORM,   0,  1,  2,      4,      0,   3,   1,     1,     1,      5,
"fcmp.sdd", NORM,   0,  1,  2,      4,      0,   3,   3,     1,     1,      5,
"fdiv.sss", NORM,   0,  1,  2,      4,     27,   1,   1,     1,     1,      5,
"fdiv.ssd", NORM,   0,  1,  2,      4,     27,   1,   3,     1,     1,      5,
"fdiv.sds", NORM,   0,  1,  2,      4,     27,   3,   1,     1,     1,      5,
"fdiv.sdd", NORM,   0,  1,  2,      4,     27,   3,   3,     1,     1,      5,
"fdiv.dss", NORM,   0,  1,  2,      4,     56,   1,   1,     3,     3,      5,
"fdiv.dsd", NORM,   0,  1,  2,      4,     56,   1,   3,     3,     3,      5,
"fdiv.dds", NORM,   0,  1,  2,      4,     56,   3,   1,     3,     3,      5,
"fdiv.ddd", NORM,   0,  1,  2,      4,     56,   3,   3,     3,     3,      5,
"flt.ss",   NORM,   0,  1, NU,      4,      0,   1,   0,     1,     1,      5,
"flt.ds",   NORM,   0,  1, NU,      4,      0,   1,   0,     3,     3,      5,
"fldcr",    NORM,   0,CTL, NU,      1,      0,   0,   0,     1,     1,      5,
"fmul.sss", NORM,   0,  1,  2,      8,      0,   1,   1,     1,     1,      6,
"fmul.ssd", NORM,   0,  1,  2,      8,      2,   1,   3,     1,     1,      6,
"fmul.sds", NORM,   0,  1,  2,      8,      2,   3,   1,     1,     1,      6,
"fmul.sdd", NORM,   0,  1,  2,      8,      4,   3,   3,     1,     1,      6,
"fmul.dss", NORM,   0,  1,  2,      8,      0,   1,   1,     3,     3,      6,
"fmul.dsd", NORM,   0,  1,  2,      8,      2,   1,   3,     3,     3,      6,
"fmul.dds", NORM,   0,  1,  2,      8,      2,   3,   1,     3,     3,      6,
"fmul.ddd", NORM,   0,  1,  2,      8,      4,   3,   3,     3,     3,      6,
"fstcr",   STORE,   1,CTL, NU,      1,      0,   1,   0,     0,     0,      0,
"fsub.sss", NORM,   0,  1,  2,      4,      0,   1,   1,     1,     1,      5,
"fsub.ssd", NORM,   0,  1,  2,      4,      0,   1,   3,     1,     1,      5,
"fsub.sds", NORM,   0,  1,  2,      4,      0,   3,   1,     1,     1,      5,
"fsub.sdd", NORM,   0,  1,  2,      4,      0,   3,   3,     1,     1,      5,
"fsub.dss", NORM,   0,  1,  2,      4,      0,   1,   1,     3,     3,      5,
"fsub.dsd", NORM,   0,  1,  2,      4,      0,   1,   3,     3,     3,      5,
"fsub.dds", NORM,   0,  1,  2,      4,      0,   3,   1,     3,     3,      5,
"fsub.ddd", NORM,   0,  1,  2,      4,      0,   3,   3,     3,     3,      5,
"fxcr",     NORM,   0,  1, NU,      1,      0,   1,   0,     1,     1,      0,
"ff0",      NORM,   0,  1, NU,      1,      0,   1,   0,     1,     1,      0,
"ff1",      NORM,   0,  1, NU,      1,      0,   1,   0,     1,     1,      0,
"int.sd",   NORM,   0,  1, NU,      4,      0,   3,   0,     1,     1,      5,
"int.ss",   NORM,   0,  1, NU,      4,      0,   1,   0,     1,     1,      5,
"jmp",       JMP,   1, NU, NU,      1,      1,   1,   0,     0,     0,      0,
"jmp.n",     JMP,   1, NU, NU,      1,      0,   1,   0,     0,     0,      0,
"jsr",   JMP+SUB,   1, NU, NU,      1,      1,   1,   0,     0,     1,      0,   /* SUB creates r1 scoreboard */
"jsr.n", JMP+SUB,   1, NU, NU,      1,      0,   1,   0,     0,     1,      0,   /* SUB creates r1 scoreboard */
"ldcr",     NORM,   0,CTL, NU,      1,      0,   0,   0,     1,     1,      0,
"ld.b",     LOAD,   0,  1,  2,     16,      0,   1,   1,     1,     1,      3,
"ld.bu",    LOAD,   0,  1,  2,     16,      0,   1,   1,     1,     1,      3,
"ld.h",     LOAD,   0,  1,  2,     16,      0,   1,   1,     1,     1,      3,
"ld.hu",    LOAD,   0,  1,  2,     16,      0,   1,   1,     1,     1,      3,
"ld",       LOAD,   0,  1,  2,     16,      0,   1,   1,     1,     1,      3,
"ld.d",     LOAD,   0,  1,  2,     16,      1,   1,   1,     3,     3,      2,
"lda.b",    NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"lda.h",    NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"lda",      NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"lda.d",    NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"mak",      NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"mask",     NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"mask.u",   NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"mul",      NORM,   0,  1,  2,      2,      0,   1,   1,     1,     1,      4,
"nint.ss",  NORM,   0,  1, NU,      4,      0,   1,   0,     1,     1,      5,
"nint.sd",  NORM,   0,  1, NU,      4,      0,   3,   0,     1,     1,      5,
"or",       NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"or.c",     NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"or.u",     NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"set",      NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"stcr",    STORE,   1,CTL, NU,      1,      0,   1,   0,     0,     0,      0,
"st.b",    STORE,   0,  1,  2,     17,      0,   1,   1,     1,     1,      0,
"st.h",    STORE,   0,  1,  2,     17,      0,   1,   1,     1,     1,      0,
"st",      STORE,   0,  1,  2,     17,      0,   1,   1,     1,     1,      0,
"st.d",    STORE,   0,  1,  2,     17,      1,   1,   1,     3,     3,      0,
"sub",      NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"sub.ci",   NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"sub.co",   NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"sub.cio",  NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"subu",     NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"subu.ci",  NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"subu.co",  NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"subu.cio", NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"trnc.ss",  NORM,   0,  1, NU,      4,      0,   1,   0,     1,     1,      5,
"trnc.sd",  NORM,   0,  1, NU,      4,      0,   3,   0,     1,     1,      5,
"rot",      NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"rte",         0,  NU, NU, NU,      1,      0,   0,   0,     0,     0,      0,
"tb0",         C, CND,  1,VEC,      1,      0,   1,   0,     0,     0,      0,
"tb1",         C,  NR,  1,VEC,      1,      0,   1,   0,     0,     0,      0,
"tbnd",        C,  NR,  1,VEC,      1,      0,   1,   0,     0,     0,      0,
"xcr",      NORM,   0,  1,CTL,      1,      0,   1,   0,     1,     1,      0,
"xmem.b",   NORM,   0,  1,  2,     16,      1,   1,   1,     1,     1,      3,
"xmem",     NORM,   0,  1,  2,     16,      1,   1,   1,     1,     1,      3,
"xor",      NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"xor.c",    NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"xor.u",    NORM,   0,  1,  2,      1,      0,   1,   1,     1,     1,      0,
"",            0,   0,  1,  2,      0,      0,   0,   0,     0,     0,      0,
};

static  char    *reg_names[]  = {       "r0",   "r1",   "r2",   "r3",
					"r4",   "r5",   "r6",   "r7",
					"r8",   "r9",  "r10",  "r11",
				       "r12",  "r13",  "r14",  "r15",
				       "r16",  "r17",  "r18",  "r19",
				       "r20",  "r21",  "r22",  "r23",
				       "r24",  "r25",  "r26",  "r27",
				       "r28",  "r29",  "r30",  "r31",
				       "",
				};

static  char    *conditions[] = {       "eq",   "eq0",  "ne",   "ne0",
					"ge",   "ge0",  "gt",   "gt0",
					"le",   "le0",  "lt",   "lt0",
					"hs",           "hi",
					"ls",           "lo",
					"1<0>", "1<1>", "1<2>", "1<3>",
					"1<4>", "1<5>", "1<6>", "1<7>",
					"1<8>", "1<9>", "1<10>","1<11>",
					"1<12>","1<13>","1<14>","1<15>",
					"1<16>","1<17>","1<18>","1<19>",
					"1<20>","1<21>","1<22>","1<23>",
					"1<24>","1<25>","1<26>","1<27>",
					"1<28>","1<29>","1<30>","1<31>",
					"",
				};

/****************************************************************************/
/* statically allocated symbols area--this provides a mechanism to switch   */
/* from one symbol to another related symbol by knowing the 'distance'      */
/* between the current and the 'to-be' object.  br -> br.n by i->opcode++   */
/*                                              r7 -> r10  by i->operand+3  */
/****************************************************************************/

Symbol  instructions[ sizeof( optable )   / sizeof( optable[0] ) +
		      sizeof( reg_names ) / sizeof( reg_names[0] ) +
		      sizeof( conditions )/ sizeof( conditions[0] ) ];

char    _char_table[257];              /* _char_table[ EOF + 1 ] must exist */

Symbol  *extu,                          /* fast check for extu instruction  */
	*eq0,
	*ne0,
	*gt0,
	*ge0,
	*lt0,
	*le0,
	*bcnd,
	*bb0,
	*bb1,
	*register_0,
	*register_1,
	*frame_pointer,
	*stack_pointer,
	*last_instruction = &instructions[ sizeof( instructions ) /
					   sizeof( instructions[0] ) ],
	*lookup();

void
install(a,c)
char    *c;
{
	char    *p;
	for( p=c ; *p ; p++ )
		_char_table[*p+1] |= a;
}

void
main(    argc, argv )
unsigned argc;
char          *argv[];
{
	Opcode  *op;
	Symbol  *s = &instructions[0];
	char    c;

	/*      Have Options List Processed                         */
	options( argc, argv );

	/*      Install character parsing/catagorization data       */
	_char_table[ EOF + 1 ] = TERM;  /* BEWARE machine dependent */
	_char_table[ NULL+ 1 ] = TERM|DELIM;
	install( DELIM,  "\n\t ,[]" );
	install( TERM,   "\n" );
	install( SEMI,   ";" );
	install( WHITE,  "\t " );
	install( LOWER,  "abcdefghijklmnopqrstuvwxyz" );
	install( STARTS, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_@$" );
	install( CONT,   "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789." );
	install( DIGIT,  "0123456789" );

	/*      Install the instructions into the table             */
	for( op = optable; c = *op->spelling; op++ )
	{       /* insert into symbol table */
		insert( op->spelling, INSTRUCTION, s );
		/* and this symbol to the opcode table */
		s->def.opcode = op;
		s++;
	}

	/*      Install the register names into the table           */
	for( c = 0; *reg_names[c]; c++ )
	{
		insert( reg_names[c], REGISTER, s );
		s->def.value = c;
		s++;
	}

	/*      Install the branch on condition                    */
	for( c = 0; *conditions[c]; c++ )
		insert( conditions[c], CONDITION, s++ );

	/*      Setup some useful fast checks                      */
	extu          = lookup( "extu", INSTRUCTION );
	bcnd          = lookup( "bcnd", INSTRUCTION );
	bb0           = lookup( "bb0",  INSTRUCTION );
	bb1           = lookup( "bb1",  INSTRUCTION );
	eq0           = lookup( "eq0",  CONDITION   );
	ne0           = lookup( "ne0",  CONDITION   );
	gt0           = lookup( "gt0",  CONDITION   );
	ge0           = lookup( "ge0",  CONDITION   );
	lt0           = lookup( "lt0",  CONDITION   );
	le0           = lookup( "le0",  CONDITION   );
	register_0    = lookup( "r0",   REGISTER    );
	register_1    = lookup( "r1",   REGISTER    );
	stack_pointer = lookup( "r31",  REGISTER    );
	frame_pointer = lookup( "r30",  REGISTER    );

	/*      Start Processing In Data Section                    */
	data();

	/*      Leave without Complaint                             */
	exit( 0 );
}

void
error( p )
{       /*      Cannot Continue--Say Something and Die              */
	printf( p );
	exit(1);
}
