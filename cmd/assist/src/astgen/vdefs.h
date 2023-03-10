/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)tools:vdefs.h	1.4"
#define   ADD         0
#define   DEL         1

#define   MINFSEGMS   0
#define   MAXFSEGMS   1
#define   HELP        2
#define   OP_NAME     3
#define   DEFAULT     4
#define   ACTION      5
#define   BUNDLE      6
#define   LOC         7
#define   SHELL_EXP   8
#define   REDRAW      {clearok(curscr,TRUE); wrefresh(curscr);}

extern char editor[16];

struct fieldlink {  /* Used by select */
   struct field *f_pt;
   struct fieldlink *next;
};

#ifdef index
#undef index
#endif
struct two_index {  /* Not used */
   struct index *pr_pt;
   struct index *po_pt;
};

extern char nullchar;

struct charstr {   /* Used by askstring() */
   char *str;
   char ch;
};

struct first_second {  /* Used to find captions and segments in vi screen */
	char first[1024];
	char second[1024];
};
