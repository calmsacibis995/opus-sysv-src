/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)sdb:com/opset.c	1.12"

/*
 *	UNIX debugger
 *
 *		Instruction printing routines.
 *		MACHINE DEPENDENT.
 *		3B: dis_dot() in "dis" subdirectory;
 *		    routines take form 3B disassembler
 */

#include "head.h"

#ifndef clipper
#define SYSTAB struct systab
SYSTAB {
	int	argc;
	char	*sname;
};
extern struct systab systab[];
#endif /* clipper */
extern STRING	regname[];
#ifndef clipper
extern STRING	fltimm[];
#endif /* clipper */
#if vax
extern int argument [];	/* arguments in case statement - argument[1] is lowest
			 * value case, argument[2] is number of cases
			 * (add one for default).
			 * This array is set by dis_dot.
			 */
extern int caseflg;	/* Flags case statement for disassembly -
			 * set by dis_dot
			 */
#endif

/*	printins() all new to disassemble for 3B-20
 *	use code from 3B disassembler -- see "dis" sub-directory
 *	New arguments; change calls in prvar.c also
 *	calls dis_dot(adr,space,fmt)
 */

printins(fmt,idsp)
char fmt;
{
	struct proct *procp;
	long value;
	unsigned short ins;
	union word word;
	long dis_dot();

#if DEBUG
	if (debugflag ==1)
	{
		enter1("printins");
	}
	else if (debugflag == 2)
	{
		enter2("printins");
		arg("fmt");
		printf("0x%x",fmt);
		comma();
		arg("idsp");
		printf("0x%x",idsp);
		closeparen();
	}
#endif
#if vax
	procp = adrtoprocp(dot);
/* On VAX, the first two bytes of a proc are reserved for use with return */
	if (procp->paddress == dot) {
		value = chkget(dot,idsp);
		printf("0x%4.4x", value & 0xffff);
		oincr = 2;
#if DEBUG
		if (debugflag == 1)
		{
			exit1();
		}
		else if (debugflag == 2)
		{
			exit2("printins");
			endofline();
		}
#endif
		return;
	}
#endif

	/* dot (the location counter) used (but not changed) by dis_dot() */
	value = dis_dot(dot,idsp,fmt);		/* disassemble instr at dot */
	oincr = value - dot;
	printline();			/* print it out */
	/* when not too difficult, print symbolic info (separately) */
	if (fmt == 'i')
		prassym();
#if DEBUG
	if (debugflag == 1)
	{
		exit1();
	}
	else if (debugflag == 2)
	{
		exit2("printins");
		endofline();
	}
#endif
}

