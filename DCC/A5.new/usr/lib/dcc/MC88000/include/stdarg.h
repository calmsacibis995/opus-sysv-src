#ifndef __Istdarg
#define __Istdarg

#ifdef m88k
typedef struct {
	int	next_arg;
	int	*mem_ptr;
	int	*reg_ptr;
} va_list;

#define va_start(list,parmN) {\
	(list).mem_ptr = (int *)__mem_ptr();\
	(list).reg_ptr = (int *)__reg_ptr();\
	(list).next_arg = (int *) ((char *)&parmN + sizeof(parmN)) - (list).mem_ptr;\
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

#else

typedef char *va_list;

#define va_start(list,parmN) list = (va_list) ((char *)&parmN + sizeof(parmN))
#define va_end(list)
#define va_arg(list, mode) ((mode *)(list += sizeof(mode)))[-1]
#endif

#endif
