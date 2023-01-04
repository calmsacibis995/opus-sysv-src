/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)as:common/symlist.c	1.6" */

#include <stdio.h>
#include <string.h>
#include <syms.h>
#include "systems.h"
#include "gendefs.h"
#include "symbols.h"
#include "errors.h"
#include "dbug.h"

#ifdef index
#undef index
#endif

extern char *strtab;
extern long currindex;

typedef struct symlstag {
    long index;
    struct symlstag *nextelmt;
    char *sym;
    int tag;
    long symoffset;
} symlist;

#define SYMSIZ	sizeof(symlist)

/*
 *
 *	"symlist.c" is a file containing functions for keeping track
 *	of symbol table indices.  The following functions are provided:
 *
 *	putindx(sym,sclass,index) Remember that the symol "symb->name"
 *			with storage class "sclass" has symbol table
 *			index "index".
 *
 *	getindx(symb,sclass) Return the symbol table index of the
 *			symbol "symb" with storage class "sclass".
 *
 */

extern void as_error ();

/* Performance improvement added with flexnames changes	 */
/* involves splitting up the single linear linked list	 */
/* into several linked lists, separated according to	 */
/* the storage class.  Previously, the linked list was	 */
/* searched testing each element to see if it was of	 */
/* the right class, then checking whether it was the	 */
/* right symbol name.  Now we just check the list which	 */
/* contains only symbol names with the right class.	 */
/* The only classes used are C_EXT, C_STAT, C_STRTAG,	 */
/* C_UNTAG, and C_ENTAG.  That accounts for the magic	 */
/* number C_ENTAG+1 as the size, with C_ENTAG+1 (16)	 */
/* NULLs for initialization.  CHANGES IN ../../inc/	 */
/* common/storclass.h SHOULD BE FOLLOWED BY CHANGES	 */
/* HERE, if necessary.					 */

static symlist *symlst[C_ENTAG + 1] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

/*
 *
 *	"alloc" is a function that permanently allocates blocks of
 *	storage.  The storage allocated by this routine is more
 *	economical than that provided by "malloc", however it cannot be
 *	returned to free storage.  Blocks allocated by "alloc" should
 *	never be passed to "free".
 *
 *	This function operates by obtaining large blocks of storage
 *	from "malloc", and then doles it out in small pieces without
 *	keeping around extra bookkeeping information for each small
 *	block.
 *
 *	The size of the blocks obtained from "malloc" is determined by
 *	the static array "incrs".  This contains successively smaller
 *	sizes of blocks to be obtained.  When a block of a given size
 *	cannot be obtained from "malloc", the next smaller size is
 *	tried.  The static variable "incptr" indexes the element of
 *	"incrs" that is currently being used for the size of the block
 *	to be allocated.
 *
 *	The current block from which storage is being allocated is
 *	addressed by the static variable "block".  The number of bytes
 *	remaining to be allocated in this block is given by the static
 *	variable "length".
 *
 *	Storage is allocated by first rounding the size up to an even
 *	number of bytes.  This avoids alignment problems if the storage
 *	is to be used for something other than characters.  If "length"
 *	is insufficient for allocating "size" bytes, another block of
 *	storage is obtained and "length" is set to the size of that
 *	block.  If necessary, a smaller size will be used to allocate
 *	this new block of storage.  Once a block of sufficient size is
 *	present, the "size" is subtracted from the remaining length,
 *	the variable "block" is incremented by the "size", and the
 *	pevious value of "block" is returned.
 *
 *	Modified 2-Sep-86 by Fred Fish to do all rounding to long word
 *	boundries, and to return type "long *" rather than the bogus
 *	"char *".  This not only fixes the lint complaints but also gives
 *	the code a fighting chance to run on machines where sizeof(int) is
 *	not equal to sizeof(long) and pointers must be longword aligned.
 *	Note that for historical reasons, malloc is declared as returning
 *	"char *" when in fact it returns a pointer which meets the most
 *	stringent alignment requirements of the host (usually "long *" is
 *	sufficient).
 */