/* prassym(): symbolic printing of disassembled instruction */
static
prassym()
{
	int cnt, regno, jj;
	long value;
	char rnam[9];
	register char *os;
	extern	char	mnem[];		/* in dis/extn.c */

#if vax || clipper
	char *regidx;
	int idxsize;
#endif /* vax || clipper */
#if vax
	int sign = 0;
	int starflg = 0;
	int os_bkup;
#else
#define	os_bkup	0
#endif

#if DEBUG
	if (debugflag ==1)
	{
		enter1("prassym");
	}
	else if (debugflag == 2)
	{
		enter2("prassym");
		closeparen();
	}
#endif
#if !m88k
	/* depends heavily on format output by disassembler */
#if clipper
	if (mnem[0] == '*')	/* invalid opcode encountered */
		return;
#else /* clipper */
	printf("\t[");
#endif
	cnt = 0;
	os = mnem;	/* instruction disassembled by dis_dot() */
	while(*os != '\t' && *os != ' ' && *os != '\0')
		os++;		/* skip over instr mnemmonic */
#if clipper
	while (*os == '\t' || *os == ',' || *os == ' ')	/* skip white space */
		os++;
	if (*os == '\0') {	/* end of line */
#if DEBUG
		if (debugflag == 1) {
			exit1();
		} else if (debugflag == 2) {
			exit2("prassym");
			endofline();
		}
#endif
		return;
	} else
		printf("\t[");
#endif /* clipper */
	while (*os) {
		while(*os == '\t' || *os == ',' || *os == ' ')
			os++;
		value = 0;
		regno = -1;
		rnam[0] = '\0';
#if u3b || u3b5 || u3b15 || u3b2
		switch (*os) {
		    case '$':
			jj = sscanf(os, "$0x%lx", &value);
			break;
		    case '%':
			jj = sscanf(os, "%%%[^), \t]", rnam);
			break;
		    case '0':
			jj = sscanf(os, "0x%lx(%%%[^)]",
					&value, rnam);
			break;
		    case '&':
			jj = sscanf(os, "&0x%lx", &value);
			break;
		    case '+':
		    case '-':
			while(*os != '\t' && *os != ' ' && *os != '\0')
				os++;
			jj = sscanf(os, " <%x>", &value);
			break;
		    default:
			jj = 0;
			break;
		}
#else
#if vax
		os_bkup = 0;
		idxsize = 0;
	top:
		switch (*os) {
		    case '$':
			jj = sscanf(os, "$0x%x", &value);
			break;

		    case '[':	/* mode 4 (indexed) */
			regidx = os;
			idxsize = 1;
			while (*os++ != ']')
				idxsize++;
			goto top;

		    case '(':	/* modes 6 and 8 - (Rn) and (Rn)+ */
			jj = sscanf(os, "(%[^)])", rnam);
			break;

		    case '*':	/* deferred addressing */
			starflg = 1;
			os++;
			goto top;

		    case '-':	/* -(Rn) or -number(Rn) */
			os_bkup = 1;
			if (*++os == '(') {
				jj = sscanf(os, "(%[^)])", rnam);
				break;
			}
			sign = 1;
			/* fall through to default case */

		    default:	/* Rn or number(Rn) */
			if (*os <= '9' && *os >= '0') {
				jj = sscanf(os, "%ld", &value);
				if (sign) {
					sign = 0;
					value = -value;
				}
				os_bkup++;
				while (*++os != '(') os_bkup++;
				/* jj > 0 only if number and register read */
				jj *= sscanf(os, "(%[^)]", rnam);
			}
			else if (*os <= 'z' && *os >= 'a') {
				if ((jj = sscanf(os, "%[^, \t]", rnam)) == EOF)
					jj = sscanf(os, "%s", rnam);
			}
			else jj = 0;
			break;
		}
#else
#if clipper
		idxsize = 0;
	top:
		switch (*os) {
		    case '0':	/* Hex value - could be Nbr(reg) or Nbr */
			jj = sscanf(os, "0x%lx(%[^)]", &value, rnam);
			break;

		    case '[':	/* Indexed addressing mode */
			regidx = os;
			do {
				idxsize++;
			} while (*os++ != ']');
			goto top;

		    case '(':	/* Relative addressing mode */
			jj = sscanf(os, "(%[^)])", rnam);
			break;

		    case 'r':	/* General register */
		    case 'f':	/* Floating point register or 'fp' */
		    case 'p':	/* Processor register, 'pc' or 'psw'*/
		    case 's':	/* 'sp' or 'ssw' */
			jj = sscanf(os, "%[^, \t]", rnam);
			break;

		    default:	/* Anything else - do nothing */
			break;
		}
#endif /* clipper */
#endif /* vax */
#endif /* u3b || u3b5 || u3b15 || u3b2 */
		if (*rnam) {
			int kk;

			for (kk = 0; regname[kk]; kk++) {
				if (eqstr(rnam, regname[kk])) {
					regno = kk;
					break;
				}
			}
		}
		if (jj > 0) {
			if (cnt++ > 0)
				printf(",");
#if vax || clipper
			while (idxsize-- > 0)
				printf("%c", *regidx++);
#if vax
			if (starflg) {
				printf("*");
				starflg = 0;
			}
#endif /* vax */
#endif /* vax || clipper */
			jj = psymoff(value, regno, 'i');
		}
		if (jj == (-1))
			os -= os_bkup;
		while(*os != '\t' && *os != ',' && *os != ' ' && *os != '\0') {
			if (jj == (-1))
				printf("%c", *os);   /* just as is */
			os++;
		}
	}
	printf("]");

#if vax
	/* Disassemble cases of case instruction (not in mnem because
	 * size of output may be very large).  argument[1] has lower limit
	 * of cases, argument[2]+1 contains the number of cases, both set
	 * by dis_dot().  dot + oincr - 2*(argument[2] + 1) is where the
	 * list of case branch displacements begins.
	 */
	if (caseflg) {
		int numargs = argument[2] + 1;
		int firstcase = dot + oincr - numargs - numargs;
		int argno;
		for (argno = 0; argno < numargs; argno++) {
			/* branch displacement for case "argno"
			 * (address = PC [i.e. dot] + displacement)
			 * is at firstcase + 2*argno (2 bytes per case)
			 */
			value = (long)(short)get(firstcase+argno+argno,ISP);

			printf("\n    ");
			if (argno + argument[1] < 0) printf("-%#x:    ",
				-(argno + argument[1]));
			else printf("%#x:    ", argno + argument[1]);
			if (value < 0) printf("-%#x ", -value);
			else		printf("%#x ", value);
			printf("<%#x>\t[", firstcase + value);

			psymoff(firstcase+value, -1, 'i');
			printf("]");
		}

	}
#endif
#endif /* m88k */
#if DEBUG
	if (debugflag == 1)
	{
		exit1();
	}
	else if (debugflag == 2)
	{
		exit2("prassym");
		endofline();
	}
#endif
}

