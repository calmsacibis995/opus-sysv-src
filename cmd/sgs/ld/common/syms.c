/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)ld:common/syms.c	1.26.1.11"
#include "system.h"
#include <stdio.h>
#include <fatal.h>
#include <signal.h>
#include "structs.h"
#include "paths.h"
#include "extrns.h"
#if TRVEC
#include "tv.h"
#include "ldtv.h"
#endif
#include "sgsmacros.h"
#include "instr.h"
#include "ldmacros.h"



/*
 * ***** NOTICE ***** NOTICE ***** NOTICE *******
 *						*
 *	This version of syms.c is designed to	*
 *	permit multiple aux entries per symbol	*
 *						*
 *	This version of syms.c uses a one-way	*
 *	circular list to link all symbols that	*
 *	hash to the same hashtab[] entry	*
 *						*
 * ***** NOTICE ***** NOTICE ***** NOTICE *******
 */

#if !UNIX
#define HASHSIZE	1021
#else
#define HASHSIZE	521
#endif
/*
 * VAX cc bug KLUDGE
 */
#define abs(x)	(x < 0 ? -x : x)

ITEMID hashtab[HASHSIZE];

#if UNIX
#define MAXEXTRA	100
#define XSMTBSZ		100
#define XAXTBSZ		50
#define MYSMTBSZ	200
#define MYAXTBSZ	100
#else
#if AR16WR
#define MAXEXTRA	20
#define	XSMTBSZ		80
#define XAXTBSZ		40
#define MYSMTBSZ	400
#define MYAXTBSZ	200
#endif
#if AR32W
#define MAXEXTRA	100
#define XSMTBSZ		800
#define XAXTBSZ		400
#define MYSMTBSZ	4000
#define MYAXTBSZ	2000
#endif
#if AR32WR
#define	MAXEXTRA	10
#define XSMTBSZ		400
#define XAXTBSZ		200
#define MYSMTBSZ	2000
#define MYAXTBSZ	1000
#endif
#endif
int	xsymcnt = 0;
int	xauxcnt = 0;
SYMTAB	*xsymptr[MAXEXTRA];
AUXTAB	*xauxptr[MAXEXTRA];
SYMTAB	mysymtab[MYSMTBSZ+1];
AUXTAB	myauxtab[MYAXTBSZ+1];

