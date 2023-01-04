#ifndef _Isetjmp
#define _Isetjmp

#ifdef m88k
#define _JBLEN	40
#else
#define _JBLEN	31
#endif

typedef int jmp_buf[_JBLEN];

#define sigjmp_buf jmp_buf

extern int setjmp(jmp_buf);
extern void longjmp(jmp_buf, int);

#endif
