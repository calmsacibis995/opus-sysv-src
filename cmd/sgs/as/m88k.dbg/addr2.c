/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


#include <stdio.h>
#include <reloc.h>
#include <storclass.h>
#include <syms.h>
#include "systems.h"
#include "gendefs.h"
#include "symbols.h"
#include "codeout.h"
#include "scnhdr.h"
#include "instab.h"
#include "errors.h"
#include "dbug.h"

#ifndef NO_DBUG
extern char *symprt ();
#endif

extern unsigned short relent;
extern long newdot;
extern FILE *fdrel;
extern FILE *fdcode;
extern upsymins *lookup ();
extern void as_error ();
extern void codgen ();
extern void define ();
extern void setval ();
extern void settyp ();
extern void setscl ();
extern void settag ();
extern void setlno ();
extern void setsiz ();
extern void inline ();
extern void setdim1 ();
extern void setdim2 ();
extern void endef ();
extern void lineno ();
extern void linenum ();
extern void lineval ();
extern void newstmt ();
extern void setfile ();
extern void dotzero ();
extern struct scnhdr sectab[];
extern symbol symtab[];
extern symbol *dot;
extern BYTE *longsdi;
extern short filedef;

long swapb4 (val)
register long val;
{
    register long rtnval = 0;

    DBUG_ENTER ("swapb4");
    rtnval |= (val >> 24) & 0x000000FFL;	/* high goes to low */
    rtnval |= (val >> 8) & 0x0000FF00L;		/* 2nd high goes to 2nd low */
    rtnval |= (val << 8) & 0x00FF0000L;		/* 2nd low goes to 2nd high */
    rtnval |= (val << 24) & 0xFF000000L;	/* low goes to high */
    DBUG_RETURN (rtnval);
}

static void relocat (sym, code, rtype, offset)
register symbol *sym;
codebuf *code;
register short rtype;
unsigned short offset;
{
    char *rsym;
    prelent trelent;
    register short stype;

    DBUG_ENTER ("relocat");
    if (sym != NULLSYM) {
	DBUG_PRINT ("rel1", ("%s", symprt (sym)));
	stype = sym -> styp & TYPE;
	switch (stype) {
	    case ABS: 
	        DBUG_PRINT ("symtype", ("found an ABS symbol"));
	        DBUG_VOID_RETURN;
	    case TXT: 
	    case DAT: 
	    case BSS: 
	        DBUG_PRINT ("symtype", ("found a TXT, DAT, or BSS symbol"));
		/* The old method pointed to .text, .data, or .bbs */
		/* symbols in the symbol table. */
		/* rsym = sectab[sym -> sectnum].s_name; */
		rsym = sym -> _name.name;
		break;
	    case UNDEF: 
	        DBUG_PRINT ("symtype", ("found an UNDEF symbol"));
		if (sym -> styp != SECTION) {
		    /* make sure it gets in symbol table */
		    sym -> styp |= EXTERN;
		}
		rsym = sym -> _name.name;
		break;
	    default: 
		as_error (ERR_INVTYP);
		break;
	}
    } else {
	rsym = NULL;
    }
#ifdef OLDWAY
    trelent.relval = newdot & ~3;	/* ~3 is 88000 hack */
#else
    trelent.relval = newdot;
#endif
    trelent.relname = rsym;
    trelent.reltype = rtype;
    trelent.reloffset = offset;
    (void) fwrite ((char *) &trelent, sizeof (prelent), 1, fdrel);
    ++relent;
    DBUG_VOID_RETURN;
}

void vrt32 (sym, code)
symbol *sym;
codebuf *code;

{
    DBUG_ENTER ("vrt32");
    if (sym != NULLSYM) {
	DBUG_PRINT ("symprt", ("%s", symprt (sym)));
	if ((sym-> styp & TYPE) != ABS) {
	    relocat (sym, code, R_VRT32, 0);
	} else {
	    DBUG_PRINT ("abs", ("old value = %#lx", code -> cvalue));
	    code -> cvalue = sym -> value;
	    DBUG_PRINT ("abs", ("new value = %#lx", code -> cvalue));
	}
    }
    DBUG_VOID_RETURN;
}

/*
 *	The 16 bit direct address is for the lower 16 bits of
 *	the 32 bit word whose address is placed in the object file's
 *	relocation entry.  I.E, the relocation entry points to the
 *	instruction which must have the lower 16 bits relocated.
 *
 */

void vrt16 (sym, code)
symbol *sym;
codebuf *code;