hash(name)
register char *name;
{

/*
 * Compute hash value for a symbol name
 */

	register unsigned hashval;
	register int tmp;	/* VAX CC BUG KLUDGE */

	hashval = 0;
	while (*name)
		hashval = hashval * 10 + *name++;	/*used to be 13*/

/*
 * Because of a bug in the current VAX compiler, the following
 * line has temporarily been replaced by the KLUDGE following
 * this comment.  The define of abs() is also added above
 *
 *	return( (int) (hashval % HASHSIZE) );
 */

	tmp = (int) hashval;
	tmp = abs(tmp);
	return ( tmp % HASHSIZE );
}
/*eject*/
SYMTAB *
findsym(name)
char *name;
{

/*
 * Return a pointer to the ld symbol table entry for a symbol.
 *
 * NULL indicates that the symbol does not exist
 */

	register SYMTAB *p;	/* ld symbol table entry	*/
	register int chain;	/* size of a collision chain	*/
	int hashval;		/* hashtab[] index for the sym	*/
	register ITEMID acid,	/* SDP id for ld symbol entry	*/
	       base;		/* head of collision chain	*/


/*
 * All symbols that hash to the same value are connected in a one-way,
 *	circular linked list.  The most recently referenced symbol on
 *	the chain is considered the head of the chain, and its id is
 *	stored in the hash table
 *
 * Get the head of the collision chain, and walk down it looking for
 *	a match
 */

	hashval = hash(name);
	if( (acid = hashtab[hashval]) == 0 )
		return( NULL );
	base = acid;
	chain = 1;

	do {
		char sym_space[9];
		char	*sym_name;
		p = getsym(acid);
		if (p->smnamelength == 9)
		{
			strncpy( sym_space, p->sment.n_name, 8 );
			sym_space[8] = '\0';
			sym_name = sym_space;
		}
#if FLEXNAMES
		else if (p->smnamelength > 9)
			sym_name = p->sment.n_nptr;
#endif
		else
			sym_name = p->sment.n_name;

		if (equal( sym_name, name, p->smnamelength ))
		{
#if PERF_DATA
			maxchain = max(maxchain,chain);
			nwalks += (chain - 1);
#endif
			hashtab[hashval] = acid;
			return( p );
		}

		acid = p->smchainid;
		chain++;
		
		}
	while( acid != base );

#if PERF_DATA
	nfwalks += (chain - 1);
#endif

	return( NULL );		/* not found */
}
/*eject*/
AUXTAB *
findaux(sym, ndx)
SYMTAB *sym;
int ndx;
{

/*
 * Return a pointer to the ndx'th aux entry for the ld symbol table
 * entry given by sym
 *
 * NULL indicates that no such aux entry exists
 */

	register AUXTAB *a;	/* pointer to a symbol aux entry*/
	ITEMID acid;		/* SDP id for ld aux entry	*/

/*
 * All aux entries belonging to a symbol are linked together in a
 *	one-way linked list.  The entries are ordered by aux-entry
 *	number: the first aux entry is the head of the list; it is
 *	linked to the 2'nd aux entry, which is linked to the 3'rd, etc
 *
 * Get the head of the aux entry list, and walk down it looking for
 *	the specified aux entry
 */

	acid = sym->smauxid;

	while( acid != 0 ) {

		a = getaux(acid);

		if( a->axord == ndx )
			return( a );

		acid = a->axchainid;

		}

	return( NULL );		/* not found */
}
/*eject*/
SYMTAB *
makesym(sym, filp)
register SYMENT *sym;
INFILE	*filp;
{

/*
 * Given a (new) *.o symbol table entry, add it to the ld
 * symbol table
 *
 * A ld symbol table entry is built and initialized. Appropriate
 * entry is made in the hash table 
 *
 * The address of the ld symbol table entry is returned
 */

	register SYMTAB *p;	/* pointer to a ld symbol entry	*/
	register ITEMID acid;	/* SDP id of a ld symbol entry	*/
	int hashval;		/* index into hash table for sym*/
	int auxcnt;
	char *name;

	name = PTRNAME( sym );
	if( (p = findsym(name)) == NULL ) {
		/*
 		 * Case 1: The symbol is not in the ld symbol table.
		 *	   Add a new entry, making it the head of the
		 *		collision chain
		 *	   Update the hash table
 		 */
		if (numldsyms >= MYSMTBSZ + (xsymcnt * XSMTBSZ)) {
			if (xsymcnt >= MAXEXTRA)
				lderror(2,0,"internal error: ","symbol table overflow");
			xsymptr[xsymcnt++] = (SYMTAB *) mycalloc(XSMTBSZ * SYMSIZ);
			}
		acid = ++numldsyms;
		p = getsym(acid);
		p->sment = *sym;
		p->smnamelength = strlen( name ) + 1;
		if ( sym->n_scnum == 0 && sym->n_value == 0 )
			unresolved++;
#if FLEXNAMES
		if ((sym->n_zeroes == 0L) && ((filp == NULL)
				|| (filp && (filp->flstrings == NULL))))
			p->sment.n_nptr = savefn(sym->n_nptr);
#endif
		p->sment.n_numaux = 0;
		p->sm_how = __undefined;
		p->sm_seen = 1;
		p->smmyacid = acid;

		hashval = hash(name);
		if( hashtab[hashval] > 0 ) {
			SYMTAB *p2;
			p2 = getsym(hashtab[hashval]);
			p->smchainid = p2->smchainid;
			p2->smchainid = acid;
#if PERF_DATA
			ncolisns++;
#endif
			}
		else
			p->smchainid = acid;

		hashtab[hashval] = acid;
		return( p );
		}

/*
 * Case 2: The symbol is already in the symbol table
 *	   Update the existing entry
 */

	if( sym->n_scnum != 0 ) {
		/*
		 * The makesym() argument 'sym' is the defining entry
		 * for the symbol
		 */
		if( p->sment.n_scnum == 0 ) {

			/*
			 * The existing ld symbol table entry
			 * was from a reference
			 *
			 * Merge the information from the function
			 * argument and the ld symbol table
			 *
			 * The ld symbol table entry may have a ld-
			 * generated aux entry. If so, carry this
			 * entry over.
			 */
#if COMMON
			if (p->sment.n_value > 0)
			{
				if (Mflag)
					lderror(0,0,NULL, "Symbol %s in %s is multiply defined.", 
						name,curfilnm );
			}
			else
#endif
				unresolved--;
			if (p->sm_seen == 0 && p->sm_how != __undefined)
			{
				p->sm_seen = 1;
			}
			p->sment.n_value = sym->n_value;
			p->sment.n_scnum = sym->n_scnum;
			p->sment.n_type = sym->n_type;
			p->sment.n_sclass = sym->n_sclass;
			}
		else if (p->sment.n_scnum != N_ABS)
			lderror(1,0,NULL, "Symbol %s in %s is multiply defined. First defined in %s",
				name, curfilnm, p->smscnptr->isfilptr->flname );
		else if (sym->n_scnum != N_ABS)
			lderror(1,0,NULL, "Symbol %s in %s is multiply defined.",
				name, curfilnm);
		else if (sym->n_value != p->sment.n_value)
			lderror(1,0,NULL, "Absolute symbol %s in %s multiply defined.",
				name, curfilnm);
		}

#if COMMON
	else if (sym->n_value > 0)	/* & sym->n_scnum == 0 => common sym */
	{
		if (p->sment.n_scnum != 0 || (p->sment.n_scnum == 0
			&& p->sment.n_value > 0))
		{
			if (Mflag)
				lderror(0, 0, NULL, "Symbol %s in %s is multiply defined.",
					name, curfilnm);
		}
		else
			unresolved--;
		if (p->sm_seen == 0 && p->sm_how != __undefined)
		{
			p->sm_seen = 1;
		}

		/* set value for .common symbol */

		if ( p->sment.n_scnum == N_UNDEF )
		{
			if ( !tflag && ( p->sment.n_value > 0 )
				&& ( p->sment.n_value != sym->n_value ))
			{
				lderror(0, 0, NULL,
					"Multiply defined symbol %s, in %s, has more than one size",
					PTRNAME(sym), curfilnm);
			}

			p->sment.n_value = /* max of old & new val */
				(p->sment.n_value > sym->n_value) ?
				 p->sment.n_value : sym->n_value;

			if (sym->n_type != T_NULL && p->sment.n_type == T_NULL)
				p->sment.n_type = sym->n_type;
		}
	}
#endif
	return( p );
}
/*eject*/
AUXTAB *
makeaux(sym, aux, ndx)
register SYMTAB *sym;	/* symbol table entry to which the aux entry belongs */
register AUXENT *aux;	/* new aux entry				     */
int    ndx;	/* number of aux entries already added to entry	     */
{

/*
 * Given a (*.o) aux entry, add it to an existing ld symbol
 * table entry
 *
 * The address of the ld aux entry is returned
 */

	register AUXTAB *a;
	register int tvndx;
	ITEMID acid;

/*
 * Add the new aux entry to the symbol table entry
 */

	if( (acid = sym->smauxid) == 0 ) {
		/*
		 * Add the first aux entry for a symbol 
		 */
		if( ndx != 0 )
			lderror(1,0,NULL,"Making aux entry %d for symbol %s out of sequence",
				ndx+1, SYMNAME( sym->sment ));
		if (numldaux >= MYAXTBSZ + (xauxcnt * XAXTBSZ)) {
			if (xauxcnt >= MAXEXTRA)
				lderror(2,0,"internal error: ","aux table overflow");
			xauxptr[xauxcnt++] = (AUXTAB *) mycalloc(XAXTBSZ * AUXSIZ);
			}
		acid = ++numldaux;
		a = getaux(acid);
		a->axent = *aux;
		a->axord = 1;
		a->axmyacid = acid;
		a->axchainid = 0;

		sym->sment.n_numaux++;
		sym->smauxid = acid;

		return( a );
		}
	
	a = getaux(acid);


	if( ndx == 0 ) {
		/*
		 * The symbol table entry already has a aux entry.
		 *
		 * This occurs when ld generates an aux entry for
		 * the symbol, to contain a tv slot index.
		 *
		 * Merge this ld-entry with the entry obtained from
		 * the input file
		 *
		 * Two cases: due to a tv ref in an earlier
		 *	input file, the aux entry in the symbol
		 *	table is a dummy one and should be
		 *	overwritten except for the tvndx.
		 *
		 *  (2) we are doing subsystem linking and
		 *	we are getting dummy and real auxents
		 *	in an unknown order.  in this case
		 *	we have to test to see which one is
		 *	the dummy and either overwrite it
		 *	or throw it away.  We have this info
		 *	in psymtab() and should be an arg
		 *	to makeaux().  However a KLUDGE is
		 *	to see which one is all 0's except
		 *	for the tvndx and call that one the
		 *	dummy.  This should be cleaned up
		 *	and made reasonable later.
		 */

		/* if incoming is not dummy */
		if ( ! ( (aux->x_sym.x_tagndx == 0L) &&
			(aux->x_sym.x_misc.x_fsize == 0L) &&
			   (aux->x_sym.x_fcnary.x_fcn.x_lnnoptr == 0L) &&
			      (aux->x_sym.x_fcnary.x_fcn.x_endndx == 0L) ) ) {
#ifndef M88K
			tvndx = a->axent.x_sym.x_tvndx;
#endif
			a->axent = *aux;
#ifndef M88K
			if (tvndx != N_TV)
				a->axent.x_sym.x_tvndx = tvndx;
#endif
			}

		return( a );
		}

/*
 * Step down the chain of aux entries, to the correct position for the
 * new entry
 */

	while( a->axord != ndx ) {
		acid = a->axchainid;
		a = getaux(acid);
		}

	if( a->axchainid != 0 ) {
		/*
		 * Overwriting an existing aux entry (but not the first aux
		 * entry). Warn the user
		 */
		lderror(0,0,NULL,"Overwriting aux entry %d of symbol %s",
			ndx+1, SYMNAME( sym->sment ));

		acid = a->axchainid;
		a = getaux(acid);
		a->axent = *aux;

		return( a );
		}
	else {
		/*
		 * Add the new aux entry at the end of the list
		 */
		if (numldaux >= MYAXTBSZ + (xauxcnt * XAXTBSZ)) {
			if (xauxcnt >= MAXEXTRA)
				lderror(2,0,"internal error: ","aux table overflow");
			xauxptr[xauxcnt++] = (AUXTAB *) mycalloc(XAXTBSZ * AUXSIZ);
			}
		acid = a->axchainid = ++numldsyms;
		a = getaux(acid);
		a->axent = *aux;
		a->axord = ndx + 1;
		a->axmyacid = acid;
		a->axchainid = 0;

		sym->sment.n_numaux++;

		return( a );
		}
}

