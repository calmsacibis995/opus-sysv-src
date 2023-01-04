#ident "@(#) $Header: syms.c 4.1 88/04/26 17:03:09 root Exp $ RISC Source Base"
#include <stdio.h>
#include <a.out.h>

/*
 *	On System V machines, <a.out.h> automatically includes the
 *	following files.  If this is a BSD host instead, then 
 *	explicitly include them and hope that our include path
 *	points somewhere where they will be found.
 */

#ifndef SYMENT
#undef N_ABS
#include "syms.h"
#endif

#include "coffdump.h"

struct tran {
    CHAR *name;
    SHORT value;
};

static struct tran sections[] = {
    TRANSPAIR (N_UNDEF),
    TRANSPAIR (N_ABS),
    TRANSPAIR (N_DEBUG),
    TRANSPAIR (N_TV),
    TRANSPAIR (P_TV),
    NULL, 0
};

static struct tran classes[] = {
    TRANSPAIR (C_EFCN),
    TRANSPAIR (C_NULL),
    TRANSPAIR (C_AUTO),
    TRANSPAIR (C_EXT),
    TRANSPAIR (C_STAT),
    TRANSPAIR (C_REG),
    TRANSPAIR (C_EXTDEF),
    TRANSPAIR (C_LABEL),
    TRANSPAIR (C_ULABEL),
    TRANSPAIR (C_MOS),
    TRANSPAIR (C_ARG),
    TRANSPAIR (C_STRTAG),
    TRANSPAIR (C_MOU),
    TRANSPAIR (C_UNTAG),
    TRANSPAIR (C_TPDEF),
    TRANSPAIR (C_USTATIC),
    TRANSPAIR (C_ENTAG),
    TRANSPAIR (C_MOE),
    TRANSPAIR (C_REGPARM),
    TRANSPAIR (C_FIELD),
    TRANSPAIR (C_BLOCK),
    TRANSPAIR (C_FCN),
    TRANSPAIR (C_EOS),
    TRANSPAIR (C_FILE),
    TRANSPAIR (C_LINE),
    TRANSPAIR (C_ALIAS),
    TRANSPAIR (C_HIDDEN),
    NULL, 0
};

static CHAR pbuf [128];

static int aux_filename ();
static int aux_section ();
static int aux_tagname ();
static int aux_eostruct ();
static int aux_function ();
static int aux_array ();
static int aux_boblock ();
static int aux_eoblock ();
static int aux_bofunc ();
static int aux_eofunc ();
static int aux_sue ();

extern CHAR *LongTuple ();	/* Build decimal/octal/hex ascii string */
extern CHAR *IntTuple ();	/* Build decimal/octal/hex ascii string */
extern CHAR *UIntTuple ();	/* Build decimal/octal/hex ascii string */

/*
 *	Symbol table entries may be followed by an auxiliary entry.
 *	Information from the symbol table entry is used to determine
 *	what the structure of the auxiliary entry is going to be.
 *	The following structure, defines, and translation table
 *	is used to select the type of aux entry, based on information
 *	from the current entry.  The information here may be found in
 *	the document describing the common object file format.
 */

#define AUX_UNKNOWN	0	/* Unknown type of aux entry */
#define AUX_FILENAME	1	/* Aux entry for a file name */
#define AUX_SECTION	2	/* Aux entry for a section */
#define AUX_TAGNAME	3	/* Aux entry for a tag name */
#define AUX_EOSTRUCT	4	/* Aux entry for end of structure */
#define AUX_FUNCTION	5	/* Aux entry for a function */
#define AUX_ARRAY	6	/* Aux entry for an array */
#define AUX_BOBLOCK	7	/* Aux entry for beginning of block */
#define AUX_EOBLOCK	8	/* Aux entry for end of block */
#define AUX_BOFUNC	9	/* Aux entry for beginning of function */
#define AUX_EOFUNC	10	/* Aux entry for end of function */
#define AUX_SUE 	11	/* Aux entry for struct, union, or enum */

#define NAME(a) (strncmp (a, name, SYMNMLEN) == 0)


/*
 *  FUNCTION
 *
 *	TransSection    translate section number to symbolic name
 *
 *  SYNOPSIS
 *
 *	CHAR *TransSection (value)
 *	SHORT value;
 *
 *  DESCRIPTION
 *
 *	Given a numeric value <value> from the n_scnum field in
 *	the symbol table entry, translates the numeric value to
 *	a symbolic value.  If the corresponding symbol is not
 *	found for any reason, a pointer to a static buffer containing
 *	a printable ascii string with the matching value of <value>
 *	is returned.
 *
 */
 
