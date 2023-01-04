/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)sdb:com/machdep.c	1.23"

/*
 ****		MACHINE and OPERATING SYSTEM DEPENDENT
 ****		Routines which deal with the run-time stack
 */

#include "head.h"
#include "coff.h"
#include "machdep.h"




extern BKPTR	bkpthead;
extern MSG		NOPCS;

long rdsym();			/* in symt.c */
extern SYMENT syment;		/* rdsym() stores symbol table entry */
extern AUXENT auxent[];		/* rdsym() stores auxiliary entry */
static int frameflg;		/* set if now past last frame */
static int broken;		/* broken frame stack */
static int nosave;
static int savrflag;
static ADDR topoffrm;		/* top of frame if ap < fp */
static ADDR epargp;		/* Fortran entry point prologue builds an */
				/* extra frame and saves the old ap */

#define EVEN	(-2)

/* initialize frame pointers to top of call stack */
/*  MACHINE DEPENDENT */
struct proct *
initframe() {
	register struct proct *procp;

#if DEBUG
	if (debugflag ==1)
	{
		enter1("initframe");
	}
	else if (debugflag == 2)
	{
		enter2("initframe");
		closeparen();
	}
#endif
	if ((pid == 0) && (fcor < 0 || fakecor)) {
		frameflg = 1;
		return(badproc);
	}

#if vax || u3b
	argp = SDBREG(AP);
#endif
#if m88k
	argp = SDBREG(APNO);
	frame = SDBREG(FPNO);
#endif
#if clipper || ns32000
	argp = SDBREG(AP);
#endif
#if vax || u3b || clipper || ns32000
	frame = SDBREG(FP);
#else
#if u3b5 || u3b15 || u3b2
	argp = regvals[10];
	frame = regvals[9];
#endif
#endif
#if vax || u3b || clipper || ns32000 || m88k
	callpc = SDBREG(PC);
/*printf("frame is %x, argp is %x, callpc is %x\n",frame,argp,callpc);*/
#else
#if u3b5 || u3b15 || u3b2
	callpc = regvals[15];
#endif
#endif

	procp = adrtoprocp(callpc);
	frameflg = 0;
	topoffrm = 0;
	broken = 0;

#if u3b || u3b2 || u3b5 || u3b15
	/*
	**	if (frame <= argp)  the stack frame is broken.
	**	try to fix it.
	*/	

	if (frame <= argp) 
		broken = fixframe(procp);
#endif
#if vax || clipper || m88k
#if vax
	/* FP in VAX starts at maxstor = 1L << 31 and decreases */
	if ((frame & 0xf0000000) != 0x70000000) {
#else
#if m88k
	/* FP in M88K starts at 0xf0000000 and decreases */
	if ((frame & 0xff000000) != 0xef000000) {
#else
	/* FP in CLIPPER starts at 0xc000000 and decreases */
	if ((frame & 0xff000000) != 0xbf000000) {
#endif
#endif
		frameflg = 1;
	}
#endif

#if DEBUG
	if (debugflag == 1)
	{
		exit1();
	}
	else if (debugflag == 2)
	{
		exit2("initframe");
		printf("0x%x",procp);
		endofline();
	}
#endif
	return(procp);
}
/*--------------------------------------*/

fixframe(procp)
register struct proct *procp;
{
	register struct proct *tmprocp;
	extern ADDR dpstart, dpsize;
	union word word;
	
#if DEBUG
	if (debugflag == 2)
	{
		enter2("fixframe");
		closeparen();
		endofline();
	}
#endif
/*
 * if pc is in the range of doprnt.s text, reproduce a frame
 */
	if ((callpc > dpstart) && (callpc < dpstart + dpsize))
	{
#if u3b2 || u3b5 || u3b15
		fixdprnt(regvals[12]);
#else
		fixdprnt(SDBREG(SP));
#endif
		return 0;
	}

/*
 *  FORTRAN entry points. the prolog pushes the original ap on the stack,
 *  followed by the arguments. ap is set to the first argument.
 */
	if ((argp - frame) == 2*WORDSIZE)
	{
		tmprocp = procp;
		while ((tmprocp->notstab) && (tmprocp != badproc))
			tmprocp = adrtoprocp(tmprocp->paddress -1);
		if ((tmprocp->sfptr)->f_type == F77) {
			epargp = get(argp - WORDSIZE, DSP); /*save the real ap*/
			return 0;
		}
	}

/*
 *  Next instruction is 'RET', fp is restored,  (for 3b2, 3b5 )	
 *  or just before the first instruction of the procedure.
 *  In both cases, sp  points to top of frame.
 */

#if u3b2 || u3b5 || u3b15
	word.w = get(callpc,ISP);
	if (nosave  || (frame == argp) || (word.c[0]  == 0x08) || 
				(callpc == procp->paddress)) 
	{
		topoffrm = regvals[12];
		return 0;
	}
#else
	if (nosave || (callpc == procp->paddress))
	{
		topoffrm = SDBREG(SP);	/* SP points to top of frame */
		return 0;
	}
#endif

	return 1;
}
/*--------------------------------------*/
/*
 * _doprnt uses fp as a general purpose register. 
 * Before any of the printf routines jmp to doprnt, LOCALS bytes are pushed
 * on the stack for local variables, and ap is saved at (SP - APSAVE).
 * In doprnt itself a max of 3 words are pushed on the stack - so fixprnt
 * checks up to 3 locations to find the last frame.
 */
#if u3b2 || u3b5 || u3b15
#define LOCALS	468
#else
#define LOCALS	460
#endif
#define APSAVE	16

fixdprnt(addr)
ADDR addr;
{
	int i;

#if DEBUG
	if (debugflag == 2)
	{
		enter2("fixdprnt");
		closeparen();
		endofline();
	}
#endif
	addr -= (LOCALS + SAVEDREGS*WORDSIZE) ;
	
	for (i = 0; i < 3; i++)
	{
	     if ( (isisp(get(addr, DSP))) &&
		      (get (addr + WORDSIZE,DSP) > (POS)USRSTACK ) &&
		      (get (addr + 2*WORDSIZE,DSP) > (POS)USRSTACK ) )
				break;
	      else
				addr -= WORDSIZE;
	}
	frame = addr + (SAVEDREGS*WORDSIZE);
	argp =  get(frame + LOCALS - APSAVE, DSP);
	
}
/*--------------------------------------*/
/*
 *  savr frame
 */
fixsavr()
{

#if DEBUG
	if (debugflag == 2)
	{
		enter2("fixsavr");
		closeparen();
		endofline();
	}
#endif
#if u3b2 || u3b5 || u3b15
	if (!isisp(callpc) && (callpc < USRSTACK)) {
#else
	if (!isisp(callpc)) {
#endif
		topoffrm = NEXTCALLPC - WORDSIZE;
		callpc = get(topoffrm, DSP);
		argp = get(frame +  3 * WORDSIZE,DSP);
		frame = get(NEXTFRAME, DSP);
	} 
}
/*--------------------------------------*/

struct proct *
nextframe() {
	register struct proct *procp;
	extern ADDR dpstart, dpsize;
	ADDR oldap;

#if DEBUG
	if (debugflag ==1)
	{
		enter1("nextframe");
	}
	else if (debugflag == 2)
	{
		enter2("nextframe");
		closeparen();
	}
#endif
#if u3b5 || u3b15 || u3b2 || u3b
	/*
 	**	If we have stopped in an assembly language
 	**	routine which does not do a save, then
 	**	the frame pointer will not have been saved
	**	and reset for the current frame. 
 	*/
	if (broken) 
		return(badproc);	/* broken stack frame */
	if (epargp) {
		argp = epargp;
		epargp = 0;
	}
	oldap = argp;			/* save old AP */
	if (frame <= argp) {
		if (topoffrm == 0)  {		 /*topoffrm is not set */
			frameflg = 1;
			return(badproc);
		}
		callpc = get(topoffrm - 2*WORDSIZE, DSP);
		if (savrflag)
			fixsavr();
		else 
			argp = get(topoffrm - 1*WORDSIZE, DSP);
	}
	else {
		callpc = get(NEXTCALLPC, DSP);
		if (savrflag)
			fixsavr();
		else {
			argp = get(NEXTARGP, DSP);
			frame = get(NEXTFRAME, DSP);
		}
	}
 
#endif
#if vax || clipper || ns32000 || m88k
#if m88k
#if DIAB
	callpc = get(argp + getframeoffset(callpc) - (WORDSIZE), DSP) & EVEN;
	argp =  (argp + getframeoffset(callpc)) & EVEN;
	frame = argp;
#else
	if (argp != 0) {
		callpc = get(NEXTCALLPC, DSP) & EVEN;
		argp = get(NEXTARGP, DSP) & EVEN;
		frame = (argp - getframeoffset(callpc)) & EVEN;
	}
#endif
#else
	callpc = get(NEXTCALLPC, DSP);
	argp = get(NEXTARGP, DSP) & EVEN;
	frame = get(NEXTFRAME, DSP) & EVEN;
#endif
#if m88k
	if (callpc == 0 || frame == 0 || argp == 0)
#else
	if (frame == 0)		/* Don't go below bottom of stack */
#endif
		return(badproc);
#endif

#if u3b || u3b5 || u3b15 || u3b2
	if (!savrflag)
		topoffrm = oldap;	/* save old AP in case next frame has no fp */

/*
 * if in _doprnt fix frame
 */
	if ((callpc > dpstart) && (callpc < dpstart + dpsize))
		fixdprnt(oldap);
#endif
	procp = adrtoprocp(callpc-1);
	frameflg = ((procp == badproc) || (eqstr("_start", procp->pname)));
#if u3b5 || u3b15 || u3b2 || u3b
/* This appears to only work okay on the 3b family (CCF) */
	if (!frameflg) {
		if (savrflag) 
			savrflag = 0;
		else if (argp >= frame)
			broken = fixframe(procp);
		if (eqstr("savr", procp->pname)) 
			savrflag = 1;
	}
#endif

#if DEBUG
	if (debugflag == 1)
	{
		exit1();
	}
	else if (debugflag == 2)
	{
		exit2("nextframe");
		printf("0x%x",procp);
		endofline();
	}
#endif
	return(procp);
}

/* returns core image address for variable */
/* formaddr() should be in symt.c ?? */
ADDR
formaddr(class, addr)
register char class;
ADDR addr; {
	ADDR x;

#if DEBUG
	if (debugflag ==1)
	{
		enter1("formaddr");
	}
	else if (debugflag == 2)
	{
		enter2("formaddr");
		arg("class");
		printf("0x%x",class);
		comma();
		arg("addr");
		printf("0x%x",addr);
		closeparen();
	}
#endif
	switch(class) {
	case C_REG:
	case C_REGPARM:
		x = stackreg(addr);
		break;
	case C_EXT:
	case C_STAT:
		x = addr;
		break;
	case C_MOS:
	case C_MOU:
	case C_MOE:
	case C_FIELD:			/* ?? */
		x = addr;
		break;
		
	case C_ARG:
		x = argp+addr;
		break;
		
	case C_AUTO:
		x = frame+addr;	/* addr was negated in outvar() */
		break;

	default:
		fprintf(FPRT1,"Bad class in formaddr: %d(%#x)\n", class, class);
		x = 0;
		break;
	}
#if DEBUG
	if (debugflag == 1)
	{
		exit1();
	}
	else if (debugflag == 2)
	{
		exit2("formaddr");
		printf("0x%x",x);
		endofline();
	}
#endif
	return x;
}

/*
 *  stackreg(reg):
 * This routine is intended to be used to find the address of a register
 * variable.  The stack is searched backwards from the current frame
 * until the procedure with the register variable is found.  If the
 * register assigned to the variable appears on the stack, then its
 * address is returned.  Otherwise, the register variable is still in
 * the named register, so the register number is returned.  We distinguish
 * addresses from register numbers by noting that register numbers are less
 * than NUMREGS (16) and that stack addresses are greater.
 *
 *  MACHINE DEPENDENT
 */
/* new stackreg() for 3B Unix
 *	C stores 'n' registers starting with %R8 and going backwards
 *	can only get 'n' by looking at text -- bits 8->11 from beginning of proc
 */

#if u3b || u3b5 || u3b15 || u3b2
#define REGSAV1		8
#define ISREGSAV(xr,xn)	(((REGSAV1-xn) < xr) && (xr <= REGSAV1))
#if u3b
#define NUMREGSOFF	1
#define REGADDR(xr)	(frame - (REGSAV1-xr+1)*REGSIZE)
#else
#define NUMREGSOFF	(9 - (getval(procp->st_offset+1,"bu",DSP)&0x0f))
#define REGADDR(xr,xn)	(frame - (ADDR)(15-xr-xn)*REGSIZE)
#endif
ADDR
stackreg(reg)
ADDR reg;
{
	register struct proct *procp;
	ADDR regaddr;
	unsigned int nrs;
	/* unsigned int nn;	don't adjust addresses - see below */

#if DEBUG
	if (debugflag ==1)
	{
		enter1("stackreg");
	}
	else if (debugflag == 2)
	{
		enter2("stackreg");
		arg("reg");
		printf("0x%x",reg);
		closeparen();
	}
#endif
	/*  if no procedure to search down to, return reg */
	if (proc[0] == '\0')
	{
#if DEBUG
		if (debugflag == 1)
		{
			exit1();
		}
		else if (debugflag == 2)
		{
			exit2("stackreg");
			printf("0x%x",reg);
			endofline();
		}
#endif
		return(reg);
	}


	regaddr = reg;
	for (procp=initframe(); procp != badproc; procp=nextframe()) {
		if (sl_procp == procp) break;
#if u3b
		nrs = getval(procp->paddress +NUMREGSOFF, "bu", DSP);
		nrs >>= 4;	/* get high four bits */
		if (ISREGSAV(reg,nrs)) {
			regaddr = REGADDR(reg);
#else
		nrs = NUMREGSOFF;
		if (ISREGSAV(reg,nrs)) {
			regaddr = REGADDR(reg,nrs);
#endif
			/*  3B stores chars and shorts in registers
			 * right justified, even though they are stored
			 * left justified in core.  Register variables
			 * are stored in register image on the stack,
			 * and that is the way they should be treated,
			 * so that the calling function doesn't have
			 * to check again whether they are actually
			 * in the registers or not.
			 * nn = WORDSIZE - typetosize(sl_type, sl_size);
			 * if (nn > 0)
			 * 	regaddr += nn;
			 */
		}
	}
	if (procp == badproc) {
		fprintf(FPRT1, "Stackreg() error: frame=%#x\n", frame);
		regaddr = ERROR;
	}
#if DEBUG
	if (debugflag == 1)
	{
		exit1();
	}
	else if (debugflag == 2)
	{
		exit2("stackreg");
		printf("0x%x",regaddr);
		endofline();
	}
#endif
	return(regaddr);
}

#else
#if vax
/*  VAX version - C stores 'n' registers arbitrarily.
 *	A mask (16 bits) is stored as the high order short in the word which is
 *	offset one word from the beginning of the frame (frame + WORDSIZE).
 *	The 0'th bit of this mask tells whether register 0 has been saved,
 *	the 1'st bit tells wheter register 1 has been saved, etc.
 *	Each saved register in the frame occupies another word of space,
 *	and they are stored in order, from lowest to highest numbered.
 *	The first saved register is at frame + 5 * WORDSIZE.
 */

#define REGOFF 5 * WORDSIZE
#define MASKOFF WORDSIZE
ADDR
stackreg(reg) {
	register int regfl, mask, i;
	register struct proct *procp;
	ADDR regaddr;

#if DEBUG
	if (debugflag ==1)
	{
		enter1("stackreg");
	}
	else if (debugflag == 2)
	{
		enter2("stackreg");
		arg("reg");
		printf("0x%x",reg);
		closeparen();
	}
#endif
	if (proc[0] == '\0')
	{
#if DEBUG
		if (debugflag == 1)
		{
			exit1();
		}
		else if (debugflag == 2)
		{
			exit2("stackreg");
			printf("0x%x",reg);
			endofline();
		}
#endif
		return(reg);
	}
	regaddr = reg;
	regfl = 0x10000 << reg;
	for (procp=initframe(); procp!=badproc; procp=nextframe()) {
		if (sl_procp == procp)
			break;
		if ((mask = get(frame + MASKOFF)) & regfl) {
			regaddr = frame + REGOFF;
			for (i=0; i < reg; i++) {
				if (mask & 0x10000) regaddr += WORDSIZE;
				mask >>= 1;
			}
		}
	}
	if (procp == badproc) {
		fprintf(FPRT1, "Stackreg() error: frame=%#x\n", frame);
		regaddr = ERROR;
	}
#if DEBUG
	if (debugflag == 1)
	{
		exit1();
	}
	else if (debugflag == 2)
	{
		exit2("stackreg");
		printf("0x%x",regaddr);
		endofline();
	}
#endif
	return(regaddr);
}
#else
#if clipper
/*
 * CLIPPER version
 */

/* <reg> in these defines comes from the symbol table.  Therefore,
 * the floating point regs are numbered from 16 - 23, not from
 * 16 - 31 as they are normally. */
#define ISUSRREG(reg) (0 <= (reg) && (reg) <= 13)
#define ISGENREG(reg) (0 <= (reg) && (reg) <= 15)
#define ISFLTREG(reg) (16 <= (reg) && (reg) <= 23)
#define ISTMPREG(reg) ((0 <= (reg) && (reg) <= 5) ||\
		       (16 <= (reg) && (reg) <= 19))
ADDR
stackreg(reg)
{
	register struct proct *procp;
	ADDR regaddr;
	ADDR stkaddr;
	ADDR ocallpc, oframe;
	int  oframeflg;
	ADDR stacksav();

#if DEBUG
	if (debugflag ==1) {
		enter1("stackreg");
	} else if (debugflag == 2) {
		enter2("stackreg");
		arg("reg");
		printf("0x%x",reg);
		closeparen();
	}
#endif
	regaddr = (ADDR)reg;		/* Default - register = address */

	oframe = frame;
	oframeflg = frameflg;
	ocallpc = callpc;
	/* Are we looking at the top proc on the stack? */
	if (frame == SDBREG(FP))
		goto done;	/* Yes - the address is the register */
	/* No - if temp reg, it can't be found on stack */
	if (ISTMPREG(reg)) {
		regaddr = (ADDR)-1;
		goto done;
	}
	for (procp = initframe(); procp != badproc; procp=nextframe()) {
		if (frame == oframe)	/* We're back where we started */
			goto done;
		stkaddr = stacksav(reg, procp->paddress, procp->paddrbf);
		if (stkaddr != (ADDR)-1) {
			regaddr = stkaddr;
			goto done;
		}
	}
	/* procp == badproc */
	fprintf(FPRT1, "Stackreg() error: frame=0x%08x\n", frame);
	regaddr = (ADDR)-1;

done:
	frame = oframe;
	frameflg = oframeflg;
	callpc = ocallpc;
#if DEBUG
	if (debugflag == 1) {
		exit1();
	} else if (debugflag == 2) {
		exit2("stackreg");
		printf("0x%x",regaddr);
		endofline();
	}
#endif
	return(regaddr);
}
#else
#if m88k
/*
 * m88k version
 */

#define ISGENREG(reg) (0 <= (reg) && (reg) <= 31)
#define ISTMPREG(reg) (2 <= (reg) && (reg) <= 13)

ADDR
stackreg(reg)
{
	register struct proct *procp;
	ADDR regaddr;
	ADDR stkaddr;
	int inst;
	ADDR instaddr;
	int savsiz;
	int regmask;
	int regfl;
	int totoffset;
	int regno;
	int i;

#if DEBUG
	if (debugflag ==1) {
		enter1("stackreg");
	} else if (debugflag == 2) {
		enter2("stackreg");
		arg("reg");
		printf("0x%x",reg);
		closeparen();
	}
#endif

	regaddr = (ADDR)reg;		/* Default - register = address */
	regfl = 1 << reg;
	/* we can use the register if this is the current procedure */
	if (argp == SDBREG(APNO))
		goto done;

	/* if a temporary register, it isn't on the stack */
	if (ISTMPREG(reg)) {
		regaddr = (ADDR)-1;
		goto done;
	}
	for (procp = initframe(); procp != badproc; procp=nextframe()) {
		if (sl_procp == procp)
			goto done;
		instaddr = procp->paddress;
		inst = get(instaddr, ISP);
		if ((inst & 0xffff0000) == 0x67ff0000) { /* subu r31,r31,n */
			savsiz = 0;
			instaddr += WORDSIZE;
			inst = get(instaddr, ISP);
			regmask = 0;
							/* st   rn,r31,n */
			while ((inst & 0xf81f0000) == 0x201f0000) { 
				savsiz++;
				regno = (inst & 0x3e00000) >> 21;
				regmask |= 1 << regno;
				if ((inst & 0x04000000) == 0)	/* st.d */
					regmask |= 1 << (regno+1);
				if (regno == 1)
					break;
				instaddr+=WORDSIZE;
				inst = get(instaddr, ISP);
			}
		}
		if (regmask & regfl) {
			if (savsiz & 01)	/* round up to even word */
				savsiz++;
			regaddr = argp - savsiz * WORDSIZE; 
			for (i=2; i < reg; i++)
				if ((regmask>>i) & 1) 
					regaddr += WORDSIZE;
		}
	}
	if (procp == badproc) {
		fprintf(FPRT1, "Stackreg() error: frame=%#x\n", frame);
		regaddr = (ADDR)-1;
	}
done:
#if DEBUG
	if (debugflag == 1) {
		exit1();
	} else if (debugflag == 2) {
		exit2("stackreg");
		printf("0x%x",regaddr);
		endofline();
	}
#endif
	return(regaddr);
}

getframeoffset(pc) {
	register struct proct *procp;
	ADDR instaddr;
	int inst;
	int offset;
	
	offset = 0;
	procp = adrtoprocp(pc);
	inst = get(procp->paddress, ISP);
	if ((inst & 0xffff0000) == 0x67ff0000) /* subu r31,r31,n */
		offset = inst & 0xffff;
	return(offset);
}
#endif /* m88k */
#if ns32000
/*  NS32000 version - C stores 'n' registers arbitrarily.
 *	A mask (8 bits) can be determined by the enter instruction for
 *	which registers have been saved.  Unfortunately to get to the
 *	enter instruction, one needs to determine which routine we are
 *	in, track down the enter instruction, and find out how many
 *	registers were saved.
 *	The 0'th bit of this mask tells whether register 0 has been saved,
 *	the 1'st bit tells whether register 1 has been saved, etc.
 *	Each saved register in the frame occupies another word of space,
 *	and they are stored in order, from hightest to lowest numbered.
 *	The first saved register is at frame + 1 * WORDSIZE.
 */

#define REGOFF (1 * WORDSIZE)
ADDR
stackreg(reg) {
	register int regfl, mask, i;
	register struct proct *procp;
	ADDR regaddr;
	int inst;
	int enteraddr;
	int enteroffset;

#if DEBUG
	if (debugflag ==1)
	{
		enter1("stackreg");
	}
	else if (debugflag == 2)
	{
		enter2("stackreg");
		arg("reg");
		printf("0x%x",reg);
		closeparen();
	}
#endif
	inst = 0;
	if (proc[0] == '\0')
	{
#if DEBUG
		if (debugflag == 1)
		{
			exit1();
		}
		else if (debugflag == 2)
		{
			exit2("stackreg");
			printf("0x%x",reg);
			endofline();
		}
#endif
		return(reg);
	}
	regaddr = reg;
	regfl = 1 << reg;
	for (procp=initframe(); procp!=badproc; procp=nextframe()) {
		if (sl_procp == procp)
			break;
		enteraddr = procp->paddress;
		if ((inst & 0xff) == 0xea) {	/* branch instruction */
			if (((inst>>8) & 0xc0) == 0) /* br byte disp */
				enteraddr += (inst>>8) & 0xff;
			else if (((inst>>8) & 0xc0) == 0x80) /* br word disp */
				enteraddr += (((inst>>8) & 0x3f) << 8) |
					((inst>>16) & 0xff);
			else {
				inst = get(procp->paddress+1, ISP);
				enteraddr += ((inst>>24) & 0xff) |
					((inst >> 8) & 0xff00) |
					((inst << 8) & 0xff0000) |
					((inst << 24) & 0x3f000000);
			}
		}
	
		inst = get(enteraddr, ISP);
#if DEBUG
		if (debugflag == 2)
			fprintf(FPRT2, "enter address is %#x, inst is %#x\n",
				 enteraddr, inst);
#endif
		if ((inst & 0xff) == 0x82) { 
			mask = (inst >> 8) & 0xff;
			inst = get(enteraddr+2, ISP);
			if ((inst & 0xc0) == 0) 	/* enter byte disp */
				enteroffset = inst & 0xff;
			else if (((inst>>8) & 0xc0) == 0x80) /* word disp */
				enteroffset = ((inst & 0xff) << 8) |
					((inst>>8) & 0x3f);
			else {			/* enter long displacement */
				inst = get(procp->paddress+1, ISP);
				enteroffset = ((inst>>24) & 0xff) |
					((inst >> 8) & 0xff00) |
					((inst << 8) & 0xff0000) |
					((inst << 24) & 0x3f000000);
			}
		} else {
			mask = 0;
			enteroffset = 0;
		}
#if DEBUG
		if (debugflag == 1)
			fprintf(FPRT2, "mask is %#x, enteroffset is %#x\n",
				mask, enteroffset);
#endif
				
		if (mask & regfl) {
			regaddr = frame - enteroffset - REGOFF;
			for (i=0; i < reg; i++) {
				if (mask & 1) regaddr -= WORDSIZE;
				mask >>= 1;
			}
		}
	}
	if (procp == badproc) {
		fprintf(FPRT1, "Stackreg() error: frame=%#x\n", frame);
		regaddr = ERROR;
	}
#if DEBUG
	if (debugflag == 1)
	{
		exit1();
	}
	else if (debugflag == 2)
	{
		exit2("stackreg");
		printf("0x%x",regaddr);
		endofline();
	}
#endif
	return(regaddr);
}
#endif /* ns32000 */
#endif /* clipper */
#endif
#endif

/* Print the stack trace: 't' and 'T' commands
 * set top (i.e. top != 0) to print just the top procedure
 * modified to print something even if a.out stripped
 */

#if u3b

/*  The 3B saves 13 words worth of registers before saving arguments */
#define NARGSTACK(X)	( ((frame - X) / WORDSIZE) -13)
#else
#if u3b5 || u3b15 || u3b2
#define NARGSTACK(X)	(((frame - X) / WORDSIZE) - 9)
#else
#if vax
/*  The number of words stored as arguments is in the first byte
 *	of the zero'th argument.  The remaining bytes of the word should
 *	be zero.  Argp is set to point to the first argument.
 */
#define NARGSTACK(X)    (X += WORDSIZE, \
			(narg = get(X-WORDSIZE, DSP)) & ~0xff ? 0 : narg\
		     )
#endif
#if ns32000
/*  The number of words stored as arguments can be determined by
 *  	looking at the caller's adjspb instruction to find the number
 *	of bytes.
 */
#define NARGSTACK getargcnt();

getargcnt() {
	int inst, narg;

	inst = get(get(frame+WORDSIZE, DSP), ISP);
	if ((inst & 0x7ff) == 0x57c) 		/* adjspb */
		narg = (-((inst<<8)>>24)) & 0xff;
	else if ((inst & 0x7ff) == 0x57d) 	/* adjspw */
		narg = -((inst<<8)>>24);
	else
		narg = 0;

	return(narg/WORDSIZE);
}
#endif
#endif
#endif

prfrx(mode) 			/* mode = 2 assume no save instructin */
int mode;			/* mode = 1  only top frame */
{
	int narg;		/* number of words that arguments occupy */
	long offs;		/* offset into symbol table */
	register char class;	/*storage class of symbol */
	register int endflg;	/* 1 iff cannot find symbolic names of
					more arguments */
	int subsproc = 0;	/* 1 iff not top function on stack */
	int top;
	register char *p;	/* pointer to procedure (function) name */
	int line_num;		/* line number in calling function */
	register struct proct *procp;
	SYMENT *syp = &syment;
	AUXENT *axp = auxent;
	ADDR argpl;	/* local copy of argp, nextframe() needs the original */
	int val;
	ADDR argptr;
	
#if DEBUG
	if (debugflag ==1)
	{
		enter1("prfrx");
	}
	else if (debugflag == 2)
	{
		enter2("prfrx");
		arg("mode");
		printf("0x%x",mode);
		closeparen();
	}
#endif
	switch(mode) {
	case 1:
			top = 1;
			nosave = 0;
			break;
	case 2:
			top = 0;
			nosave = 1;
			break;
	default:
			top = nosave = 0;
	}
	procp = initframe();
	if (frameflg) {		/*  no initial frame */
		if (pid == 0 && (fcor < 0 || fakecor))	{  /* usual error */
			errflg = "No process and no core file.";
			chkerr();
		}
		else {				/* unusual initial frame */
#if DEBUG
			if (debugflag == 1)
			{
				exit1();
			}
			else if (debugflag == 2)
			{
				exit2("prfrx");
				endofline();
			}
#endif
			nosave = 0;
			return;
		}
	}
	do {
#if u3b || u3b5 || u3b15 || u3b2

		argpl = argp;	/*  argp for the current frame */

		/*  3B crt0 (start) has an old fp of zero */
		if (frame < USRSTACK)
			broken = 1;
		else if (get(NEXTFRAME, DSP) == 0)
		{
#if DEBUG
			if (debugflag == 1)
			{
				exit1();
			}
			else if (debugflag == 2)
			{
				exit2("prfrx");
				endofline();
			}
#endif
			nosave = 0;
			return;
		}
#else
#if vax || clipper || ns32000 || m88k
#if m88k	/* don't go past end of valid back trace */
		if (callpc == 0 || frame == 0 || argp == 0) 
#else
		/*  VAX crt0 (start) gets a current fp of zero */
		if (frame == 0)
#endif
		{
#if DEBUG
			if (debugflag == 1)
			{
				exit1();
			}
			else if (debugflag == 2)
			{
				exit2("prfrx");
				endofline();
			}
#endif
			nosave = 0;
			return;
		}
#endif
#endif
		p = procp->pname;
		if (eqstr("__dbsubc", p))	/*  3B ?? */
		{
#if DEBUG
			if (debugflag == 1)
			{
				exit1();
			}
			else if (debugflag == 2)
			{
				exit2("prfrx");
				endofline();
			}
#endif
			nosave = 0;
			return;
		}
		if (procp == badproc) {		/*  if stripped a.out */
			printf("pc: 0x%lx;	args: (", callpc);
			endflg = 1;
		}
#if vax || clipper || ns32000 || m88k
		else if (p[0] == '_') {
			printf("%s(", p+1);
			endflg = 0;
		}
#endif
		else {
			if (nshlib) 
				prpname(procp,"(");
			else
				printf("%s(", p);
			endflg = 0;
		}
		if (endflg == 0) {
			offs = procp->st_offset;
			libn = procp->lib;
			do {		/*  in COFF, always just 1 ? */
				if( (offs = rdsym(offs)) == ERROR) {
					endflg++;
					break;
				}
			} while (ISFCN(syp->n_type));
			class = syp->n_sclass;
			while (! ISARGV(class)) {
				if ((offs = rdsym(offs)) == ERROR) {
					endflg++;
					break;
				}
				class = syp->n_sclass;
				if (ISFCN(syp->n_type) || 
						eqstr(syp ->n_nptr,".ef")) {
					endflg++;
					break;
				}
			}

		}

#if u3b | u3b2 | u3b5 || u3b15
		if (frame <= argp)     /* if fp wasn't saved in current frame */
		{
			if (broken)
			{
				narg = 0;
			}
			else if (epargp) {	/* Fortran entry point */
				argpl = epargp;
				narg = NARGSTACK(epargp);
			}
			else
				narg = ((topoffrm - argp) / WORDSIZE) - 2;
		}
		else {
			if (savrflag) 
				narg = 0;
			else
#endif
#if !clipper && !m88k
				narg = NARGSTACK(argp);
#endif
#if u3b | u3b2 | u3b5 || u3b15
		}
#endif
#if !clipper && !m88k
		argpl = argp + 8;	/*  argp for the current frame */
		while (narg) {
			if (endflg) {
				val = get(argpl,DSP);
				if (val > 9 || val < 0)
					printf("0x");
				printf("%x", val);
				argpl += WORDSIZE;
			} else {
				int length;
				if ((syp->n_type == T_STRUCT) ||
				    (syp->n_type == T_UNION))   {
				    /* print address of structure
				     * (so that structures of, e.g.
				     * 100 element arrays, are not dumped)
				     */
#if u3b || u3b5 || u3b15 || u3b2
				    printf( "&%s=", syp->n_nptr );
#else
#if vax || ns32000
				    /* VAX: skip leading underscore */
				    if (syp->n_nptr[0] == '_')
					printf("&%s=", syp->n_nptr+1);
				    else
					printf("&%s=", syp->n_nptr);
#endif
#endif
				    dispx(argpl, "x", C_EXT, 
						(short) (-1), DSP);
				    length = axp->x_sym.x_misc.x_lnsz.x_size;
				}
				else {
				    if (syp->n_type == T_ENUM)
					length =
					    axp->x_sym.x_misc.x_lnsz.x_size;
				    else
					length = typetosize(syp->n_type, 0);
#if u3b || u3b5 || u3b15 || u3b2
				    printf("%s=", syp->n_nptr);
				    /* The address of a short or char is
				     * expected to be the left byte of the
				     * variable.  However, argp points to
				     * the left end of the WORD containing
				     * the parameter, which is right justified.
				     * The address is thus adjusted accordingly.
				     * length<WORDSIZE test needed, since length
				     * could be sizeof(double) > WORDSIZE.
				     *
				     * Alternate code:
				     *  dispx(argp, "", C_REGPARAM,
				     *	    (short)syp->n_type, DSP);
				     * This has the same effect, since dispx
				     * sees a "register" variable (hence
				     * right justified), on the stack, and
				     * extracts the right part.  This is
				     * more efficient, but less clean, since
				     * parameters are not necessarily
				     * register variables, they only look that
				     * way.
				     */
				    if (length < WORDSIZE) {
					dispx(argpl+WORDSIZE-length,"",
					    C_EXT,(short)syp->n_type,DSP);
				    } else {
					dispx(argpl, "", C_EXT,
					    (short)syp->n_type, DSP);
				    }
#else
#if vax || ns32000
				    /* VAX: skip leading underscore */
				    if (syp->n_nptr[0] == '_')
				    {
					printf( "%s=", syp->n_nptr + 1 );
				    }
				    else
				    {
					printf( "%s=", syp->n_nptr );
				    }
				    dispx(argpl, "", C_EXT, 
					    (short) syp->n_type, DSP);
#endif
#endif
				}
				if (length > WORDSIZE) {
					argpl += length;
					/*  adjust for more than 1 word */
					narg -= length/WORDSIZE -1;
				}
				/* bytes, shorts, longs pushed as ints */
				else {
					argpl += WORDSIZE;
				}
			}
			do {
				if (endflg) break;
				if ((offs = rdsym(offs)) == ERROR) {
					endflg++;
					break;
				}
				class = syp->n_sclass;
				if (ISFCN(syp->n_type)|| 
					eqstr(syp ->n_nptr,".ef")) {
					endflg++;
					break;
				}
			} while (! ISARGV(class));
			if (--narg != 0) printf(",");
		}
#else
#if clipper /* clipper code for processing arguments */
		class = syp->n_sclass;
		narg = -1;
		while (!endflg && ISARGV(class)) {
		    narg++;
		    if (class != C_REGPARM)
			argp = frame + syp->n_value;
		    if ((syp->n_type == T_STRUCT) || (syp->n_type == T_UNION)) {
		        /* Print address of structure so that large structures
			 * are not dumped)
			 */
			/* Skip leading underscore */
			if (syp->n_nptr[0] == '_')
			    printf("&%s=", syp->n_nptr+1);
			else
			    printf("&%s=", syp->n_nptr);
			dispx(argp, "x", C_EXT, (short) (-1), DSP);
		    } else {	/* not structure or union */
		        /* Skip leading underscore */
			if (syp->n_nptr[0] == '_')
			    printf( "%s=", syp->n_nptr + 1 );
			else
			    printf( "%s=", syp->n_nptr );
			if (class != C_REGPARM)
			    dispx(argp, "", C_EXT, (short) syp->n_type, DSP);
			else {
			    ADDR regaddr = stackreg(syp->n_value);

			    if (regaddr == (ADDR)-1) {
			        if (ISGENREG(syp->n_value))
				    printf("r%d", syp->n_value);
			        else
				    printf("f%d", syp->n_value-16);
			    } else if (ISGENREG(regaddr)) {
				printf("0x%08x",SDBREG(reglist[regaddr].roffs));
			    } else if (ISFLTREG(regaddr)) {
				union dbl dbl;
				int freg = (regaddr<<1)-16;

				dbl.ww.w1 = SDBREG(reglist[freg].roffs);
				dbl.ww.w2 = SDBREG(reglist[freg+1].roffs);
				printf("%g", dbl.dd);
			    } else {
				printf("0x%08x", regaddr);
			    }
			}
		    }
		    do {
			if ((offs = rdsym(offs)) == ERROR) {
			    endflg++;
			    break;
			}
			class = syp->n_sclass;
			if (ISFCN(syp->n_type)) {
			    endflg++;
			    break;
			}
		    } while (!ISARGV(class));
		    if (!endflg && ISARGV(class))
			printf(",");
		}
#endif /* clipper */
#if m88k /* m88k code for processing arguments */
		class = syp->n_sclass;
		narg = -1;
		while (!endflg && ISARGV(class)) {
		    narg++;
		    if (class != C_REGPARM)
			argpl = argp + syp->n_value;
		    if ((syp->n_type == T_STRUCT) || (syp->n_type == T_UNION)) {
		        /* Print address of structure so that large structures
			 * are not dumped)
			 */
			/* Skip leading underscore */
			if (syp->n_nptr[0] == '_')
			    printf("&%s=", syp->n_nptr+1);
			else
			    printf("&%s=", syp->n_nptr);
			dispx(argpl, "x", C_EXT, (short) (-1), DSP);
		    } else {	/* not structure or union */
		        /* Skip leading underscore */
			if (syp->n_nptr[0] == '_')
			    printf( "%s=", syp->n_nptr + 1 );
			else
			    printf( "%s=", syp->n_nptr );
			if (class != C_REGPARM)
			    dispx(argpl, "x", C_EXT, (short) syp->n_type, DSP);
			else {
			    ADDR regaddr = stackreg(syp->n_value);
			    if (regaddr == (ADDR)-1) {
				printf("r%d", syp->n_value);
			    } else if (ISGENREG(regaddr)) {
				dispx(SDBREG(reglist[regaddr].roffs), "x", C_EXT, (short) (-1), DSP);
			    } else {
				dispx(get(regaddr,DSP), "x", C_EXT, (short) (-1), DSP);
			    }
			}
		    }
		    do {
			if ((offs = rdsym(offs)) == ERROR) {
			    endflg++;
			    break;
			}
			class = syp->n_sclass;
			if (ISFCN(syp->n_type)) {
			    endflg++;
			    break;
			}
		    } while (!ISARGV(class));
		    if (!endflg && ISARGV(class))
			printf(",");
		}
#endif /* m88k */
#endif
		if (broken)
			printf("...");
		printf(")");
		if (procp->sfptr != badfile)
			printf("   [%s",
				(procp->sfptr)->sfilename);
		if(gflag) {
			if ((line_num = adrtolineno(callpc-subsproc,procp)) > 0)
				printf(":%d", line_num);
		}
		if(procp->sfptr != badfile)
			printf("]");
		printf("\n");
		if (broken)	/* top frame has no fp, SAVE was not executed */
		{
		  printf("Broken stack frame \n");
		  break;
		}
		subsproc = 1;
		procp = nextframe();
		sl_procp = procp;
		libn = procp->lib;
	} while (!top && !frameflg );	/*  only one frame desired, or
						no frames left in backtrace */
/* Vax:	} while (((procp = nextframe()) != badproc) && !top);*/
	nosave = 0;
#if DEBUG
	if (debugflag == 1)
	{
		exit1();
	}
	else if (debugflag == 2)
	{
		exit2("prfrx");
		endofline();
	}
#endif
}


/* machine dependent initialization */
sdbenter() {
#if vax || clipper
	mkioptab();
#endif
}

/* machine dependent exit code */
sdbexit() {
	exit(0);
}

#if u3b || u3b5 || u3b15 || u3b2
/*  isubcall() to replace SUBCALL #define (more complicated on 3B) */
isubcall(loc, space)
long loc;
int space;
{
	register int opcode;
	union word word;

	word.w = get(loc,space);
	opcode = word.c[0];
#if u3b
	return(opcode == 0x79 || opcode == 0x78 || opcode == 0x77);
#else
	return(opcode == 0x2c);		/* opcode for call instruction */
#endif
}
#endif
