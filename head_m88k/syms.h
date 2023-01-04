#ifndef _SYMS_H_
#define _SYMS_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

	/* Storage Classes are defined in storclass.h  */
#ifndef __88000_OCS_DEFINED
#include "storclass.h"
#endif	/* !__88000_OCS_DEFINED */

	/* Number of characters in a symbol name */
#define  SYMNMLEN	8

	/* Number of characters in a file name */
#define  FILNMLEN	14

	/* Number of array dimensions in auxiliary entry */
#define  DIMNUM		4
#define	 DIM1NUM	2
#define  DIM2NUM	5

struct syment
{
	union 
	{
		char		_n_name[SYMNMLEN];	/* old COFF version  */
		struct {
			long	_n_zeroes;	/* new == 0 */
			long	_n_offset;	/* offset into string table  */
		} _n_n;
		char		* _n_nptr[2];	/* allows for overlaying     */
	} _n;
	long		n_value;	/* value of symbol */
	short		n_scnum;	/* section number */
	unsigned short	n_type;		/* type and derived type */
	char		n_sclass;	/* storage class */
	char		n_numaux;	/* number of aux. entries */
	short		n_dummy;	/* pad to 4 byte multiple */
};

#define n_name 		_n._n_name
#define n_nptr		_n._n_nptr[1]
#define n_zeroes	_n._n_n._n_zeroes
#define n_offset	_n._n_n._n_offset

/*
 * Relocatable symbols have a section number of the
 * section in which they are defined.  Otherwise, section
 * numbers have the following meanings:
 */
         /* undefined symbol */
#define  N_UNDEF	0
        /* value of symbol is absolute */
#define  N_ABS		-1
        /* special debugging symbol -- value of symbol is meaningless */
#define  N_DEBUG	-2

/*
 *   The fundamental type of a symbol packed into the
 *   low 4 bits of the word.
 */

#define  _EF	".ef"

#define  T_NULL     0
#define  T_ARG      1          /* function argument (only used by compiler) */
#define  T_CHAR     2          /* character */
#define  T_SHORT    3          /* short integer */
#define  T_INT      4          /* integer */
#define  T_LONG     5          /* long integer */
#define  T_FLOAT    6          /* floating point */
#define  T_DOUBLE   7          /* double word */
#define  T_STRUCT   8          /* structure  */
#define  T_UNION    9          /* union  */
#define  T_ENUM     10         /* enumeration  */
#define  T_MOE      11         /* member of enumeration */
#define  T_UCHAR    12         /* unsigned character */
#define  T_USHORT   13         /* unsigned short */
#define  T_UINT     14         /* unsigned integer */
#define  T_ULONG    15         /* unsigned long */

/*
 * derived types are:
 */

#define  DT_NON      0          /* no derived type */
#define  DT_PTR      1          /* pointer */
#define  DT_FCN      2          /* function */
#define  DT_ARY      3          /* array */

/*
 *   type packing constants
 */

#define  N_BTMASK     017
#define  N_TMASK      060
#define  N_TMASK1     0300
#define  N_TMASK2     0360
#define  N_BTSHFT     4
#define  N_TSHIFT     2

/*
 *   MACROS
 */

	/* Basic Type of x */

#define  BTYPE(x)  ((x) & N_BTMASK)

	/* Is x a pointer? */

#define  ISPTR(x)  (((x) & N_TMASK) == (DT_PTR << N_BTSHFT))

	/* Is x a function? */

#define  ISFCN(x)  (((x) & N_TMASK) == (DT_FCN << N_BTSHFT))

	/* Is x an array? */

#define  ISARY(x)  (((x) & N_TMASK) == (DT_ARY << N_BTSHFT))

	/* Is x a structure, union, or enumeration TAG? */

#define ISTAG(x)  ((x)==C_STRTAG || (x)==C_UNTAG || (x)==C_ENTAG)
#define INCREF(x) ((((x)&~N_BTMASK)<<N_TSHIFT)|(DT_PTR<<N_BTSHFT)|(x&N_BTMASK))
#define DECREF(x) ((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))

/*	Defines for "special" symbols   */

#define _ETEXT	"_etext"
#define	_EDATA	"_edata"
#define	_END	"_end"
#define _START	"_start"

/*
 *	AUXILIARY ENTRY FORMAT
 */

union auxent {
	struct {
		long		x_tagndx;	/* str, un, or enum tag indx */
		union {
			struct {
				unsigned long	x_lnno;	/* declaration line number */
				unsigned long	x_size;	/* str, union, array size */
			} x_lnsz;
			long	x_fsize;	/* size of function */
		} x_misc;
		union {
			struct {	/* if ISFCN, tag, or .bb */
				long	x_lnnoptr;	/* ptr to fcn line # */
				long	x_endndx;	/* entry ndx past block end */
			} 	x_fcn;
			struct {	/* if ISARY, up to 4 dimen. */
				unsigned short	x_dimen[DIMNUM];
			} 	x_ary;
			struct {
				unsigned long x_dimen1[DIM1NUM];
			} 	x_ary1;
		}	x_fcnary;
	} 	x_sym;
	struct {
		unsigned long x_dimen2[DIM2NUM];
	} 	x_ary2;
	union
	{
		char	        x_fname[FILNMLEN];
		struct
		{
			long	_x_zeroes;
			long	_x_offset;
		} _x_x;
	} 	x_file;
        struct {
                long            x_scnlen;  /* section length  		   */
                unsigned long  x_nreloc;  /* number of relocation entries */
                unsigned long  x_nlinno;  /* number of line numbers       */
        }       x_scn;
};

#define	x_zeroes	_x_x._x_zeroes
#define	x_offset	_x_x._x_offset

#define	SYMENT	struct syment
#define	SYMESZ	20		           /* sizeof(SYMENT) */

#define	AUXENT	union auxent
#define	AUXESZ	20		           /* sizeof(AUXENT) */

#ifndef __88000_OCS_DEFINED
/*	Defines for "special" symbols   */

#define _TORIGIN	"_torigin"
#define _DORIGIN	"_dorigin"

#define _SORIGIN	"_sorigin"
#endif	/* !__88000_OCS_DEFINED */

#endif	/* ! _SYMS_H_ */