{
    DBUG_ENTER ("vrt16");
    if (sym == NULLSYM) {
	DBUG_PRINT ("vrt16", ("generate reloc entry for constant only"));
	relocat (sym, code, R_VRT16, 0);
    } else {
	DBUG_PRINT ("symprt", ("%s", symprt (sym)));
	if ((sym-> styp & TYPE) != ABS) {
	    relocat (sym, code, R_VRT16, 0);
	} else {
	    DBUG_PRINT ("abs", ("old value = %#lx", code -> cvalue));
	    /* MV. offsets are unsigned 16 bits 
	    if (sym -> value >= (1L << 15) || sym -> value < -(1L << 15)) 
	    */
	    if (sym -> value >= (1L << 16) || sym -> value < 0) 
	    {
		as_error (ERR_OVF16, sym -> value);
	    } else {
		code -> cvalue = sym -> value;
	    }
	    DBUG_PRINT ("abs", ("new value = %#lx", code -> cvalue));
	}
    }
    DBUG_VOID_RETURN;
}


void hvrt16 (sym, code)
symbol *sym;
codebuf *code;

#if EXPR
{
    DBUG_ENTER ("hvrt16");
    if (sym == NULLSYM) {
	DBUG_PRINT ("hvrt16", ("generate reloc entry for constant only"));
 	relocat (sym, code, R_HVRT16, 0);
    } else {
	DBUG_PRINT ("symprt", ("%s", symprt (sym)));
	if ((sym-> styp & TYPE) != ABS) {
	    relocat (sym, code, R_HVRT16, 0 );
	} else {
	    DBUG_PRINT ("abs", ("old value = %#lx", code -> cvalue));
            /* Bug fix.  9/2/88  kss */
	    code -> cvalue = sym -> value & 0xffff0000;
	    code -> cvalue >>= 16;
	    DBUG_PRINT ("abs", ("new value = %#lx", code -> cvalue));
	}
    }
    DBUG_VOID_RETURN;
}

#else
{
unsigned long hioffset;

    DBUG_ENTER ("hvrt16");
    if (sym != NULLSYM) {
	DBUG_PRINT ("symprt", ("%s", symprt (sym)));
	if ((sym-> styp & TYPE) != ABS) {
	    hioffset = code -> cvalue & 0xffff0000;
	    hioffset >>= 16;
	    code -> cvalue = code -> cvalue & 0x0000ffff;
	    relocat (sym, code, R_HVRT16, hioffset );
	} else {
	    DBUG_PRINT ("abs", ("old value = %#lx", code -> cvalue));
            /* Bug fix.  9/2/88  kss */
	    code -> cvalue = sym -> value & 0xffff0000;
	    code -> cvalue >>= 16;
	    DBUG_PRINT ("abs", ("new value = %#lx", code -> cvalue));
	}
    }
    DBUG_VOID_RETURN;
}
#endif


void lvrt16 (sym, code)
symbol *sym;
codebuf *code;

#if EXPR
{
    DBUG_ENTER ("lvrt16");
    if (sym == NULLSYM) {
 	DBUG_PRINT ("lvrt16", ("generate reloc entry for constant only"));
 	relocat (sym, code, R_LVRT16, 0);
    } else {
	DBUG_PRINT ("symprt", ("%s", symprt (sym)));
	if ((sym-> styp & TYPE) != ABS) {
	    relocat (sym, code, R_LVRT16, 0);
	} else {
	    DBUG_PRINT ("abs", ("old value = %#lx", code -> cvalue));
		/* Bug fix.  9/2/88  kss */
	        code -> cvalue = sym -> value & 0x0000ffff;
	    DBUG_PRINT ("abs", ("new value = %#lx", code -> cvalue));
	}
    }
    DBUG_VOID_RETURN;
}

#else
{
unsigned long hioffset;

    DBUG_ENTER ("lvrt16");
    if (sym != NULLSYM) {
	DBUG_PRINT ("symprt", ("%s", symprt (sym)));
	if ((sym-> styp & TYPE) != ABS) {
	    hioffset = code -> cvalue & 0xffff0000;
	    hioffset >>= 16;
	    code -> cvalue = code -> cvalue & 0x0000ffff;
	    relocat (sym, code, R_LVRT16, hioffset );
	} else {
	    DBUG_PRINT ("abs", ("old value = %#lx", code -> cvalue));
		/* Bug fix.  9/2/88  kss */
	        code -> cvalue = sym -> value & 0x0000ffff;
	    DBUG_PRINT ("abs", ("new value = %#lx", code -> cvalue));
	}
    }
    DBUG_VOID_RETURN;
}
#endif


void resabs (sym, code)
register symbol *sym;
codebuf *code;
{
    DBUG_ENTER ("resabs");
    DBUG_PRINT ("symprt", ("%s", symprt (sym)));
    switch (sym -> styp & TYPE) {
	case ABS: 
	    DBUG_PRINT ("sym", ("found an ABS symbol"));
	    code -> cvalue += sym -> value;	/* sym must be non-null */
	    break;
	case UNDEF: 
	    DBUG_PRINT ("sym", ("found an UNDEF symbol"));
	    as_error (ERR_YYERROR, "Undefined symbol in absolute expression");
	    break;
	default: 
	    as_error (ERR_YYERROR, "Relocatable symbol in absolute expression");
	    break;
    }
    DBUG_VOID_RETURN;
}

