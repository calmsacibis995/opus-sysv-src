#ifndef __Isyms
#define __Isyms

/*		Storage Classes are defined in storclass.h	*/
#include "storclass.h"


#define  SYMNMLEN	8	/* No of characters in symbol	*/
#define  FILNMLEN	14	/* No of characters in filename	*/
#define  DIMNUM		4	/* No of array dimensions	*/

#ifdef m88k

struct syment					/* modified COFF	*/
{
	union
	{
		char		_n_name[SYMNMLEN]; /* if len <= 8	*/
		struct
		{
			long	_n_zeroes;	/* ==0 if len > 8	*/
			long	_n_offset;	/* offset into strtable	*/
		} _n_n;
		char		*_n_nptr[2];
	} _n;
	long			n_value;	/* value of symbol	*/
	short			n_scnum;	/* section number	*/
	unsigned short		n_type;		/* type and derived type*/
	char			n_sclass;	/* storage class	*/
	char			n_numaux;	/* no of aux. entries	*/
	char			n_pad[2];
};

#define	SYMENT	struct syment
#define	SYMESZ	sizeof(SYMENT)

union auxent {		/* modified COFF	*/
	struct {
		long	x_tagndx;
		union {
			struct {
				unsigned long	x_lnno;
				unsigned long	x_size;
			} x_lnsz;
			long	x_fsize;
		} x_misc;
		union {
			struct {
				long	x_lnnoptr;
				long	x_endndx;
			} x_fcn;
			struct {
				unsigned short	x_dimen[DIMNUM];
			} x_ary;
		} x_fcnary;
	} x_sym;
	union {
		char	x_fname[FILNMLEN];
		struct {
			long _x_zeroes;
			long _x_offset;
		} _x_x;
	} x_file;
        struct {
                long    x_scnlen;
                unsigned long  x_nreloc;
                unsigned long  x_nlinno;
        } x_scn;
};

#define	AUXENT	union auxent
#define	AUXESZ	sizeof(AUXENT)

#define x_zeroes _x_x._x_zeroes
#define x_offset _x_x._x_offset

#else

struct syment					/* standard COFF	*/
{
	union
	{
		char		_n_name[SYMNMLEN]; /* if len <= 8	*/
		struct
		{
			long	_n_zeroes;	/* ==0 if len > 8	*/
			long	_n_offset;	/* offset into strtable	*/
		} _n_n;
		char		*_n_nptr[2];
	} _n;
	long			n_value;	/* value of symbol	*/
	short			n_scnum;	/* section number	*/
	unsigned short		n_type;		/* type and derived type*/
	char			n_sclass;	/* storage class	*/
	char			n_numaux;	/* no of aux. entries	*/
};

#define	SYMENT	struct syment
#define	SYMESZ	18				/* sizeof(SYMENT)	*/

union auxent {		/* standard COFF	*/
	struct {
		long	x_tagndx;
		union {
			struct {
				unsigned short	x_lnno;
				unsigned short	x_size;
			} x_lnsz;
			long	x_fsize;
		} x_misc;
		union {
			struct {
				long	x_lnnoptr;
				long	x_endndx;
			} x_fcn;
			struct {
				unsigned short	x_dimen[DIMNUM];
			} x_ary;
		} x_fcnary;
		unsigned short  x_tvndx;
	} x_sym;
	struct {
		char	x_fname[FILNMLEN];
	} x_file;
        struct {
                long    x_scnlen;
                unsigned short  x_nreloc;
                unsigned short  x_nlinno;
        } x_scn;

	struct
	{
		long		x_tvfill;
		unsigned short	x_tvlen;
		unsigned short	x_tvran[2];
	} x_tv;
};

#define	AUXENT	union auxent
#define	AUXESZ	18	/* sizeof(AUXENT) */

#endif

#define n_name		_n._n_name
#define n_nptr		_n._n_nptr[1]
#define n_zeroes	_n._n_n._n_zeroes
#define n_offset	_n._n_n._n_offset

/*
   Relocatable symbols have a section number of the
   section in which they are defined.  Otherwise, section
   numbers have the following meanings:
*/

#define  N_UNDEF	0	        /* undefined symbol		*/
#define  N_ABS		-1        	/* value of symbol is absolute	*/
#define  N_DEBUG	-2       	/* special debugging symbol	*/

#define _EF	".ef"

/*
   The fundamental type of a symbol packed into the low 
   4 bits of the word.
*/

#define	T_NULL		0
#define T_ARG		1	/* function argument	*/
#define T_CHAR		2
#define T_SHORT		3
#define T_INT		4
#define T_LONG		5
#define T_FLOAT		6
#define T_DOUBLE	7
#define T_STRUCT	8
#define T_UNION		9
#define T_ENUM		10
#define T_MOE		11	/* member of enum	*/
#define T_UCHAR		12
#define T_USHORT	13
#define T_UINT		14
#define T_ULONG		15

/*
 * derived types are:
 */

#define  DT_NON      0          /* no derived type	*/
#define  DT_PTR      1          /* pointer		*/
#define  DT_FCN      2          /* function		*/
#define  DT_ARY      3          /* array		*/

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

	/*   Basic Type of  x   */

#define  BTYPE(x)  ((x) & N_BTMASK)

	/*   Is  x  a  pointer ?   */

#define  ISPTR(x)  (((x) & N_TMASK) == (DT_PTR << N_BTSHFT))

	/*   Is  x  a  function ?  */

#define  ISFCN(x)  (((x) & N_TMASK) == (DT_FCN << N_BTSHFT))

	/*   Is  x  an  array ?   */

#define  ISARY(x)  (((x) & N_TMASK) == (DT_ARY << N_BTSHFT))

	/* Is x a structure, union, or enumeration TAG? */

#define ISTAG(x)  ((x)==C_STRTAG || (x)==C_UNTAG || (x)==C_ENTAG)

#define  INCREF(x) ((((x)&~N_BTMASK)<<N_TSHIFT)|(DT_PTR<<N_BTSHFT)|(x&N_BTMASK))

#define  DECREF(x) ((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))


#endif