SYMTAB *
getsym(acid)
register ITEMID acid;
{

/*
 * Return a pointer to an ld symbol table entry, given the
 * access ID (array index)
 */

	if (acid <= 0)
		lderror(2,0,"internal error: ","negative symbol table id");
	if (acid <= MYSMTBSZ)
		return(&mysymtab[acid]);
	acid -= (MYSMTBSZ + 1);
	if (acid < xsymcnt * XSMTBSZ)
		return( (SYMTAB *) ( (long) xsymptr[acid/XSMTBSZ] + ((acid % XSMTBSZ) * SYMSIZ)) );
	lderror(2,0,"internal error: ","invalid symbol table id");
}

AUXTAB *
getaux(acid)
ITEMID acid;
{

/*
 * Return a pointer to a ld aux entry, given the
 * access ID
 */

	if (acid <= 0)
		lderror(2,0,"internal error: ","negative aux table id");
	if (acid <= MYAXTBSZ)
		return(&myauxtab[acid]);
	acid -= (MYAXTBSZ + 1);
	if (acid < xauxcnt * XAXTBSZ)
		return( (AUXTAB *) ( (long) xauxptr[acid/XAXTBSZ] + ((acid % XAXTBSZ) * AUXSIZ)) );
	lderror(2,0,"internal error: ","invalid aux table id");
}
/*eject*/
long
#if FLEXNAMES
psymtab(fdes, numsyms, filp, symbuf,strtabptr)
char *strtabptr; /* pointer to string table with long symbol names, if
		     one exists */
