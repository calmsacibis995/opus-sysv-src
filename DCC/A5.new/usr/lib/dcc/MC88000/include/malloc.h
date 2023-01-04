#ifndef	_Imalloc
#define	_Imalloc

#define M_MXFAST	1
#define M_NLBLKS	2
#define M_GRAIN		3
#define M_KEEP		4

struct mallinfo  {
	int	arena;
	int	ordblks;
	int	smblks;
	int	hblks;
	int	hblkhd;
	int	usmblks;
	int	fsmblks;
	int	uordblks;
	int	fordblks;
	int	keepcost;
};	

typedef unsigned int size_t;

void *malloc(size_t);
void free(void *);
void *realloc(void *, size_t);
int mallopt(int, int);
struct mallinfo mallinfo(void);

#endif

