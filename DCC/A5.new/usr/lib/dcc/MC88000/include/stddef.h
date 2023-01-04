#define NULL	0
#define offsetof(s,memb) ((size_t)((char *)&((s *)0)->memb-(char *)0))
typedef int ptrdiff_t;
typedef unsigned int size_t;
typedef char wchar_t;
