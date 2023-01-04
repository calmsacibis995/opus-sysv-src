/****************************************************************************
*
*       "dis.c"                Date Created: Nov. 17, 1986 - David L. Nolte
*		Modified for kdb Oct 5, 1987 Bruce D. Rosenkrantz
*
*/

/*
*	"instab.h"	Date Created: Nov. 17, 1986 David L. Nolte
*
*	This is the include file for the one line assembler/disembler 
*	package.
*
*/
#if AUSTINcc || clipper || ns32000 	/* the Austin CC has reversed bitfields */
union opcode
{
	long	opc;
	struct
	{
	 unsigned src2:5, opc2:11, src1:5, dest:5, opc1:6;
	} gen;
	struct
	{
	 unsigned src2:5, res:3, user:1, idx:1, ty:2, p:2, zero:2, src1:5, dest:5, fixed:6;
	} mem;
	struct
	{
	 unsigned imm16:16, src1:5, dest:5, ty:2, p:2, fixed:2;
	} memi;
	struct
	{
	 unsigned src2:5, crsd:6, sfu:3, opc:2, src1:5, dest:5, min:3, maj:3;
	} ctl;
	struct
	{
	 unsigned src2:5, td:2, t2:2, t1:2, opc:5, src1:5, dest:5, min:3, maj:3;
	} sfu1;
	struct
	{
	 unsigned src2:5, opc:11, src1:5, dest:5, min:3, maj:3;
	} rrr;
	struct
	{
	 unsigned opc:16, src1:5, dest:5, min:3, maj:3;
	} trpbfi;
	struct
	{
	 unsigned src2:5, zero2:5, nxt:1, opc:1, ignore:4, zero1:10, min:3, maj:3;
	} xfra;
	struct
	{
	 unsigned imm10:10, opc:6, src1:5, dest:5, min:3, maj:3;
	} imm10;
	struct
	{
	 unsigned imm16:16, src1:5, dest:5, min:3, maj:3;
	} imm16;
	struct
	{
	 unsigned imm26:26, min:3, maj:3;
	} imm26;
};

union rrropc
{
	unsigned long opc;
	struct
	{
	 unsigned zero:5, min:3, maj:3, ignore:5, pad:16;
	} gen;
	struct
	{
	 unsigned zero:3, user:1, idx:1, ty:2, p:2, ignore:7, pad:16;
	} mem;
	struct
	{
	 unsigned zero:5, ignore:11, pad:16;
	} tbndrte;
	struct
	{
	 unsigned zero:3, cbout:1, cbin:1, opc:3, ignore:8, pad:16;
	} intg;
	struct
	{
	 unsigned zero:5, cp:1, opc:2, ignore:8, pad:16;
	} log;
	struct
	{
	 unsigned zero:6, opc:3, ignore:7, pad:16;
	} bf;
	struct
	{
	 unsigned o:5, w:5, zero:1, opc:3, maj:2, pad:16;
	} bfi;
	struct
	{
	 unsigned vec:10, zero:1, opc:3, maj:2, pad:16;
	} trp;
};

#else
union opcode
{
	unsigned long	opc;
	struct
	{
	 unsigned opc1:6, dest:5, src1:5, opc2:11, src2:5;
	} gen;
	struct
	{
	 unsigned memfixed:6, dest:5, src1:5, zero:2, p:2, ty:2, idx:1, user:1, res:3, src2:5;
	} mem;
	struct
	{
	 unsigned memifixed:2, p:2, ty:2, dest:5, src1:5, imm16:16;
	} memi;
	struct
	{
	 unsigned ctlmaj:3, min:3, dest:5, src1:5, opc:2, sfu:3, crsd:6, src2:5;
	} ctl;
	struct
	{
	 unsigned sfu1maj:3, min:3, dest:5, src1:5, opc:5, t1:2, t2:2, td:2, src2:5;
	} sfu1;
	struct
	{
	 unsigned maj:3, min:3, dest:5, src1:5, opc:11, src2:5;
	} rrr;
	struct
	{
	 unsigned trpbfimaj:3, min:3, dest:5, src1:5, opc:16;
	} trpbfi;
	struct
	{
	 unsigned xframaj:3, min:3, zero1:10, ignore:4, opc:1, nxt:1, zero2:5, src2:5;
	} xfra;
	struct
	{
	 unsigned imm10maj:3, min:3, dest:5, src1:5, opc:6, imm10:10;
	} imm10;
	struct
	{
	 unsigned imm16maj:3, min:3, dest:5, src1:5, imm16:16;
	} imm16;
	struct
	{
	 unsigned imm26maj:3, min:3, imm26:26;
	} imm26;
};