void relpc8 (sym, code)
register symbol *sym;
register codebuf *code;
{
    register long val;
    register short stype;

    DBUG_ENTER ("relpc8");
    val = code -> cvalue;
    if (sym != NULLSYM) {
	DBUG_PRINT ("symprt", ("%s", symprt (sym)));
	if ((stype = sym -> styp & TYPE) != dot -> styp && stype != ABS) {
	    as_error (ERR_RSIAS);
	} else {
	    val += sym -> value;
	}
    }
    if ((val -= dot -> value) >= (1L << 7) || val < -(1L << 7)) {
	as_error (ERR_PCOVF8, val);
    }
    code -> cvalue = val;
    DBUG_VOID_RETURN;
}

/*
 *	The 26 bit PC relative address is for the lower 26 bits of
 *	the 32 bit word whose address is placed in the object file's
 *	relocation entry.  I.E, the relocation entry points to the
 *	instruction which must have the lower 26 bits relocated.
 *	The lower 26 bits represent a LONG WORD address, I.E, the
 *	upper 26 bits of a 28 bit byte address.
 *
 */

void pcr26l (sym, code)
register symbol *sym;
register codebuf *code;

#ifdef EXPR
{
    DBUG_ENTER ("pcr26l");
    if (sym == NULLSYM) {
	DBUG_PRINT ("pcr26l", ("generate reloc entry for constant only"));
	relocat (sym, code, R_PCR26L, 0);
    } else {
	DBUG_PRINT ("symprt", ("%s", symprt (sym)));
	if ((sym-> styp & TYPE) != ABS) {
	    relocat (sym, code, R_PCR26L, 0);
	} else {
	    DBUG_PRINT ("abs", ("old value = %#lx", code -> cvalue));
	    if (sym -> value >= (1L << 25) || sym -> value < -(1L << 25)) {
		as_error (ERR_PCOVF26, sym -> value);
	    } else {
		code -> cvalue = sym -> value >> 2;
	    }
	    DBUG_PRINT ("abs", ("new value = %#lx", code -> cvalue));
	}
    }
    DBUG_VOID_RETURN;
}

#else
/* Resolve all PCR26L relocations possible.  kss  8/88 */
{
	register long offset;

	if (sym == NULLSYM){
		relocat (sym, code, R_PCR26L, 0);
	}
	else {
		if  ((sym -> sectnum) == (dot -> sectnum)) {
			offset = sym -> value - newdot;
			offset >>= 2;
			offset &= 0x03ffffff;
			code->cvalue = offset;
			}
		else if ((sym-> styp & TYPE) == (ABS)) { 
			if (sym -> value >= (1L << 25) || sym -> value < -(1L << 25)) {
				as_error (ERR_PCOVF26, sym -> value);
			}
	   		else {
				code -> cvalue = sym -> value >> 2;
	    		}
		}
		else
			relocat (sym, code, R_PCR26L, 0);
	}
	return;
}
#endif

/*
 *	The 16 bit PC relative address is for the lower 16 bits of
 *	the 32 bit word whose address is placed in the object file's
 *	relocation entry.  I.E, the relocation entry points to the
 *	instruction which must have the lower 16 bits relocated.
 *	The lower 16 bits represent a LONG WORD address, I.E, the
 *	upper 16 bits of a 18 bit byte address.
 *
 */

void pcr16l (sym, code)
register symbol *sym;
register codebuf *code;

#ifdef EXPR
{
    DBUG_ENTER ("pcr16l");
    if (sym == NULLSYM) {
	DBUG_PRINT ("pcr16l", ("generate reloc entry for constant only"));
	relocat (sym, code, R_PCR16L, 0);
    } else {
	DBUG_PRINT ("symprt", ("%s", symprt (sym)));
	if ((sym-> styp & TYPE) != ABS) {
	    relocat (sym, code, R_PCR16L, 0);
	} else {
	    DBUG_PRINT ("abs", ("old value = %#lx", code -> cvalue));
	    if (sym -> value >= (1L << 15) || sym -> value < -(1L << 15)) {
		as_error (ERR_PCOVF16, sym -> value);
	    } else {
		code -> cvalue = sym -> value >> 2;
	    }
	    DBUG_PRINT ("abs", ("new value = %#lx", code -> cvalue));
	}
    }
    DBUG_VOID_RETURN;
}