long *alloc (size)
register int size;
{
    register int mod;
    register char *ptr;

    static unsigned incrs[] = {
	2048, 1024, 512, 256, 128, 64, 0
    };
    static short incptr = 0;
    static short length = 0;

    static char *block;
    extern char *malloc ();

    /* 
     * Round up "size" to multiple of the longwordsize
     * of the machine the assembler is running on
     */

    DBUG_ENTER ("alloc");
    if ((mod = size % sizeof (long)) != 0) {
	size += sizeof (long) - mod;
    }
    if (size > length) {
	while ((block = malloc (incrs[incptr])) == 0) {
	    if (incrs[++incptr] == 0) {
		as_error (ERR_MALLOC, incrs[incptr-1]);
	    }
	}
	length = incrs[incptr];
    }
    ptr = block;
    block += size;
    length -= size;
    DBUG_RETURN ((long *)ptr);
}

/*
 *
 *	"putindx" is a function that remembers the index of a symbol
 *	table entry.  It first checks to see that the symbol has a
 *	storage class indicating that it is either an external or
 *	static external symbol, or a structure, union, or enumeration
 *	tag.  If it is not one of these, this function does nothing
 *	since no one will ever care what its symbol table index is.
 *	Otherwise, a new structure of type "symlist" is created and
 *	initialized using the parameters to this function.  This is
 *	pushed onto the beginning of a list of such structures pointed
 *	to by "symlst".  (For flexnames, it is added to the list
 *	associated with that storage class.)
 *	The reason that "putindx" is called with a symbol table
 *	pointer rather than a character pointer is to ensure
 *	that the string pointed to (the symbol name) will
 *	stay around, since the pointer to this string is
 *	saved.  Only the symbol table is guaranteed to remain.
 *
 */

void putindx (sym, sclass, index)
symbol *sym;
register short sclass;
long index;
{
    register symlist *temp;

    DBUG_ENTER ("putindx");
    switch (sclass) {
	case C_EXT: 
	case C_STAT: 
	case C_STRTAG: 
	case C_UNTAG: 
	case C_ENTAG: 
	    temp = (symlist *) alloc (SYMSIZ);
	    if (sym -> _name.tabentry.zeroes == 0) {
		temp -> tag = INDEX;
		temp -> symoffset = sym -> _name.tabentry.offset;
	    }
	    else {
		temp -> tag = CHARPTR;
		temp -> sym = sym -> _name.name;
	    }
	    temp -> index = index;
	    temp -> nextelmt = symlst[sclass];
	    symlst[sclass] = temp;
    }
    DBUG_VOID_RETURN;
}

/*
 *
 *	"getindx" is a function that gets the symbol table index of
 *	a symbol "symb" with storage class "sclass".  If "sclass" is
 *	specified as zero, any storage class is OK (FOR NON-FLEXNAMES
 *	ASSEMBLER ONLY!).  For the non-flexnames assembler, it does
 *	this by searching through the list of structures of type "symlist"
 *	starting at "symlst" for an entry with the right storage class
 *	which has the right name.  For the flexnames assembler, it searches
 *	only the list containing symbols with that storage class.  If the
 *	symbol is found, its index is returned.  Otherwise, -1 is returned
 *	to indicate failure.
 *
 */

long getindx (symb, stclass)
register char *symb;
short stclass;
{
    register symlist *ptr;
    register char *p1;
    register char *p2;
    register long indx = -1L;

    DBUG_ENTER ("getindx");
    DBUG_PRINT ("indx", ("find symbol '%s', of storage class %d", symb, stclass));
    for (ptr = symlst[stclass]; ptr != NULL; ptr = ptr -> nextelmt) {
	p1 = symb;
	if (ptr -> tag == INDEX) {
	    p2 = &strtab[ptr -> symoffset];
	} else  {
	    p2 = ptr -> sym;
	    /* More often than not, strcmp will return false here.	*/
	    /* To avoid too many unnecessary function calls of	 	*/
	    /* strcmp, we do a precheck of the SECOND character.	*/
	    /* The second character is used, since we're guaranteed	*/
	    /* at least two characters (though either second char-	*/
	    /* acter may be a null), and since the first character	*/
	    /* may not be meaningful (e.g., _ on the VAX).		*/
	}
	if (p1[1] == p2[1] && !strcmp (p1, p2))  {
	    indx = ptr -> index;
	    DBUG_PRINT ("indx", ("found it at %d", indx));
	    break;
	}
    }
    DBUG_RETURN (indx);
}
