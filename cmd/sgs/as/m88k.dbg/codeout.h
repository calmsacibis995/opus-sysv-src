#ident "@(#) $Header: codeout.h 4.1 88/04/26 16:58:26 root Exp $ RISC Source Base"
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/
/*	Changes Copyright (c) 1986 Motorola Computer Systems	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


#define TBUFSIZ (BUFSIZ/sizeof(long))

typedef struct {
    long cvalue;
    unsigned short caction;
    unsigned short cindex;
    unsigned short cnbits;
} codebuf;

typedef struct symentag {
    long stindex;
    long fcnlen;
    long fwdindex;
    struct symentag *stnext;
} stent;

typedef struct {
    long relval;
    char *relname;
    short reltype;
    unsigned short reloffset;
} prelent;
