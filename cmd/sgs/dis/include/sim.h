
static char copyright15[] = "Copyright (c) Motorola, Inc. 1986";


/*
 * This file contains the structures and constants needed to build the M78000
 * M78000 simulator.  It is the main include file, containing all the
 * structures, macros and definitions except for the floating point
 * instruction set.
 */

/*
 * The following flag informs the Simulator as to what type of byte ordering
 * will be used. For instance, a BOFLAG = 1 indicates a DEC VAX and IBM type
 * of ordering shall be used.
*/

/* # define     BOFLAG   1                       /* BYTE ORDERING FLAG */

/* define the number of bits in the primary opcode field of the instruction,
 * the destination field, the source 1 and source 2 fields.
 */
# define    OP       8                        /* size of opcode field */ 
# define    DEST     6                        /* size of destination  */
# define    SOURCE1  6                        /* size of source1      */
# define    SOURCE2  6                        /* size of source2      */

# define    REGs    32                        /* number of registers  */

# define    WORD    long
# define    FLAG    unsigned
# define    STATE   short 

# define    TRUE     1
# define    FALSE    0

# define    READ     0
# define    WRITE    1

/* The next four equates define the priorities that the various classes
 * of instructions have regarding writing results back into registers and
 * signalling exceptions.
 */

# define    PINT  0   /* Integer Priority */
# define    PFLT  1   /* Floating Point Priority */
# define    PMEM  2   /* Memory Priority */
# define    NA    3   /* Not Applicable, instruction doesnt write to regs */
# define    HIPRI 3   /* highest of these priorities */

/* The instruction registers are an artificial mechanism to speed up
 * simulator execution.  In the real processor, an instruction register
 * is 32 bits wide.  In the simulator, the 32 bit instruction is kept in
 * a structure field called rawop, and the instruction is partially decoded,
 * and split into various fields and flags which make up the other fields
 * of the structure.
 * The partial decode is done when the instructions are initially loaded
 * into simulator memory.  The simulator code memory is not an array of
 * 32 bit words, but is an array of instruction register structures.
 * Yes this wastes memory, but it executes much quicker.
 */

struct IR_FIELDS {
		    unsigned long   op:OP,
				    dest: DEST,
				    src1: SOURCE1,
				    src2: SOURCE2;
			      int   ltncy,
				    extime,
				    wb_pri;     /* writeback priority     */
		    unsigned short  imm_flags:2,/* immediate size         */
				    rs1_used:1, /* register source 1 used */
				    rs2_used:1, /* register source 2 used */
				    rsd_used:1, /* register source/dest. used */
				    c_flag:1,   /* complement      */
				    u_flag:1,   /* upper half word */
				    n_flag:1,   /* execute next    */
				    wb_flag:1,  /* uses writeback slot */
				    dest_64:1,  /* dest size       */
				    s1_64:1,    /* source 1 size   */
				    s2_64:1,    /* source 2 size   */
				    scale_flag:1, /* scaled register */
				    brk_flg:1;
                 };

struct	mem_segs {
	struct mem_wrd *seg;			/* pointer (returned by calloc) to segment */
	unsigned long baseaddr;			/* base load address from file headers */
	unsigned long endaddr;			/* Ending address of segment */
	int	      flags;			/* segment control flags (none defined 12/5/86) */
};

#define	MAXSEGS		(10)			/* max number of segment allowed */
#define	MEMSEGSIZE	(sizeof(struct mem_segs))/* size of mem_segs structure */


#define BRK_RD		(0x01)			/* break on memory read */
#define BRK_WR		(0x02)			/* break on memory write */
#define BRK_EXEC	(0x04)			/* break on execution */
#define	BRK_CNT		(0x08)			/* break on terminal count */


struct	mem_wrd {
	struct IR_FIELDS opcode;		/* simulator instruction break down */
	union {
		unsigned long  l;		/* memory element break down */
		unsigned short s[2];
		unsigned char  c[4];
	} mem;
};

#define	MEMWRDSIZE	(sizeof(struct mem_wrd))	/* size of each 32 bit memory model */

/* External declarations */

extern	struct mem_segs memory[];
extern	struct PROCESSOR m78000;

struct  PROCESSOR   {
	     unsigned WORD
			    ip,          /* execute instruction pointer */
			    vbr,         /* vector base register */
			    psr;         /* processor status register */