union rrropc
{
	unsigned long opc;
	struct
	{
	 unsigned pad1:16, ignore:5, maj:3, min:3, zero:5;
	} gen;
	struct
	{
	 unsigned pad2:16, ignore:7, p:2, ty:2, idx:1, user:1, zero:3;
	} mem;
	struct
	{
	 unsigned pad3:16, ignore:11, zero:5;
	} tbndrte;
	struct
	{
	 unsigned pad4:16, ignore:8, opc:3, cbin:1, cbout:1, zero:3;
	} intg;
	struct
	{
	 unsigned pad5:16, ignore:8, opc:2, cp:1, zero:5;
	} log;
	struct
	{
	 unsigned pad6:16, ignore:7, opc:3, zero:6;
	} bf;
	struct
	{
	 unsigned pad7:16, maj:2, opc:3, zero:1, w:5, o:5;
	} bfi;
	struct
	{
	 unsigned pad8:16, maj:2, opc:3, zero:1, vec:10;
	} trp;
};
#endif

#include <stdio.h>
#include "sgs.h"


        /**********************************************************
	*
	*       Function Definition  (Local and External)
	*
	*/

extern	char *nameofbit();
extern	char *cndtype();
extern	void *extsympr();


	/**********************************************************
	*
	*       External Variables Definition
	*
	*/

extern long loc;
extern char mneu[];
extern long trace;

	/**********************************************************
	*
	*       Local Defines
	*
	*/



	/**********************************************************
	*
	*       Local Variables Definition
	*
	*/

char *rname[] = { "%r0", "%r1", "%r2", "%r3", "%r4", "%r5", "%r6",
	"%r7", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14",
	"%r15", "%r16", "%r17", "%r18", "%r19", "%r20", "%r21", "%r22",
	"%r23", "%r24", "%r25", "%r26", "%r27", "%r28", "%r29", "%r30",
	"%r31", "%r32" };
char	mnemonic[15] = { "\0" };
char	operands[30] = { "\0" };
int	memscaleu[] = { 1, 4, 2, 1 };
int	memscale[] = { 8, 4, 2, 1 };
union	opcode opcode;
static unsigned long dest, src1, src2;


	/**********************************************************
	*
	*      Messages
	*/