#else
psymtab(fdes, numsyms, filp, symbuf, strtabptr)
#endif
FILE *fdes;	/* pointer to the file, positioned to the symbol table */
long numsyms;	/* total number of symbol table entries for the file   */
INFILE *filp;	/* pointer to the infilist entry for the file	       */
register SYMENT *symbuf;
{

/*
 * This routine is called during the scanning of the LDFILE list.
 *
 * Every *.o and archive library member which is to be link edited
 * is processed exactly once by this function:
 *
 *	1. Add all ld symbols from the file to ld's cumulative
 *		ld symbol table
 *	********** for FLEXNAMES ***********
 *	1a. When there is a long symbol name, change the
 *		offset of the name in the symbol table to a pointer
 *		to the name
 *	************************************
 *	2. Compute and return the number of local symbol table
 *		entries (locals, statics, functions, etc.) for this
 *		file
 */


	register SYMENT *sym;		/* input file symbol table entry     */
	register AUXENT *aux;		/* symbol aux entry		     */
	register SYMTAB *symp;		/* ld symbol table entry pointer     */
	register long n;
	long locsyms,		/* nbr local symbols from input file */
	     relsyms,		/* nbr relocatable symbols	     */
	     l;
	int  i;			/* for-loop index		     */
	char tmp_numaux,
	     tmp_sclass;
	SYMENT	lsymbuf;
	AUXENT	lauxbuf;

	sym = &lsymbuf;
	aux = &lauxbuf;

	locsyms = 0L;
	relsyms = 0L;

	for( n = 0L; n < numsyms; n++ ) {

#if !PORTAR && !PORT5AR
		if (symbuf != NULL) 
			sym = symbuf++;
		else 
#endif
			if (fread(sym,SYMESZ,1,fdes) != 1)
				lderror(2,0,NULL,"fail to read symbol table of file %s",
				filp->flname);

#if FLEXNAMES
			/* this symbol has a long name - change offset to ptr */
		if (sym->n_zeroes == 0L)
		{
			if (strtabptr == NULL)
				lderror( 2, 90, NULL, "no string table in file %s", filp->flname );
			sym->n_nptr = strtabptr + sym->n_offset;
		}
#endif

		switch( sym->n_sclass ) {

		/*
		 * Symbols of type C_EXT are global, and must be added
		 * to ld's cumulative symbol table
		 */
		case C_EXT:
			relsyms++;
			symp = makesym(sym, filp);

			if( sym->n_scnum > 0 )
				symp->smscnptr = sectnum(sym->n_scnum,filp);


			/*
			 * Functions (except shared library absolute
			 * functions), although added to ld's symbol
			 * table, must be put in the "local symbol"
			 * section of the final output file
			 */
			if( ISFCN(sym->n_type) && (sym->n_scnum != N_ABS) ) {
				symp->smoutndx = locsyms;
				locsyms += (long) (sym->n_numaux + 1);
				symp->smlocflg = 1;
				}

			n += (long) sym->n_numaux;
			tmp_numaux = sym->n_numaux;
			for( i = 0; i < tmp_numaux; i++ ) {
#if !PORTAR && !PORT5AR
				if (symbuf != NULL) 
					aux = (AUXENT *) symbuf++;
				else 
#endif
					if (fread(aux,AUXESZ,1,fdes) != 1)
						lderror(2,0,NULL,"fail to read aux entries of file %s",
						filp->flname);

#if TRVEC
				if ( i == 0 && (ISFCN( sym->n_type ) || (sym->n_type == 0)))
				     if (aux->x_sym.x_tvndx == N_TV) {
					aux->x_sym.x_tvndx = P_TV;
					tvspec.tvosptr->oshdr.s_size += TVENTSZ;
					}
#endif
				makeaux(symp, aux, i);
				}

			break;

		case C_FILE:
#if DMERT
			if (!dmertplib)
#endif
				locsyms += 1 + (long) sym->n_numaux;
			if (sym->n_numaux) {
				n += sym->n_numaux;
#if !PORTAR && !PORT5AR
				if (symbuf != NULL)
					symbuf++;
				else
#endif
					if (fseek( fdes, (long) (AUXESZ) * sym->n_numaux, 1 ) != 0)
						lderror( 2, 0, NULL, "fail to skip aux entry of %s", filp->flname);
			}
			break;

		case C_STAT:
#if UNIX
			relsyms++;
			locsyms += 1 + sym->n_numaux;
			if (sym->n_numaux == 1) {
				n++;
#if !PORTAR && !PORT5AR
				if (symbuf != NULL)
					symbuf++;
				else
#endif
					if (fseek( fdes, (long) AUXESZ, 1 ) != 0)
						lderror( 2, 0, NULL, "fail to skip the aux entry of %s", filp->flname);
			};
			continue;
#endif

		case C_HIDDEN:
			relsyms++;

		default:
#if DMERT
			if (!dmertplib && !xflag)
#else
			if (!xflag)
#endif
				locsyms++;

			if( sym->n_numaux > 0 ) {

#if DMERT
				if (!dmertplib && !xflag)
#else
				if (!xflag)
#endif
					locsyms += (long) sym->n_numaux;
				n += (long) sym->n_numaux;
				tmp_sclass = sym->n_sclass;
				tmp_numaux = sym->n_numaux;
#if !PORTAR && !PORT5AR
				if (symbuf != NULL) 
					aux = (AUXENT *) symbuf++;
				else 
#endif
					if (fread(aux,AUXESZ,1,fdes) != 1)
						lderror(2,0,NULL,"fail to read aux entries of %s",
						filp->flname);

				/*
				 * Skip all aux entries after the first
				 */
				if( tmp_numaux > 1 ) 
#if !PORTAR && !PORT5AR
					if (symbuf != NULL) 
						symbuf++;
					else
#endif
						if (fseek(fdes, (long) (AUXESZ * (tmp_numaux-1)), 1) != 0)
							lderror(2,0,NULL,"fail to skip the aux entry of %s",
							filp->flname);

				/*
				 * Skip all members of structures and
			 	 * unions
				 */
				if( ISTAG(tmp_sclass) ) {
					l = aux->x_sym.x_fcnary.x_fcn.x_endndx - n - 1;
#if DMERT 
					if (!dmertplib && !xflag)
#else
					if (!xflag)
#endif
						locsyms += l;
					n = aux->x_sym.x_fcnary.x_fcn.x_endndx - 1;
#if !PORTAR && !PORT5AR
					if (symbuf != NULL) 
						symbuf += l;
					else 
#endif
						if (fseek(fdes, l * SYMESZ, 1) != 0)
							lderror(2,0,NULL,"fail to skip the mem of struct of %s",
							filp->flname);
					}
#if TRVEC

				/*
				 * Increment size of .tv if this
				 * symbol needs a slot in the tv.
				 */
#ifndef M88K
				if( aflag && tvflag && (tmp_sclass == C_STAT) && (aux->x_sym.x_tvndx == N_TV) )
					tvspec.tvosptr->oshdr.s_size += TVENTSZ;
#endif
#endif
				}
			}
		}

	if( relsyms > maxrelocsms )
		maxrelocsms = relsyms;

	return( locsyms );
}