CHAR *TransSection (value)
SHORT value;
{
    register struct tran *tp;
    register CHAR *name;
    
    DBUG_ENTER ("TransSection");
    name = NULL;
    for (tp = sections; tp -> name != NULL; tp++) {
	if (tp -> value == value) {
	    name = tp -> name;
	    break;
	}
    }
    if (name == NULL) {
	sprintf (pbuf, "%u", value);
	name = pbuf;
    }
    DBUG_RETURN (name);
}


/*
 *  FUNCTION
 *
 *	TransClass    translate storage class to symbolic name
 *
 *  SYNOPSIS
 *
 *	CHAR *TransClass (value)
 *	CHAR value;
 *
 *  DESCRIPTION
 *
 *	Given a numeric value <value> from the n_sclass field in
 *	the symbol table entry, translates the numeric value to
 *	a symbolic value.  If the corresponding symbol is not
 *	found for any reason, a pointer to a static buffer containing
 *	a printable ascii string with the matching value of <value>
 *	is returned.
 *
 */
 
CHAR *TransClass (value)
CHAR value;
{
    register struct tran *tp;
    register CHAR *name;
    
    DBUG_ENTER ("TransClass");
    name = NULL;
    for (tp = classes; tp -> name != NULL; tp++) {
	if (tp -> value == value) {
	    name = tp -> name;
	    break;
	}
    }
    if (name == NULL) {
	sprintf (pbuf, "%u", value);
	name = pbuf;
    }
    DBUG_RETURN (name);
}

int AuxType (symp)
SYMENT *symp;
{
    register int auxtype = AUX_UNKNOWN;
    register char *name;
    register int sclass;
    register int d1;
    register int btype;

    DBUG_ENTER ("AuxType");
    name = symp -> n_name;
    sclass = symp -> n_sclass;
    d1 = ((symp -> n_type) & N_TMASK) >> N_BTSHFT;
    btype = BTYPE (symp -> n_type);
    if (aux_filename (name, sclass, d1, btype)) {
	auxtype = AUX_FILENAME;
    } else if (aux_section (name, sclass, d1, btype)) {
	auxtype = AUX_SECTION;
    } else if (aux_tagname (name, sclass, d1, btype)) {
	auxtype = AUX_TAGNAME;
    } else if (aux_eostruct (name, sclass, d1, btype)) {
	auxtype = AUX_EOSTRUCT;
    } else if (aux_function (name, sclass, d1, btype)) {
	auxtype = AUX_FUNCTION;
    } else if (aux_array (name, sclass, d1, btype)) {
	auxtype = AUX_ARRAY;
    } else if (aux_boblock (name, sclass, d1, btype)) {
	auxtype = AUX_BOBLOCK;
    } else if (aux_eoblock (name, sclass, d1, btype)) {
	auxtype = AUX_EOBLOCK;
    } else if (aux_bofunc (name, sclass, d1, btype)) {
	auxtype = AUX_BOFUNC;
    } else if (aux_eofunc (name, sclass, d1, btype)) {
	auxtype = AUX_EOFUNC;
    } else if (aux_sue (name, sclass, d1, btype)) {
	auxtype = AUX_SUE;
    }
    DBUG_3 ("auxtype", "auxtype = %d", auxtype);
    DBUG_RETURN (auxtype);
}

static int aux_filename (name, sclass, d1, btype)
char *name;
int sclass;
int d1;
int btype;
{
    auto int result = 0;

    if (strncmp (name, ".file", SYMNMLEN) == 0) {
	if (sclass == C_FILE) {
	    if (d1 == DT_NON) {
		if (btype == T_NULL) {
		    result = 1;
		}
	    }
	}
    }
    return (result);
}

static int aux_section (name, sclass, d1, btype)
char *name;
int sclass;
int d1;
int btype;
{
    auto int result = 0;

    if (NAME (".text") || NAME (".data") || NAME (".bss")) {
	if (sclass == C_STAT) {
	    if (d1 == DT_NON) {
		if (btype == T_NULL) {
		    result = 1;
		}
	    }
	}
    }
    return (result);
}

