/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)dis:m32/bits.c	1.17"
/*
 */
#include	<stdio.h>
#include	"dis.h"
#include	"filehdr.h"
#include	"scnhdr.h"
#include	"ldfcn.h"
#include	"sgs.h"


#define		MAXERRS	10	/* maximum # of errors allowed before	*/
				/* abandoning this disassembly as a	*/
				/* hopeless case			*/

static short errlev = 0;	/* to keep track of errors encountered during	*/
			/* the disassembly, probably due to being out	*/
			/* of sync.					*/

#define		OPLEN	35	/* maximum length of a single operand	*/
				/* (will be used for printing)		*/

static	char	operand[4][OPLEN];	/* to store operands as they	*/
					/* are encountered		*/
static	char	symarr[4][OPLEN];

static	long	start;			/* start of each instruction	*/
					/* used with jumps		*/

static	int	fpflag;		/* will indicate floating point instruction	*/
				/* (0=NOT FLOATING POINT, 1=SINGLE, 2=DOUBLE) 	*/
				/* so that immediates will be printed in	*/
				/* decimal floating point.			*/
static	int	bytesleft = 0;	/* will indicate how many unread bytes are in buffer */

#define	TWO_8	256
#define	TWO_16	65536

/*
 *	dis_text ()
 *
 *	disassemble a text section
 */

dis_text()

{
	/* the following arrays are contained in tables.c	*/
	extern	struct	instable	opcodetbl[256];
	extern	struct	formtable	adrmodtbl[256];

	/* the following entries are from _extn.c	*/
	extern	SCNHDR	scnhdr;
	extern	char	*sname;
	extern	char	*fname;
	extern	LDFILE	*t_ptr;
	extern	int	Aflag;
	extern	int	Sflag;
	extern	int	Lflag;
	extern	int	sflag;
	extern	int	trace;
	extern	long	loc;
	extern	char	mneu[];
	extern	char	object[];
	extern	char	symrep[];
	extern	unsigned short	cur1byte;

	/* the following routines are in _utls.c	*/
	extern	int	printline();
	extern	int	looklabel();
	extern	int	line_nums();
	extern	int	prt_offset();
	extern	int	convert();
	extern	short	sect_name();
	extern	int	compoff();

	/* the following routines are in this file	*/
	long		getword();
	unsigned short	gethalfword();

	SCNHDR		*sech;
	struct instable	*cp;
	unsigned	key;
	char		temp[NCPS+1];
	short		sect_num;
	long 		lngtmp;
	char		ctemp[OPLEN];	/* to store EXTOP operand   */

	/* initialization for each beginning of text disassembly	*/

	bytesleft = 0;
	sech = &scnhdr;

	if (Lflag > 0)
		sect_num = sect_name();
	/*
	 * An instruction is disassembled with each iteration of the
	 * following loop.  The loop is terminated upon completion of the
	 * section (loc minus the section's physical address becomes equal
	 * to the section size) or if the number of bad op codes encountered
	 * would indicate this disassembly is hopeless.
	 */

	start = sech->s_paddr;
#if M88K
	if (Aflag && (Aflag-sech->s_paddr > 0 && 
			sech->s_paddr+sech->s_size-Aflag > 0))
		start = Aflag;
#endif
	for (loc = start; ((loc-sech->s_paddr) < sech->s_size) && (errlev < MAXERRS); printline() ){

		start = loc;
		object[0] = '\0';
		mneu[0] = '\0';
		symrep[0] = '\0';

		if (Lflag > 0)
			looklabel(loc,sect_num);/* look for C source labels */
		line_nums(sech->s_nlnno);	/* print breakpoint line # */
		prt_offset();			/* print offset		   */

		idis(getword(t_ptr));
	}
	if (errlev >= MAXERRS) {
		printf("%sdis: %s: %s: section probably not text section\n",
			SGS,fname, sname);
		printf("\tdisassembly terminated\n");
		exit(4);
	}
}

/*
 *	get1byte()
 *
 *	This routine will read the next byte in the object file from
 *	the buffer (filling the 4 byte buffer if necessary). 
 *
 */
