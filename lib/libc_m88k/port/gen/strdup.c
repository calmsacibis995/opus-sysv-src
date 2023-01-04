/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/strdup.c	1.7"
/*LINTLIBRARY*/
/* string duplication
   returns pointer to a new string which is the duplicate of string
   pointed to by s1
   NULL is returned if new string can't be created
*/

#include <string.h>
#ifndef NULL
#define NULL	0
#endif

extern int _strlen();
extern char *_malloc();
extern char *_strcpy();

char *
_strdup(s1) 

   char * s1;

{  
   char * s2;

   s2 = _malloc((unsigned) _strlen(s1)+1) ;
   return(s2==NULL ? NULL : _strcpy(s2,s1) );
}