static int aux_tagname (name, sclass, d1, btype)
char *name;
int sclass;
int d1;
int btype;
{
    auto int result = 0;

    if (sclass == C_STRTAG || sclass == C_UNTAG || sclass == C_ENTAG) {
	if (d1 == DT_NON) {
	    if (btype == T_NULL) {
		result = 1;
	    }
	}
    }
    return (result);
}

static int aux_eostruct (name, sclass, d1, btype)
char *name;
int sclass;
int d1;
int btype;
{
    auto int result = 0;

    if (NAME ("eos")) {
	if (sclass == C_EOS) {
	    if (d1 == DT_NON) {
		if (btype == T_NULL) {
		    result = 1;
		}
	    }
	}
    }
    return (result);
}

static int aux_function (name, sclass, d1, btype)
char *name;
int sclass;
int d1;
int btype;
{
    auto int result = 0;

    if (sclass == C_EXT || sclass == C_STAT) {
	if (d1 == DT_FCN) {
	    if (btype != T_MOE) {
		result = 1;
	    }
	}
    }
    return (result);
}

static int aux_array (name, sclass, d1, btype)
char *name;
int sclass;
int d1;
int btype;
{
    auto int result = 0;

    if (sclass == C_AUTO || sclass == C_STAT || sclass == C_MOS
	|| sclass == C_MOU || sclass == C_TPDEF) {
	if (d1 == DT_ARY) {
	    if (btype != T_MOE) {
		result = 1;
	    }
	}
    }
    return (result);
}

static int aux_boblock (name, sclass, d1, btype)
char *name;
int sclass;
int d1;
int btype;
{
    auto int result = 0;

    if (NAME ("bb")) {
	if (sclass == C_BLOCK) {
	    if (d1 == DT_NON) {
		if (btype == T_NULL) {
		    result = 1;
		}
	    }
	}
    }
    return (result);
}

static int aux_eoblock (name, sclass, d1, btype)
char *name;
int sclass;
int d1;
int btype;
{
    auto int result = 0;

    if (NAME (".eb")) {
	if (sclass == C_BLOCK) {
	    if (d1 == DT_NON) {
		if (btype == T_NULL) {
		    result = 1;
		}
	    }
	}
    }
    return (result);
}

static int aux_bofunc (name, sclass, d1, btype)
char *name;
int sclass;
int d1;
int btype;
{
    auto int result = 0;

    if (NAME (".bf")) {
	if (sclass == C_FCN) {
	    if (d1 == DT_NON) {
		if (btype == T_NULL) {
		    result = 1;
		}
	    }
	}
    }
    return (result);
}

static int aux_eofunc (name, sclass, d1, btype)
char *name;
int sclass;
int d1;
int btype;
{
    auto int result = 0;

    if (NAME (".ef")) {
	if (sclass == C_FCN) {
	    if (d1 == DT_NON) {
		if (btype == T_NULL) {
		    result = 1;
		}
	    }
	}
    }
    return (result);
}

static int aux_sue (name, sclass, d1, btype)
char *name;
int sclass;
int d1;
int btype;
{
    auto int result = 0;

    if (sclass == C_AUTO || sclass == C_STAT || sclass == C_MOS
	|| sclass == C_MOU || sclass == C_TPDEF) {
	if (d1 == DT_PTR || d1 == DT_ARY || d1 == DT_NON) {
	    if (btype == T_STRUCT || btype == T_UNION || btype == T_ENUM) {
		result = 1;
	    }
	}
    }
    return (result);
}

VOID PrtAuxent (auxtype, symindex, auxp)
int auxtype;
LONG symindex;
AUXENT *auxp;
{
    printf ("\tindex:\t\t%s\n", LongTuple (symindex));
    switch (auxtype) {
	case AUX_FILENAME:
	    printf ("\tx_fname:\t\t%s\n", auxp -> x_file.x_fname);
	    break;
	case AUX_SECTION:
	    printf ("\tx_scnlen:\t%s\n", LongTuple (auxp -> x_scn.x_scnlen));
	    printf ("\tx_nreloc:\t%s\n", UIntTuple (auxp -> x_scn.x_nreloc));
	    printf ("\tx_nlinno:\t%s\n", UIntTuple (auxp -> x_scn.x_nlinno));
	    break;
	default:
	    printf ("\t<unrecognized auxiliary entry type!>\n");
	    break;
    }
    printf ("\n");
}