get1byte(fptr)
LDFILE	*fptr;
{
	extern	long	loc;
	extern	int	oflag;
	extern	char	object[];
	extern	int	trace;
	extern	LDFILE	*t_ptr;
	extern	char	bytebuf[];
	extern	unsigned short	cur1byte;

	if (bytesleft == 0) {
		fillbuff(t_ptr);
		if (bytesleft == 0) {
			fprintf(stderr,"\n%sdis:  premature EOF\n",SGS);
			exit(4);
		}
	}
	cur1byte = ((unsigned short) bytebuf[4-bytesleft]) & 0x00ff;
	bytesleft--;
	(oflag > 0)?	sprintf(object, "%s%.3o ",object,cur1byte):
			sprintf(object, "%s%.2x ",object,cur1byte);
	loc++;
	if (trace > 1)
		printf("\nin get1byte object<%s> cur1byte<%.2x>\n",object,cur1byte);
}

/*
 *	gethalfword()
 *
 *	This routine will read the next 2 bytes in the object file from
 *	the buffer (filling the 4 byte buffer if necessary).
 */
unsigned short
gethalfword(fptr)
LDFILE *fptr;
{
	extern	unsigned short	cur1byte;
	extern	char	object[];
	extern	int	trace;
	union {
		unsigned short 	half;
		char		bytes[2];
	} curhalf;

	curhalf.half = 0;
#ifdef AR32W
	get1byte(fptr);
	curhalf.bytes[1] = cur1byte;
	get1byte(fptr);
	curhalf.bytes[0] = cur1byte;
#else
	get1byte(fptr);
	curhalf.bytes[0] = cur1byte;
	get1byte(fptr);
	curhalf.bytes[1] = cur1byte;
#endif
	if (trace > 1)
		printf("\nin gethalfword object<%s> halfword<%.4x>\n",object,curhalf.half);
	return(curhalf.half);
}

/*
 *	getword()
 *	This routine will read the next 4 bytes in the object file from
 *	the buffer (filling the 4 byte buffer if necessary).
 *
 */
long
getword(fptr)
LDFILE *fptr;
{
	extern	long	loc;
	extern	int	oflag;
	extern	char	object[];
	extern	char	bytebuf[];
	extern	LDFILE	*t_ptr;
	extern	int	trace;
	char	temp1;
	short	byte0, byte1, byte2, byte3;
	int	i, j, bytesread;
	union {
		char	bytes[4];
		long	word;
	} curword;

	curword.word = 0;
	for(i = 0, j= 4 - bytesleft; i < bytesleft; i++, j++)
		curword.bytes[i] = bytebuf[j];
	if (bytesleft < 4) {
		bytesread = bytesleft;
		fillbuff(t_ptr);
		if ((bytesread + bytesleft) < 4) {
			fprintf(stderr,"\n%sdis:  premature EOF\n",SGS);
			exit(4);
		}
		for (i = bytesread, j= 0; i < 4; i++, j++) {
			bytesleft--;
			curword.bytes[i] = bytebuf[j];
		}
	}
	byte0 = ((short)curword.bytes[0]) & 0x00ff;
	byte1 = ((short)curword.bytes[1]) & 0x00ff;
	byte2 = ((short)curword.bytes[2]) & 0x00ff;
	byte3 = ((short)curword.bytes[3]) & 0x00ff;
	(oflag > 0)?	sprintf(object,"%s%.3o %.3o %.3o %.3o ",object,
					byte0, byte1, byte2, byte3):
			sprintf(object,"%s%.2x %.2x %.2x %.2x ",object,
					byte0, byte1, byte2, byte3);
#ifdef AR16WR
	temp1 = curword.bytes[0];
	curword.bytes[0] = curword.bytes[2];
	curword.bytes[2] = temp1;
	temp1 = curword.bytes[1];
	curword.bytes[1] = curword.bytes[3];
	curword.bytes[3] = temp1;
#endif
#ifdef AR32WR
	temp1 = curword.bytes[0];
	curword.bytes[0] = curword.bytes[3];
	curword.bytes[3] = temp1;
	temp1 = curword.bytes[1];
	curword.bytes[1] = curword.bytes[2];
	curword.bytes[2] = temp1;
#endif
	loc += 4;
	if (trace > 1)
		printf("\nin getword object<%s>> word<%.8lx>\n",object,curword.word);
	return(curword.word);
}