/* changed 2nd arg in psymoff():  char **r --> int regno */
static
psymoff(val, regno, fmt)
L_INT val; char fmt; int regno;
{
	struct proct *procp;
	register long diff = -1;

#if DEBUG
	if (debugflag ==1)
	{
		enter1("psymoff");
	}
	else if (debugflag == 2)
	{
		enter2("psymoff");
		arg("val");
		printf("0x%x",val);
		comma();
		arg("regno");
		printf("0x%x",regno);
		comma();
		arg("fmt");
		printf("0x%x",fmt);
		closeparen();
	}
#endif
	if (fmt == 'i') {
#if !clipper && !m88k
		if (regno == APNO) {   /* parameter ("ap" in regname) */
			diff = adrtoparam((ADDR)val, adrtoprocp(dot));
		} else
#endif
		if (regno == FPNO) {	/* local ("fp" in regname) */
			diff = adrtolocal((ADDR)val, adrtoprocp(dot));
		}
		else if (ISREGVAR(regno)) {
			diff = adrtoregvar((ADDR)regno, adrtoprocp(dot));
		}
		if (regno != -1 && diff < 0)
		{
#if DEBUG
			if (debugflag == 1)
			{
				exit1();
			}
			else if (debugflag == 2)
			{
				exit2("psymoff");
				printf("0x%x",-1);
				endofline();
			}
#endif
			return(-1);
		}
		if(diff != -1) {
#if vax || clipper || m88k
			/* VAX symbols have _ prepended */
			if (*sl_name == '_')
			{
				printf( "%s", sl_name+1 );
			}
			else
			{
				printf( "%s", sl_name );
			}
#else
#if u3b || u3b5 || u3b15 || u3b2
			printf( "%s", sl_name );
#endif
#endif
			prdiff(diff);
#if DEBUG
			if (debugflag == 1)
			{
				exit1();
			}
			else if (debugflag == 2)
			{
				exit2("psymoff");
				printf("0x%x",0);
				endofline();
			}
#endif
			return(0);
		}

		if (val < firstdata) {
			if ((procp = adrtoprocp((ADDR) val)) != badproc) {
				prlnoff(procp, val);
#if DEBUG
			if (debugflag == 1)
			{
				exit1();
			}
			else if (debugflag == 2)
			{
				exit2("psymoff");
				printf("0x%x",0);
				endofline();
			}
#endif
				return(0);
			}
		} else {
			if ((diff = adrtoext((ADDR) val)) != -1) {
#if vax || clipper || m88k
				if (*sl_name == '_')
				{
					printf( "%s", sl_name + 1 );
				}
				else
				{
					printf( "%s", sl_name );
				}
#else
#if u3b || u3b5 || u3b15 || u3b2
				printf( "%s", sl_name );
#endif
#endif
				prdiff(diff);
#if DEBUG
				if (debugflag == 1) {
					exit1();
				} else if (debugflag == 2) {
					exit2("psymoff");
					printf("0x%x",0);
					endofline();
				}
#endif
				return(0);
			}
		}
	}
#if clipper || m88k
	printf("%d", val);
#else
	prhex(val);
#endif
#if DEBUG
	if (debugflag == 1)
	{
		exit1();
	}
	else if (debugflag == 2)
	{
		exit2("psymoff");
		printf("0x%x",1);
		endofline();
	}
#endif
	return(1);
}


prdiff(diff)
{
#if DEBUG
	if (debugflag ==1)
	{
		enter1("prdiff");
	}
	else if (debugflag == 2)
	{
		enter2("prdiff");
		arg("diff");
		printf("0x%x",diff);
		closeparen();
	}
#endif
	if (diff) {
		printf("+");
		prhex(diff);
	}
#if DEBUG
	if (debugflag == 1)
	{
		exit1();
	}
	else if (debugflag == 2)
	{
		exit2("prdiff");
		endofline();
	}
#endif
}
