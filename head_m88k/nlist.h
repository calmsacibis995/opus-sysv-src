#ifndef _NLIST_H_
#define _NLIST_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/* symbol table entry structure */

struct nlist
{
	char		*n_name;	/* symbol name */
	long		n_value;	/* value of symbol */
	short		n_scnum;	/* section number */
	unsigned short	n_type;		/* type and derived type */
	char		n_sclass;	/* storage class */
	char		n_numaux;	/* number of aux. entries */
};

#endif	/* ! _NLIST_H_ */
