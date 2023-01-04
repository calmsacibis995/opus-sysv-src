h25103
s 00023/00034/00084
d D 1.2 90/04/25 12:03:36 root 2 1
c using /usr/include/varags.h
e
s 00118/00000/00000
d D 1.1 90/03/06 12:30:48 root 1 0
c date and time created 90/03/06 12:30:48 by root
e
u
U
t
T
I 1
D 2
/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/
E 2
I 2
#ifndef _VARARGS_H_
#define _VARARGS_H_
E 2

D 2
#ifndef _SYS_VARARGS_H_
#define _SYS_VARARGS_H_
E 2

I 2
/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	@(#)varargs.h	1.2	*/

E 2
#ifdef __88000_OCS_DEFINED
D 2
#include <stdargs.h>
E 2
I 2
#include <stdarg.h>
E 2
#endif

D 2
#ifndef __88000_OCS_DEFINED
E 2

#ifndef ghs
#ifndef __DCC__
typedef char *va_list;
#endif
#endif

D 2
#ifdef m88k
E 2
I 2
#ifndef __88000_OCS_DEFINED
E 2
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
I 2

#ifdef ghs		/* GreenHills varargs implementation */
typedef struct {int arg; int *stk; int *va_reg;} va_list;
#define va_dcl va_type va_alist;
#define va_end(p)
#define va_start(p) \
    ((p).arg=(int *)&va_alist-va_stkarg,(p).stk=va_stkarg,(p).va_reg=va_intreg)
#define va_arg(p,mode) (\
    (p).arg += ((p).arg & (va_align(mode) == 8)) + (3+sizeof(mode))/4,\
    ((mode *)((va_regtyp(mode)&&(p).arg<=8?(p).va_reg:(p).stk)+(p).arg))[-1])
#else
E 2
#ifdef __DCC__
#ifndef __Ivarargs
#define __Ivarargs

typedef struct {
	int	next_arg;
	int	*mem_ptr;
	int	*reg_ptr;
} va_list;

#define va_dcl int va_alist;
#define va_start(list) {\
	(list).mem_ptr = (int *)__mem_ptr();\
	(list).reg_ptr = (int *)__reg_ptr();\
	(list).next_arg =  &va_alist - (list).mem_ptr;\
}
#define va_end(list)
#define va_arg(list, mode) (\
	sizeof(mode,2) <= 3 ? (				/* char-short */\
		(list).next_arg < 8 ? (\
			*(mode*)((char*)(&(list).reg_ptr[++(list).next_arg])-sizeof(mode))\
		) : (\
			*(mode*)((char*)(&(list).mem_ptr[++(list).next_arg])-sizeof(mode))\
		)\
	) : sizeof(mode) == 4 && sizeof(mode,1) == 4 ? (/* 4 byte types */\
		(list).next_arg < 8 ? (\
			*(mode*)&(list).reg_ptr[(list).next_arg++]\
		) : (\
			*(mode*)&(list).mem_ptr[(list).next_arg++]\
		)\
	) : sizeof(mode,2) == 9  ? (			/* double */\
		((list).next_arg & 1) && (list).next_arg++,\
		(list).next_arg < 8 ? (\
			(list).next_arg += 2,\
			*(mode*)&(list).reg_ptr[(list).next_arg-2]\
		) : (\
			(list).next_arg += 2,\
			*(mode*)&(list).mem_ptr[(list).next_arg-2]\
		)\
	) : (						/* structs */\
		(void)(sizeof(mode,1) == 8 && ((list).next_arg & 1) && (list).next_arg++),\
		(list).next_arg += (sizeof(mode)+3)>>2,\
		*(mode*)&(list).mem_ptr[(list).next_arg-((sizeof(mode)+3)>>2)]\
	)\
)

#endif /* Ivarargs */
D 2
#endif /* __DCC__ */
#ifdef ghs		/* GreenHills varargs implementation */
typedef struct {int arg; int *stk; int *va_reg;} va_list;
#define va_dcl va_type va_alist;
#define va_end(p)
#define va_start(p) \
    ((p).arg=(int *)&va_alist-va_stkarg,(p).stk=va_stkarg,(p).va_reg=va_intreg)
#define va_arg(p,mode) (\
    (p).arg += ((p).arg & (va_align(mode) == 8)) + (3+sizeof(mode))/4,\
    ((mode *)((va_regtyp(mode)&&(p).arg<=8?(p).va_reg:(p).stk)+(p).arg))[-1])
E 2
#else
#define va_dcl int va_alist;
#define va_start(list) list = (va_list) (&va_alist)
#define va_end(list)
#define va_align(list,mode) (va_list)(((int)list + sizeof(mode) - 1) & (~(sizeof(mode)-1)))
#define va_arg(list, mode) ((mode *)(list = va_align(list, mode) + sizeof(mode)))[-1]
I 2
#endif /* __DCC__ */
E 2
#endif	/* ghs */
D 2

E 2
#endif	/* __STDC__ */
I 2
#endif	/* !__88000_OCS_DEFINED */ 
E 2

D 2
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
E 2
I 2
#endif	/* ! _VARARGS_H_ */
E 2
E 1