/*eject*/
AUXTAB *
loopaux(sym, aux, changed)
SYMTAB *sym;
AUXTAB *aux;
int changed;
{

/*
 * Loop through all the aux entries for a given ld symbol table entry
 *
 *
 * This routine is used as follows:
 *
 *	For the first call to this routine, aux is set to NULL.
 *	For all subsequent calls, aux equals the value returned by
 *		the previous call.
 *	When aux again equals NULL, all aux entries have been returned
 */

	if( aux == NULL ) {
		/*
		 * Get the first aux entry, pointed to by the ld symbol
		 * table entry
		 */
		if( sym->smauxid != 0 )
			return(getaux(sym->smauxid));

		return( NULL );
		}

/*
 * Find the next aux entry
 */

	if (aux->axchainid != 0)
		return(getaux(aux->axchainid));

	return( NULL );
}
/*eject*/
INSECT *
sectnum(i, fp)
int i;
INFILE *fp;
{

/*
 * Called by psymtab to retrieve a pointer to the  i-th section header
 */

	register INSECT *sptr;

	if (i == 0)
		return(NULL);


	for ( sptr = fp->flishead; sptr != NULL; sptr = sptr->isnext )
		if ( sptr->isecnum == i)
			return(sptr);
	return(NULL);
}
/*eject*/
ITEMID
add_dot(dot_val, dot_sec)
long dot_val;
INSECT *dot_sec;
{
	register SYMTAB *sym;
	register ITEMID acid;
	static int count = 0;

/*
 * This is a special-purpose "makesym()" designed for exclusive use
 * in assignment expressions. It will create a symbol table entry
 * having the following characteristics:
 *
 *	n_name:		.dotnnn		001 <= nnn <= 999
 *	n_value:	dot_val
 *	n_scnum:	0
 *	n_type:		T_NULL
 *	n_sclass:	C_NULL
 *	n_numaux:	0
 *
 *	smscnptr:	dot_sec
 *	smlocflg:	1
 *
 * The symbol table entry will NOT appear in the hash table, and hence 
 * will not be accessible except thorugh the stored itemid returned
 * by this function
 */

	if (numldsyms >= MYSMTBSZ + (xsymcnt * XSMTBSZ)) {
		if (xsymcnt >= MAXEXTRA)
			lderror(2,0,"internal error: ","symbol table overflow");
		xsymptr[xsymcnt++] = (SYMTAB *) mycalloc(XSMTBSZ * SYMSIZ);
		}
	acid = ++numldsyms;
	sym = getsym(acid);
	sprintf(sym->sment.n_name, ".dot%03d", ++count);
	sym->sment.n_value = dot_val;
	sym->smscnptr = dot_sec;
	sym->smmyacid = acid;
	sym->smchainid = acid;
	sym->smlocflg = 1;

	return( acid );

}