/*
 *	cnvrtlng (num, temp, flag)
 *
 *	Convert the passed number to either hex or octal, depending on
 *	the oflag, leaving the result in the supplied string array.
 *	If  LEAD  is specified, preceed the number with '0' or '0x' to
 *	indicate the base (used for information going to the mnemonic
 *	printout).  NOLEAD  will be used for all other printing (for
 *	printing the offset, object code, and the second byte in two
 *	byte immediates, displacements, etc.) and will assure that
 *	there are leading zeros.
 */

cnvrtlng(num,temp,flag)
long	num;
char	temp[];
int	flag;

{
	extern	int	oflag;		/* in _extn.c */

	if (flag == LEAD)
		(oflag) ?	sprintf(temp,"&0%lo",num):
				sprintf(temp,"&%lx",num);
}
/*
 * 	longprint
 *	simply a routine to print a long constant with an optional
 *	prefix string such as "*" or "$" for operand descriptors
 */
longprint(result,prefix,value)
char	*result;
char	*prefix;
long	value;
{
	extern	int	oflag;

	if(oflag){
		sprintf(result,"%s0%lo",prefix,value);
		}
	else{
		sprintf(result,"%s%lx",prefix,value);
		}
	return;
}

/*
 *	fillbuff()
 *
 *	This routine will read 4 bytes from the object file into the 
 *	4 byte buffer.
 *	The bytes will be stored in the buffer in the correct order
 *	for the disassembler to process them. This requires a knowledge
 *	of the type of host machine on which the disassembler is being
 *	executed (AR32WR = vax, AR32W = maxi or 3B, AR16WR = 11/70), as
 *	well as a knowledge of the target machine (FBO = forward byte
 *	ordered, RBO = reverse byte ordered).
 *
 */
fillbuff()
{
	extern	char	bytebuf[];
	extern	LDFILE	*t_ptr;
	long tlong;

bytesleft = FREAD( &tlong, 1,  4, t_ptr);
	switch (bytesleft) {
	case 0:
	case 4:
		break;
	case 1:
		bytebuf[1] = bytebuf[2] = bytebuf[3] = 0;
		break;
	case 2:
		bytebuf[2] = bytebuf[3] = 0;
		break;
	case 3:
		bytebuf[3] = 0;
		break;
	}
	/* NOTE		The bytes have been read in the correct order
	 *		if one of the following is true:
	 *
	 *		host = AR32WR  and  target = FBO
	 *			or
	 *		host = AR32W   and  target = RBO
	 *
	 */

#if (RBO && AR32WR) || (FBO && AR32W)
	bytebuf[0] = (char)((tlong >> 24) & 0x000000ffL);
	bytebuf[1] = (char)((tlong >> 16) & 0x000000ffL);
	bytebuf[2] = (char)((tlong >>  8) & 0x000000ffL);
	bytebuf[3] = (char)( tlong        & 0x000000ffL);
#endif

#if (FBO && AR32WR) || (RBO && AR32W)
	bytebuf[0] = (char)( tlong        & 0x000000ffL);
	bytebuf[1] = (char)((tlong >>  8) & 0x000000ffL);
	bytebuf[2] = (char)((tlong >> 16) & 0x000000ffL);
	bytebuf[3] = (char)((tlong >> 24) & 0x000000ffL);
#endif

#if RBO && AR16WR
	bytebuf[0] = (char)((tlong >>  8) & 0x000000ffL);
	bytebuf[1] = (char)( tlong        & 0x000000ffL);
	bytebuf[2] = (char)((tlong >> 24) & 0x000000ffL);
	bytebuf[3] = (char)((tlong >> 16) & 0x000000ffL);
#endif
#if FBO && AR16WR
	bytebuf[0] = (char)((tlong >> 16) & 0x000000ffL);
	bytebuf[1] = (char)((tlong >> 24) & 0x000000ffL);
	bytebuf[2] = (char)( tlong        & 0x000000ffL);
	bytebuf[3] = (char)((tlong >>  8) & 0x000000ffL);
#endif
}

