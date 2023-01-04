# ifndef NOSCCS
static char *sccsid = "@(#)chiprev.c	1.13	10/15/85";
static char *cpyrid = "@(#)Copyright (C) 1984 by National Semiconductor Corp.";
# endif

#include <local/chiprev.h>

char *crev_default = "32016N";  /* <-- CUSTOMERS SHOULD CHANGE ONLY THIS */

char crev_octalarg[12] = "0"; /* changed to octal form of arg by chiprev() */

static 
struct cb_xlate { char *cb_name; unsigned long cb_code; } cb_xt[] = {

"32032D",	CB_ABTPSR | CB_ABTSPI |CB_LOCKTEXT | CB_RETMOD | CB_SLVINT |
	CB_TOS |  CB_PACKMOV | 
	CB_DIVMRMW | CB_MULMRMW | CB_IMMFBW | CB_CMPFREG | CB_RMWINDX,

"32016G",	CB_US | CB_ABTPSR | CB_ABTSPI |CB_LOCKTEXT | CB_SLVINT |
	CB_MRMW | CB_DIVMRMW | CB_MULMRMW | CB_PACKMOV | 
	CB_CHECKBW | CB_IMMFBW | CB_FPU | CB_TOS |  
	CB_CMPFREG | CB_RMWINDX | CB_STRING | CB_RETMOD,
		  
"32016K",	CB_US | CB_ABTPSR | CB_ABTSPI |CB_LOCKTEXT | CB_SLVINT |
	CB_MRMW | CB_DIVMRMW | CB_MULMRMW | CB_PACKMOV | 
	CB_CHECKBW | CB_IMMFBW | CB_FPU | CB_TOS |  
	CB_CMPFREG | CB_RMWINDX | CB_STRING | CB_RETMOD,
		
/* Sys16 with 32016K plus hardware kludges */
"SYS16K",	CB_US | CB_ABTPSR | CB_ABTSPI |CB_LOCKTEXT | CB_SLVINT |
	CB_MRMW | CB_DIVMRMW | CB_MULMRMW | CB_PACKMOV | 
	CB_CHECKBW | CB_IMMFBW | CB_FPU | CB_TOS | 
	CB_CMPFREG | CB_RMWINDX | CB_RETMOD,
		
"32016N",	CB_MULMRMW | CB_IMMFBW | CB_ABTSPI |CB_CMPFREG | CB_RMWINDX |
		CB_SHIFTTOS | CB_RETMOD,
		
"32016M",	CB_CMPFREG | CB_IMMFBW | CB_RETMOD,

"32032F1",	CB_CMPFREG | CB_IMMFBW | CB_RETMOD,

/* The KERNEL "chipname" is used when compiling kernel software which must
   be capable of executing on the "worst" chip.  Even though the kernel
   may avoid executing unnecessary software work-arounds, the compiled
   kernel code must work for the worst case.  
 */
"KERNEL",	CB_MULMRMW | CB_IMMFBW | CB_ABTSPI |CB_CMPFREG | CB_RMWINDX |
		CB_SHIFTTOS | CB_RETMOD,
	/* KERNEL == 32016N */
		  
"PERFECT",	0,

0/*end*/ };


/* Fixtab[] matches mask words with points to bugname variables. */
static
struct fixstruc { long mask; cb_flagtype *ptr; } fixtab[] = {
      	CB_MRMW,	& cb_mrmw,
	CB_MULMRMW,	& cb_mulmrmw,
	CB_DIVMRMW,	& cb_divmrmw,
	CB_PACKMOV,	& cb_packmov,
	CB_CHECKBW,	& cb_checkbw,
	CB_IMMFBW,	& cb_immfbw,
	CB_CMPFREG,	& cb_cmpfreg,
	CB_RMWINDX,     & cb_rmwindx,
	CB_STRING,	& cb_string,
	CB_FPU,		& cb_fpu,
	CB_TOS,		& cb_tos,
	CB_SHIFTTOS,	& cb_shifttos,
	CB_US,		& cb_us,
	CB_ABTPSR,	& cb_abtpsr,
	CB_ABTSPI,	& cb_abtspi,
	CB_LOCKTEXT,	& cb_locktext,
	CB_RETMOD,	& cb_retmod,
	CB_SLVINT,	& cb_slvint,
	0 };
/*
	Parse and translate chip bug command line option, and record
	which bug work-arounds are to be implemented by calling program.
	The CHIPBUG() macro is used to find out if a particular
	work-around is supposed to be implemented.

	cmdstr is either an octal number (with leading zero), or
	a "chip name".   If an octal code, each 1-bit indicates
	that the corresponding chip bug work-around is to be
	implemented.  The bits are given by #defines in chiprev.h.
	"Chip names" are internally translated to the appropriate
	set of work-arounds, using a compiled-in table.

	crev_default[] should be passed to chiprev() if no user argument
	is given.  If chiprev() is never called, a perfect chip (no bugs)
	is assumed.

	crev_octalarg[] is set to an ascii string which is the *octal* form of 
	the argument, even if a "chip name" is specified.  This may be passed 
	by nmcc to subordinate programs, rather than the original argument, so 
	that only nmcc need be relinked if the "chipname" definitions change.

	Non-zero is normally returned.
	If cmdstr has incorrect syntax (or an unrecognized "chipname"),
	zero is returned.

	NOTE: External procedures are not used because chiprev() may
	be called from within the kernel.
*/
static long accum_bitmask = 0; /* Accumulated bits. */
chiprev(cmdstr) register char *cmdstr; {
	long bitmask;
	register struct fixstruc *fxp;
	if (cmdstr==0) return(0);
	if (*cmdstr == '0') {
		register int ch;
		bitmask = 0;
		while ( (ch = *cmdstr++) ) {
			if ( ch < '0' || ch > '7' ) return(0);
			bitmask = (bitmask << 3) | (ch-'0');
		}
	} else {
		register struct cb_xlate *xp;
		for (xp=cb_xt; xp->cb_name; xp++)
			if (strcmp(xp->cb_name,cmdstr)==0) {
				bitmask = xp->cb_code;
				break;
			}
		if (xp->cb_name == 0) return(0);
	}
	accum_bitmask |= bitmask;
	if (accum_bitmask != 0) { 
		/* sprintf(crev_octalarg,"0%O",accum_bitmask); */
	  	register char *cp = crev_octalarg+1; /* leave initial zero */
	  	register long n = accum_bitmask;
		register shiftcnt;
		register long tn;
	  	for (shiftcnt=30; shiftcnt >= 0; shiftcnt -= 3) {
			/* First digit is 2 bits, others are 3... */
			tn = (n >> shiftcnt) & (shiftcnt==30 ? 03 : 07);
			if ((tn != 0) || (cp > crev_octalarg+1)) *cp++ = ('0'+tn);
		}
		*cp = 0;
	}

	for (fxp = fixtab; fxp->mask != 0; fxp++)
		if (bitmask & fxp->mask) 
			*fxp->ptr = 1;
	return(1);
}