char null[] = { "" };
char *memnames[] = { "xmem", "ld", "st", "lda" };
char *memuser[] = { null, ".usr" };
char *memszu[] = { ".bu", null, ".hu", ".bu" };
char *memsz[] = { ".d", null, ".h", ".b" };
char *lognames[] = { "and", "mask", "xor", "or" };
char *intnames[] = { "addu", "subu", "divu", "mul", "add", "sub", "div", "cmp" };
char *intsuffix[] = { null, ".co", ".ci", ".cio" };
char *bitnames[] = { "clr", "set", "ext", "extu", "mak", "rot", null, null };
char *trpnames[] = { null, null, "tb0", "tb1", null, "tcnd" };
char *xfrnames[] = { "br", "bsr", "bb0", "bb1" };
char *ctlnames[] = { null, "ldcr", "stcr", "xcr" };
char *sfu1names[] = { "fmul", 0, 0, 0, "flt", "fadd", "fsub", "fcmp",
		      0, "int", "nint", "trnc", 0, 0, "fdiv", "fsrt",
		      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char *tynames[] = { null, "eq", "ne", "gt", "le", "lt",
		    "ge", "hi", "ls", "lo", "hs" };
char *bitpats[] = { null, "never", "gt0", "eq0", "ge0", "0100", "0101", "0110", "0111",
		    "1000", "1001", "1010", "1011", "lt0", "ne0", "le0", "always" };

	/**********************************************************
	*
	*      dis()
	*/

static char empty[] = "";

int
idis(opc)
unsigned long opc;
{
	int retvalue;

	opcode.opc = opc;
	dest = opcode.rrr.dest;
	src1 = opcode.rrr.src1;
	src2 = opcode.rrr.src2;

	if (trace) 
		printf("OPCODE={%x}\n",opc);

	switch(opcode.rrr.maj)
	{
		case	0:
		case	1:
			retvalue = memi();
			break;

		case	2:
			retvalue = logi();
			break;

		case	3:
			retvalue = inti();
			break;

		case	4:
			retvalue = sfu();
			break;

		case	6:
			retvalue = xfr();
			break;

		case	7:
			retvalue = rrr();
			break;

		default:
			retvalue = -1;
			break;
	}
	if (retvalue) {
		strcpy(mnemonic,";bad instr");
		sprintf(operands,"%8x",opc);
		fprintf(stderr,"\n%sdis:  illegal instruction (%x)\n",SGS,opc);
	}
	
	sprintf(mneu,"%-12.10s%-32.31s", mnemonic, operands);
	return(1);	/* kdb gets 1 if its done a disassembly */
}


memi()
{
	char	*memsize = memsz[opcode.memi.ty];

	switch(opcode.memi.p)
	{
		case	0:
			if(opcode.memi.ty & 0x2)
				opcode.memi.p = 1;
			memsize = memszu[opcode.memi.ty];
			break;

		case	1:
		case	2:
			break;

		case	3:
			return(-1);
	}
	sprintf(mnemonic, "%s%s", memnames[opcode.memi.p], memsize);
	sprintf(operands, "r%d,r%d,0x%04x", dest, src1, 
		opcode.imm16.imm16);
	return(0);
}


imm16()
{
	sprintf(operands, "r%d,r%d,0x%04x", dest, src1, opcode.imm16.imm16);
	return(0);
}

logi()
{
	sprintf(mnemonic, "%s%s", lognames[opcode.imm16.min >> 1], ((opcode.imm16.min & 0x1) ? ".u": null));
	return(imm16());
}

inti()
{
	sprintf(mnemonic, intnames[opcode.imm16.min]);
	return(imm16());
}

sfu()
{
	switch(opcode.rrr.min)
	{
		case	0:
			return(ctl());

		case	1:
			return(sfu1());

		case	2:
		case	3:
		case	4:
		case	5:
		case	6:
		case	7:
			break;
	}
	return(-1);
}

xfr()
{
	long immvalue;
	char *sym;
	char tstr[128];

	sym = tstr;
	sprintf(mnemonic, "%s%s", xfrnames[opcode.imm16.min >> 1], ((opcode.imm16.min & 0x1) ? ".n": null));
	if(opcode.imm16.min & 0x4)
	{
		immvalue = ((opcode.imm16.imm16 & 0x8000) ? 0xffff0000: 0) | opcode.imm16.imm16;
		extsympr(loc - 4 + (immvalue << 2), &sym);
		if (opcode.imm16.min & 4)
			sprintf(operands, "%d,r%d,%s", dest, src1, tstr);
		else
			sprintf(operands, "%s,r%d,%s", nameofbit(dest), src1, tstr);
	}
	else
	{
		immvalue = ((opcode.imm26.imm26 & 0x02000000) ? 0xfc000000: 0) | opcode.imm26.imm26;
		extsympr(loc - 4 + (immvalue << 2), &sym);
		sprintf(operands, "%s", tstr);
	}
	return(0);
}

rrr()
{
	long immvalue;
	char *sym;
	char tstr[128];

	sym = tstr;
	switch(opcode.rrr.min)
	{
		case	2:
		case	3:
			sprintf(mnemonic, "%s%s", "bcnd", ((opcode.imm16.min & 0x1) ? ".n": null));
			immvalue = ((opcode.imm16.imm16 & 0x8000) ? 0xffff0000: 0) | opcode.imm16.imm16;
			extsympr(loc - 4 + (immvalue << 2), &sym);
			sprintf(operands, "%s,r%d,%s", cndtype(dest), src1, tstr);
			return(0);

		case	4:
			return(gen1());

		case	5:
			return(gen2());

		case	6:
			if(dest)
				return(-1);
			sprintf(mnemonic, "tbnd");
			sprintf(operands, "r%d,0x%04x", src1, opcode.imm16.imm16);
			return(0);

		default:
			break;
	}
	return(-1);
}

ctl()
{
	char modifier;

	modifier = "\0f######"[opcode.ctl.sfu];
	if((opcode.ctl.opc == 0) || (modifier == '#'))
		return(-1);
	
	if(modifier)
		sprintf(mnemonic, "%c%s", modifier, ctlnames[opcode.ctl.opc]);
	else
		sprintf(mnemonic, ctlnames[opcode.ctl.opc]);

	switch(opcode.ctl.opc)
	{
		case	1:
			if(src1 || src2)
				return(-1);
			sprintf(operands, "r%d,cr%d", dest, opcode.ctl.crsd);
			break;

		case	2:
			if(dest)
				return(-1);
			sprintf(operands, "r%d,cr%d", src1, opcode.ctl.crsd);
			break;

		case	3:
			sprintf(operands, "r%d,r%d,cr%d", dest, src1, opcode.ctl.crsd);
			break;
	}
	return(0);
}

sfu1()
{
	char	td, t1, t2;

	td = "sd##"[opcode.sfu1.td];			/* td */
	t1 = "sd##"[opcode.sfu1.t1];			/* t1 */
	t2 = "sd##"[opcode.sfu1.t2];			/* t2 */

	if((td == '#') || (t1 == '#') || (t2 == '#') 
	   || (sfu1names[opcode.sfu1.opc] == 0))
		return(-1);

	switch(opcode.sfu1.opc)
	{
		case	0x7:
			if(td != 's')
				return(-1);
		case	0x0:
		case	0x5:
		case	0x6:
		case	0xe:
			sprintf(mnemonic, "%s.%c%c%c", sfu1names[opcode.sfu1.opc], td, t1, t2);
			sprintf(operands, "r%d,r%d,r%d", dest, src1, src2);
			return(0);

		case	0x4:
			if(t2 != 's')
				return(-1);
			break;

		case	0x9:
		case	0xa:
		case	0xb:
			if(td != 's')
				return(-1);
			break;

		default:
			return(-1);
	}
	if(src1 || (t1 != 's'))
		return(-1);
	sprintf(mnemonic, "%s.%c%c", sfu1names[opcode.sfu1.opc], td, t2);
	sprintf(operands, "r%d,r%d", dest, src2);
	return(0);
}

gen1()
{
	union rrropc opc;

	opc.opc = opcode.trpbfi.opc;

	if(opc.bfi.zero)
		return(-1);

	switch(opc.bfi.maj)
	{
		case	2:
			if(!bitnames[opc.bfi.opc])
				return(-1);
			sprintf(mnemonic, bitnames[opc.bfi.opc]);
			sprintf(operands, "r%d,r%d,%d<%d>", dest, src1, opc.bfi.w, opc.bfi.o);
			return(0);
			
		case	3:
			if(!trpnames[opc.trp.opc])
				return(-1);
			sprintf(mnemonic, trpnames[opc.trp.opc]);
			if (opc.trp.opc == 5)
				sprintf(operands, "%s,r%d,0x%03x",cndtype(dest),
					src1, opc.trp.vec);
			else
				sprintf(operands, "%d,r%d,0x%03x",dest,src1,opc.trp.vec);
			return(0);

		default:
			break;
	}
	return(-1);
}

gen2()
{
	char	*ptr;
	int	scale;
	union rrropc opc;

	opc.opc = opcode.rrr.opc;

	switch(opc.gen.maj)
	{
		case	0:				/* ld.xx & xmem.xx */
		case	1:				/* st.xx & lda.xx */
			ptr = memsz[opc.mem.ty];
			scale = memscale[opc.mem.ty];
			switch(opc.mem.p)
			{
				case	0:	/* xmem or unsigned load */
#if 1 /* DC */
					if(opc.mem.ty & 0x2)
						opc.mem.p = 1;	/* ld */
#else
					if((opc.mem.ty == 1) || (opc.mem.ty == 0))
						return(-1);
#endif
					ptr = memszu[opc.mem.ty];
					scale = memscaleu[opc.mem.ty];
					break;
		
				case	1:	/* signed load */
				case	2:	/* store */
#if 0
					/* byte mode scaled index */
					if((opc.mem.idx) && (opc.mem.ty == 3))
						return(-1);
#endif
					break;

				case	3:
					if((!opc.mem.idx) || (opc.mem.user))
						return(-1);
					break;
			}
			sprintf(mnemonic, "%s%s%s", memnames[opc.mem.p], ptr, memuser[opc.mem.user]);
			return(rrrmdisp(scale));

		case	2:
			if((opc.log.zero) || (opc.log.opc == 1))
				return(-1);
			sprintf(mnemonic, "%s%s", lognames[opc.log.opc], ((opc.log.cp) ? ".c": null));
			return(rrrdisp());

		case	3:
			if(opc.intg.zero)
				return(-1);
			switch(opc.intg.opc)
			{
#if 0
					if((opc.intg.cbin) || (opc.intg.cbout))
						return(-1);
					ptr = "";
					break;
#endif
				case	2:
				case	3:
				case	6:
				case	7:
					if(opc.intg.cbin)
						return(-1);
					ptr = "";
					break;

				default:
					ptr = intsuffix[(opc.intg.cbin << 1) + opc.intg.cbout];
					break;
			}
			sprintf(mnemonic, "%s%s", intnames[opc.intg.opc], ptr);
			return(rrrdisp());

		case	4:
		case	5:
			if(opc.bf.zero || !bitnames[opc.bf.opc])
				return(-1);
			sprintf(mnemonic, bitnames[opc.bf.opc]);
			return(rrrdisp());

		case	6:
			if(opcode.xfra.zero1 || opcode.xfra.zero2)
				return(-1);
			sprintf(mnemonic, "%s%s", (opcode.xfra.opc) ? "jsr": "jmp", (opcode.xfra.nxt) ? ".n": null);
			sprintf(operands, "r%d", src2);
			return(0);

		case	7:
			if(opc.gen.zero)
				return(-1);
			switch(opc.gen.min)
			{
				case	2:
				case	3:
					if(src1)
						return(-1);
					sprintf(mnemonic, (opc.log.cp) ? "ff0": "ff1");
					sprintf(operands, "r%d,r%d", dest, src2);
					return(0);

				case	6:
					if(dest)
						return(-1);
					sprintf(mnemonic, "tbnd");
					sprintf(operands, "r%d,r%d", src1, src2);
					return(0);
					
				case	7:
					if(opcode.xfra.zero1 || src2)
						return(-1);
					sprintf(mnemonic, "rte");
					return(operands[0] = '\0');

				default:
					break;
			}
		default:
			break;
	}
	return(-1);
}


rrrdisp()
{
	sprintf(operands, "r%d,r%d,r%d", dest, src1, src2);
	return(0);
}

rrrmdisp(scale)
int scale;
{
	union rrropc opc;

	opc.opc = opcode.rrr.opc;
	if(opc.mem.zero)
		return(-1);
	if(!opc.mem.idx)
		sprintf(operands, "r%d,r%d,r%d", dest, src1, src2);
	else
		sprintf(operands, "r%d,r%d[r%d]", dest, src1, src2);
	return(0);
}

char *
nameofbit(code)
int code;
{
	if((code >= 2) && (code <= 11))
		return(tynames[code-1]);

	return(tynames[0]);
}

char *
cndtype(code)
int code;
{
	if(code < 16)
		return(bitpats[code+1]);

	return(bitpats[0]);
}