#else
/* Resolve all PCR16L relocations possible.  kss  8/88 */
{
	register long offset;

	if (sym == NULLSYM) {
		relocat (sym, code, R_PCR16L, 0);
	}
	else {
		if  ((sym -> sectnum) == (dot -> sectnum)) {
			offset = sym -> value - newdot;
			offset >>= 2;
			offset ++;
			offset &= 0x0000ffff;
			code->cvalue = offset;
		}
		else if ((sym-> styp & TYPE) == (ABS)) {
	    		if (sym -> value >= (1L << 15) || sym -> value < -(1L << 15)) {
				as_error (ERR_PCOVF16, sym -> value);
			}
	    		else {
				code -> cvalue = sym -> value >> 2;
			}
		}
		else
			relocat (sym, code, R_PCR16L, 0);
	}
	return;
}
#endif

#ifdef CALLPCREL

void callnopt (sym, code)
register symbol *sym;
register codebuf *code;
{
    DBUG_ENTER ("callnopt");
    if (*++longsdi) {
	codgen (8, (long) CABSMD);/* descriptor for operand */
	(void) getcode (code);
	if (code -> cindex) {
	    sym = symtab + (code -> cindex - 1);
	} else {
	    sym = (symbol *) NULL;
	}
	code -> cnbits = 32;
	vrt32 (sym, code);
    }
    DBUG_VOID_RETURN;
}

#endif	/* CALLPCREL */

/*ARGSUSED*/
void fndbrlen (sym, code)
symbol *sym;
register codebuf *code;
{
    register long opcd;

    DBUG_ENTER ("fndbrlen");
    (void) getcode (code);
    opcd = code -> cvalue;
    codgen (8, (long) (3 + oplen (code)));
    dot -> value = newdot;	/* resynchronize */
    codgen (8, opcd);
    /* now return with descriptor of operand in "code" */
    DBUG_VOID_RETURN;
}

int oplen (code)
register codebuf *code;
{
    static int  opndlen[2][16] = {
	{
	    1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 3, 3, 2, 2, 0, 1
	},			/*  < 15 */
	{
	    1, 1, 1, 1, 5, 3, 2, 5, 5, 5, 3, 3, 2, 2, 5, 1
	}			/* == 15 */
    };
    register int desc;

    DBUG_ENTER ("oplen");
    (void) getcode (code);
    desc = (int) (code -> cvalue);
    DBUG_RETURN (opndlen[(desc & 0xF) == 0xF][desc >> 4]);
}

void shiftval (sym, code)
symbol *sym;
register codebuf *code;
{
    DBUG_ENTER ("shiftval");
    resabs (sym, code);		/* symbol must be absolute */
    if (code -> cvalue < 1L || code -> cvalue > 31L) {
	as_error (ERR_YYERROR, "Bit position out of range");
    }
    code -> cvalue = swapb4 ((long) (1L << code -> cvalue));	/* shift and swap */
    DBUG_VOID_RETURN;
}

int (*(modes[NACTION + 2])) () = {
     /*0*/  (int (*)()) 0,
     /*1*/  (int (*)()) define,
     /*2*/  (int (*)()) setval,
     /*3*/  (int (*)()) setscl,
     /*4*/  (int (*)()) settyp,
     /*5*/  (int (*)()) settag,
     /*6*/  (int (*)()) setlno,
     /*7*/  (int (*)()) setsiz,
     /*8*/  (int (*)()) setdim1,
     /*9*/  (int (*)()) setdim2,
     /*10*/ (int (*)()) endef,
     /*11*/ (int (*)()) lineno,
     /*12*/ (int (*)()) linenum,
     /*13*/ (int (*)()) lineval,
     /*14*/ (int (*)()) newstmt,
     /*15*/ (int (*)()) setfile,
     /*16*/ (int (*)()) lvrt16,		/* Direct reference, low 16 of 32 */
     /*17*/ (int (*)()) resabs,
     /*18*/ (int (*)()) relpc8,
     /*19*/ (int (*)()) 0,		/* Unused */
     /*20*/ (int (*)()) 0,		/* Unused */
     /*21*/ (int (*)()) 0,		/* Unused */
     /*22*/ (int (*)()) fndbrlen,
     /*23*/ (int (*)()) shiftval,
     /*24*/ (int (*)()) vrt32,		/* Direct reference, 32 bits */
     /*25*/ (int (*)()) pcr16l,		/* PC relative, 16 bits, long align */
     /*26*/ (int (*)()) inline,
     /*27*/ (int (*)()) dotzero,
     /*28*/ (int (*)()) pcr26l,		/* PC relative, 26 bits, long align */
     /*29*/ (int (*)()) vrt16,		/* Direct reference, 16 bits */
     /*30*/ (int (*)()) hvrt16,		/* Direct reference, high 16 of 32 */
     /*31*/ (int (*)()) 0,		/* Unused */
#ifdef	CALLPCREL
     /*32*/ (int (*)()) callnopt,
#endif
	    (int (*)()) 0
};