int num_hidden = 0;
int num_exported = 0;
int scope_changes = 0;

static char both_msg[] = "Symbol %s marked as both exported and hidden!";

int sym_scope(s, how)
	char *s;
	enum scope how;
{
	SYMENT sym;
	int n;
	enum how_prev;
	SYMTAB *p;

	if (how != __hidden && how != __exported)
		lderror(2,0,NULL,
		"Symbol %s tagged with unknown scope!", s);
	switch (hasmeta(s)) {
	    case 1:
		scope_changes = 1;
		store_re(s, how);
		return;
	    case -2:
		take_out_escapes(s);
		break;
	    case -1:
		lderror(0,0,NULL,
		"Invalid regular expression: %s, ignored", s);
		return;
		}
	zero( (char *) &sym, SYMESZ );

#if FLEXNAMES
	if ((n = strlen(s)) > 8) {
		sym.n_zeroes = 0L;
		sym.n_nptr = myalloc(n+1);
		copy(sym.n_nptr, s, n+1);
		}
	else
#endif
		copy( sym.n_name, s, 8);

	sym.n_sclass = C_EXT;
	sym.n_scnum = N_UNDEF;

	if ((p = findsym(s)) == NULL)
	{
		p = makesym(&sym, NULL);
		p->sm_seen = 0;
	}
#if FLEXNAMES
	if (n > 8 && p->sment.n_nptr != sym.n_nptr)
		(void) free(sym.n_nptr);
#endif
	switch (p->sm_how) {
		case __undefined:
			p->sm_how = how;
			if (how == __hidden)
				num_hidden++;
			else
				num_exported++;
			scope_changes = 1;
			if (p->sment.n_scnum != N_UNDEF || p->sment.n_value != 0)
				p->sm_seen = 1;
			return(0);
		case __exported:
			if (how == __hidden)
				lderror(2,0,NULL, both_msg, s);
			return(1);
		case __hidden:
			if (how == __exported)
				lderror(2,0,NULL, both_msg, s);
			return(1);
		}
	/* NOTREACHED */
}

