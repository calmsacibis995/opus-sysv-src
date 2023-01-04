h52333
s 00069/00000/00000
d D 1.1 90/03/06 12:30:03 root 1 0
c date and time created 90/03/06 12:30:03 by root
e
u
U
t
T
I 1
/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

#ifndef _SYS_VARARGS_H_
#define _SYS_VARARGS_H_

#ifdef __88000_OCS_DEFINED
#include <stdargs.h>
#endif

#ifndef __88000_OCS_DEFINED

#ifndef ghs
typedef char *va_list;
#endif

#ifdef m88k
/*
 *	Varargs implementation (machine dependent) for m88000.  Note
 *	that doubles are double aligned.  Arguments are passed in
 *	registers and flushed to the stack when the compiler discovers
 *	than any argument has it's address taken.  The alignment requirements
 *	for doubles also applies to the arguments passed in registers
 *	(first word of double is in an even numbered register)
 */
#ifdef __STDC__		/* Ansi standard varargs */

#define va_start(list,parmN) list = (va_list) (&parmN + sizeof(parmN))
#define va_end(list)
#define va_align(list,mode) (va_list)(((int)list + sizeof(mode) - 1) & (~(sizeof(mode)-1)))
#define va_arg(list, mode) ((mode *)(list = va_align(list, mode) + sizeof(mode)))[-1]

#else

#ifdef ghs		/* GreenHills varargs implementation */
typedef struct {int arg; int *stk; int *va_reg;} va_list;
#define va_dcl va_type va_alist;
#define va_end(p)
#define va_start(p) \
    ((p).arg=(int *)&va_alist-va_stkarg,(p).stk=va_stkarg,(p).va_reg=va_intreg)
#define va_arg(p,mode) (\
    (p).arg += ((p).arg & (va_align(mode) == 8)) + sizeof(mode)/4,\
    ((mode *)((va_regtyp(mode)&&(p).arg<=8?(p).va_reg:(p).stk)+(p).arg))[-1])
#else
#define va_dcl int va_alist;
#define va_start(list) list = (va_list) (&va_alist)
#define va_end(list)
#define va_align(list,mode) (va_list)(((int)list + sizeof(mode) - 1) & (~(sizeof(mode)-1)))
#define va_arg(list, mode) ((mode *)(list = va_align(list, mode) + sizeof(mode)))[-1]
#endif	/* ghs */

#endif	/* __STDC__ */

#else	/* m88k */

#define va_dcl int va_alist;
#define va_start(list) list = (char *) &va_alist
#define va_end(list)
#ifdef u370
#define va_arg(list, mode) ((mode *)(list = \
	(char *) ((int)list + 2*sizeof(mode) - 1 & -sizeof(mode))))[-1]
#else
#define va_arg(list, mode) ((mode *)(list += sizeof(mode)))[-1]
#endif	/* u370 else */
#endif	/* m88k else */
#endif  /* __88000_OCS_DEFINED */


#endif	/* ! _SYS_VARARGS_H_ */
E 1
