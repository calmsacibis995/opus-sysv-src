
static char copyright11[] = "Copyright (c) Motorola, Inc. 1985,1986,1987,1988";

#include "sim.h"

/*  Constants and Masks */

#define SFU0       0x80000000
#define SFU1       0x84000000
#define SFU7       0x9c000000
#define RRI10      0xf0000000
#define RRR        0xf4000000
#define SFUMASK    0xfc00ffe0
#define RRRMASK    0xfc00ffe0
#define RRI10MASK  0xfc00fc00
#define DEFMASK    0xfc000000
#define CTRL       0x0000f000
#define CTRLMASK   0xfc00f800

/* Operands types */

#define HEX          1
#define REG          2
#define IND          3
#define CONT         3
#define IND          3
#define BF           4
#define REGSC        5    /* scaled register */
#define CRREG        6    /* control register */
#define FCRREG       7    /* floating point control register */

/* Hashing Specification */

#define HASHVAL     79

/* Type definitions */

typedef unsigned int UINT;

/* Structure templates */

typedef struct {
   unsigned int offset:5;
   unsigned int width:6;
   unsigned int type:5;
} OPSPEC;

	struct SIM_FLAGS {
	      int  ltncy,   /* latency (max number of clocks needed to execute) */
		  extime,   /* execution time (min number of clocks needed to execute) */
		  wb_pri;   /* writeback slot priority */
   unsigned long   op:OP,   /* simulator version of opcode */
	     imm_flags:2,   /* 10,16 or 26 bit immediate flags */
	      rs1_used:1,   /* register source 1 used */
	      rs2_used:1,   /* register source 2 used */
	      rsd_used:1,   /* register source/dest used */
		c_flag:1,   /* complement */
		u_flag:1,   /* upper half word */
		n_flag:1,   /* execute next */
	       wb_flag:1,   /* uses writeback slot */
	       dest_64:1,   /* double precision dest */
		 s1_64:1,   /* double precision source 1 */
		 s2_64:1,   /* double precision source 2 */
	    scale_flag:1;   /* register is scaled */
};

typedef struct INSTRUCTAB {
   unsigned int  opcode;
   char          *mnemonic;
   OPSPEC        op1,op2,op3;
   struct SIM_FLAGS flgs;
   struct INSTRUCTAB    *next;
} INSTAB;