int hideornots(s)
	char *s;
{
	SYMTAB *p;

	if ((p = findsym(s)) == NULL)
		return(action_re(s, __hidden));
	else return(hideornot(p));
}

/*
 * Assuming that "s" is an external symbol, should we
 * hide it?
 */

int hideornot(s)
	SYMTAB *s;
{
	int res;

	if (s->sm_how == __undefined) {
		char *name;
#ifdef FLEXNAMES
		name = (s->sment.n_zeroes == 0L) ? s->sment.n_nptr
			:  s->sment.n_name;
#else
		name = s->sment.n_name;
#endif
		return(action_re(name, __hidden));
	} else if (s->sm_how == __hidden)
		return(1);
	else /* s->sm_how = __exported */
		return(0);
	/* NOTREACHED */
}
/*
 * Hide the following symbol, checking that it's defined (and all
 * that) first.
 */

hide_sym(sm, how)
	SYMENT *sm;
	enum scope how;
{
	char *s;

	if (!scope_changes) return;
	if (rflag && sm->n_sclass == C_EXT &&
		sm->n_scnum == N_UNDEF && sm->n_value == 0)
	{
		if (how != __undefined)
			lderror(2, 0, NULL, "Trying to hide undefined symbol %s\n",
				PTRNAME(sm));
		return;
	}
	if (rflag && sm->n_sclass == C_EXT &&
		sm->n_scnum == N_UNDEF && sm->n_value > 0)
	{
		if (how != __undefined)
			lderror(2, 0, NULL,
				"common symbol %s being hidden.\n",
				PTRNAME(sm));
		return;
	}
	sm->n_sclass = C_STAT;
	return;
}

take_out_escapes(s)
	char *s;
{
	char *p;

	for (p = s; *p; p++)
	{
		if (*p == '\\')
			(void) strcpy(p, p+1);
	}
}