		    WORD    S1bus, /* source 1 */
                            S2bus, /* source 2 */
                            Dbus,  /* destination */
			    DAbus, /* data address bus */
                            ALU,
			    Regs[REGs],       /* data registers */
			    time_left[REGs],  /* max clocks before reg is available */
			    wb_pri[REGs],     /* writeback priority of reg */
			    SFU0_regs[REGs],  /* integer unit control regs */
			    SFU1_regs[REGs],  /* floating point control regs */
			    Scoreboard[REGs],
			    Vbr;
	    unsigned WORD   scoreboard,
			    Psw,
			    Tpsw;
		    FLAG   jump_pending:1;   /* waiting for a jump instr. */
                    };

# define    i26bit      1    /* size of immediate field */
# define    i16bit      2
# define    i10bit      3

/* Definitions for fields in psr */

# define mode  31
# define rbo   30
# define ser   29
# define carry 28
# define sf7m  11
# define sf6m  10
# define sf5m   9
# define sf4m   8
# define sf3m   7
# define sf2m   6
# define sf1m   5
# define mam    4
# define inm    3
# define exm    2
# define trm    1
# define ovfm   0

#define	    MODEMASK   (1<<(mode-1))
# define    SILENT     0   /* simulate without output to crt */
# define    VERBOSE    1   /* simulate in verbose mode */
# define    PR_INSTR   2   /* only print instructions */

# define    RESET      16 /* reset phase */

# define    PHASE1     0  /* data path phases */
# define    PHASE2     1

/* the 1 clock operations */

# define    ADDU        1
# define    ADDC        2
# define    ADDUC       3
# define    ADD         4

# define    SUBU    ADD+1
# define    SUBB    ADD+2
# define    SUBUB   ADD+3
# define    SUB     ADD+4

# define    AND     ADD+5
# define    OR      ADD+6
# define    XOR     ADD+7
# define    CMP     ADD+8
                        
/* the LOADS */

# define    LDAB    CMP+1
# define    LDAH    CMP+2
# define    LDA     CMP+3
# define    LDAD    CMP+4

# define    LDB   LDAD+1
# define    LDH   LDAD+2
# define    LD    LDAD+3
# define    LDD   LDAD+4
# define    LDBU  LDAD+5
# define    LDHU  LDAD+6

/* the STORES */

# define    STB    LDHU+1
# define    STH    LDHU+2
# define    ST     LDHU+3
# define    STD    LDHU+4

/* the exchange */

# define    XMEMBU LDHU+5
# define    XMEM   LDHU+6

/* the branches */
# define    JSR    STD+1
# define    BSR    STD+2
# define    BR     STD+3
# define    JMP    STD+4
# define    BB1    STD+5
# define    BB0    STD+6
# define    RTN    STD+7
# define    BCND   STD+8

/* the TRAPS */
# define    TB1    BCND+1
# define    TB0    BCND+2
# define    TCND   BCND+3
# define    RTE    BCND+4
# define    TBND   BCND+5

/* the MISC instructions */
# define    MUL     TBND + 1
# define    DIV     MUL  +2
# define    DIVU    MUL  +3
# define    MASK    MUL  +4
# define    FF0     MUL  +5
# define    FF1     MUL  +6
# define    CLR     MUL  +7
# define    SET     MUL  +8
# define    EXT     MUL  +9
# define    EXTU    MUL  +10
# define    MAK     MUL  +11
# define    ROT     MUL  +12

/* control register manipulations */

# define    LDCR    ROT  +1
# define    STCR    ROT  +2
# define    XCR     ROT  +3

# define    FLDCR    ROT  +4
# define    FSTCR    ROT  +5
# define    FXCR     ROT  +6


# define    NOP     XCR +1

/* floating point instructions */

# define    FADD    NOP +1
# define    FSUB    NOP +2
# define    FMUL    NOP +3
# define    FDIV    NOP +4
# define    FSQRT   NOP +5
# define    FCMP    NOP +6
# define    FIP     NOP +7
# define    FLT     NOP +8
# define    INT     NOP +9
# define    NINT    NOP +10
# define    TRNC    NOP +11
# define    FLDC   NOP +12
# define    FSTC   NOP +13
# define    FXC    NOP +14

# define    UEXT(src,off,wid)  ((((unsigned int)src)>>off) & ((1<<wid) - 1))
# define    SEXT(src,off,wid)  (((((int)src)<<(32-(off+wid))) >>(32-wid)) )
# define    MAKE(src,off,wid)  ((((unsigned int)src) & ((1<<wid) - 1)) << off)

# define opword(n) (unsigned long) (memaddr->mem.l)
