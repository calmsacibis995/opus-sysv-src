#ifdef NDEBUG
#define assert(ignore)	((void)(0))
#else
#define assert(expr)	((void)(!(expr) ? _assert(#expr,__FILE__,__LINE__):(void)0))
void _assert(const char *, const char *, int);
#endif
