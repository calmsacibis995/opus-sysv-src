/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)sdb:com/optab.c	1.12"

#include	"head.h"

/*
 *  	Instruction printing data.
 *		MACHINE DEPENDENT
 *
 */

/* The registers in regname must be in the correct order, because the numeric
 * value of the register is used (in dis/bits.c) to index into this table.
 * The null final entry is a sentinel, used in prassym (opset.c).
 *
 * The registers in reglist must be in correct order, because the
 * number of the register is used to index into this table, to get roffs,
 * which gives the offset in the user area from register 0.
 */

#if u3b

STRING	regname[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
			"ap", "fp", "sp", "pc", "psbr", "ssbr", (char *)0};

REGLIST reglist [NUMREGLS] = {
		"r0", R0,
		"r1", R1,
		"r2", R2,
		"r3", R3,
		"r4", R4,
		"r5", R5,
		"r6", R6,
		"r7", R7,
		"r8", R8,
		"fp", FP,
		"ap", AP,
		"sp", SP,
		"pc", PC,
		"psw", PS,
		"psbr", PSBR,
		"ssbr", SSBR,
/*		"u_rval1", VAL1,	not register; removed */
/*		"u_rval2", VAL2,	not register; removed */
};
#endif

#if vax
STRING	regname[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
					"r8", "r9", "r10","r11","ap", "fp", "sp", "pc", (char *)0};

REGLIST reglist [NUMREGLS] = {
		"r0", R0,
		"r1", R1,
		"r2", R2,
		"r3", R3,
		"r4", R4,
		"r5", R5,
		"r6", R6,
		"r7", R7,
		"r8", R8,
		"r9", R9,
		"r10", R10,
		"r11", R11,
		"ap", AP,
		"fp", FP,
		"sp", SP,
		"pc", PC,
		"ps", PS,
};
#endif

#if clipper
STRING	regname[] = {
	"r0", "r1", "r2",  "r3",  "r4",  "r5",  "r6", "r7",
	"r8", "r9", "r10", "r11", "r12", "r13", "fp", "sp",
	"psw","ssw","f0", "f1", "f2",  "f3",  "f4",  "f5",
	"f6", "f7", "pc", (char *)0
};

REGLIST reglist[NUMREGLS] = {
	{"r0",  R0},
	{"r1",  R1},
	{"r2",  R2},
	{"r3",  R3},
	{"r4",  R4},
	{"r5",  R5},
	{"r6",  R6},
	{"r7",  R7},
	{"r8",  R8},
	{"r9",  R9},
	{"r10", R10},
	{"r11", R11},
	{"r12", R12},
	{"r13", R13},
	{"fp",  FP},
	{"sp",  SP},
	{"ssw", SS},
	{"psw", PS},
	{"f0",  F0},
	{"f0x", F0x},
	{"f1",  F1},
	{"f1x", F1x},
	{"f2",  F2},
	{"f2x", F2x},
	{"f3",  F3},
	{"f3x", F3x},
	{"f4",  F4},
	{"f4x", F4x},
	{"f5",  F5},
	{"f5x", F5x}, 
	{"f6",  F6},
	{"f6x", F6x},
	{"f7",  F7},
	{"f7x", F7x},
	{"pc",  PC},
};
#endif

#if m88k
STRING	regname[] = {
	"r0", "r1", "r2",  "r3",  "r4",  "r5",  "r6", "r7",
	"r8", "r9", "r10", "r11", "r12", "r13", "f14", "s15",
	"r16", "r17", "r18",  "r19",  "r20",  "r21",  "r22", "r23",
	"r24", "r25", "r26",  "r27",  "r28",  "r29",  "r30", "r31",
	"sr", "scip", "snip",  "sfip",
	(char *)0
};

REGLIST reglist[NUMREGLS] = {
	{"r0",  R0},
	{"r1",  R1},
	{"r2",  R2},
	{"r3",  R3},
	{"r4",  R4},
	{"r5",  R5},
	{"r6",  R6},
	{"r7",  R7},
	{"r8",  R8},
	{"r9",  R9},
	{"r10", R10},
	{"r11", R11},
	{"r12", R12},
	{"r13", R13},
	{"r14", R14},
	{"r15", R15},
	{"r16", R16},
	{"r17", R17},
	{"r18", R18},
	{"r19", R19},
	{"r20", R20},
	{"r21", R21},
	{"r22", R22},
	{"r23", R23},
	{"r24", R24},
	{"r25", R25},
	{"r26", R26},
	{"r27", R27},
	{"r28", R28},
	{"r29", R29},
	{"r30", R30},
	{"r31", R31},
	{"sr",  SR},
	{"scip",SCIP},
	{"snip",SNIP},
	{"sfip",SFIP}
};
#endif

#if ns32000
STRING	regname[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
					"fp", "sp", "pc", (char *)0};

REGLIST reglist [NUMREGLS] = {
		"r0", R0,
		"r1", R1,
		"r2", R2,
		"r3", R3,
		"r4", R4,
		"r5", R5,
		"r6", R6,
		"r7", R7,
		"fp", FP,
		"sp", SP,
		"pc", PC,
		"ps", PS,
};
#endif


#if u3b5 || u3b15 || u3b2
STRING regname[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
		    "fp", "ap", "psw", "sp", "pcbp", "isp", "pc", "x0", "x1",
			"x2", "x3", "asr", "dr"};

REGLIST reglist [NUMREGLS] = {
		"r0",	R0,
		"r1",	R1,
		"r2",	R2,
		"r3",	R3,
		"r4",	R4,
		"r5",	R5,
		"r6",	R6,
		"r7",	R7,
		"r8",	R8,
		"fp",	FP,
		"ap",	AP,
		"psw",	PS,
		"sp",	SP,
		"pcbp",	0x80000000, /* pcbp not available through ptrace */
		"isp",	0x80000000, /* isp not available  through ptrace */
		"pc",	PC,
};
#endif

#ifndef clipper
STRING	fltimm[] = {
"0.5", "0.5625", "0.625", "0.6875", "0.75", "0.8125", "0.875", "0.9375",
"1.0", "1.125", "1.25", "1.375", "1.5", "1.625", "1.75", "1.875",
"2.0", "2.25", "2.5", "2.75", "3.0", "3.25", "3.5", "3.75",
"4.0", "4.5", "5.0", "5.5", "6.0", "6.5", "7.0", "7.5",
"8.0", "9.0", "10.0", "11.0", "12.0", "13.0", "14.0", "15.0",
"16.0", "18.0", "20.0", "22.0", "24.0", "26.0", "28.0", "30.0",
"32.0", "36.0", "40.0", "44.0", "48.0", "52.0", "56.0", "60.0",
"64.0", "72.0", "80.0", "88.0", "96.0", "104.0", "112.0", "120.0"
};
#endif /* clipper */

char *fmtr = {"%r"};
char *fmtR = {"%R"};
