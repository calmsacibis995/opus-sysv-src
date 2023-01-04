/*
**	%Q% %W%
**
**	mcrt0.c
**
**	C runtime startup code with profiling support
**
**	notes:
**	monitor(lowpc, highpc, buffer, bufsize, nfunc) is libc/port/gen/mon.c
**
**	buffer format:
**
**	struct hdr hdr;
**	struct cnt cnt[nfunc];
**	WORD prf[(highpc-lowpc)/4];
*/

#include <mon.h>
#include "atexit.h"

#define NFUNC 600

extern char etext;
extern void _exit();
extern void _cleanup();
extern int main();

char **environ;
void exit();
extern char **___Argv;

void
_start(argc, argv, envp)
register int argc;
register char *argv[];
register char *envp[];
{
	int brksize;
	int brkrv;

	___Argv = argv;
	environ = envp;
	brksize = sizeof(struct hdr);
	brksize += NFUNC * sizeof(struct cnt);
	brksize += ((int)(&etext - (char *)exit))/(sizeof(int)/sizeof(WORD));
	brkrv = sbrk(brksize);
	if (brkrv == -1) {
		write(2, "No space for monitor buffer\n", 28);
		_exit(1);
	}
	monitor((int(*)())exit, (int(*)())&etext, (WORD *)brkrv,
		brksize/sizeof(WORD), NFUNC);
	exit(main(argc, argv, envp));
}

/*
**	If the atexit() function is linked in, it builds a queue of
**	functions to execute starting at _atexit_queue.
*/
struct qdfunc *_atexit_queue;
extern void (*_stdio_cleanup)();

void
exit(status)
int status;
{
	monitor(NULL, NULL, NULL, 0, 0);	/* write mon.out */

	while (_atexit_queue != NULL) {
		(*(_atexit_queue -> funcp))();
		_atexit_queue = _atexit_queue -> qnext;
	}
#if m88k
/*
** The following lines have been commented out to force
** the call to _cleanup(). When exit is called the pointer
** is NULL, and so it is possible that buffers which need
** to be flushed are not. This problem can occur if you
** redirect standard out to a file.
**
**	if (_stdio_cleanup != NULL) {
**	(*_stdio_cleanup)();
**	}
*/
	_cleanup();
#else
	if (_stdio_cleanup != NULL) {
		(*_stdio_cleanup)();
	}
#endif
	_exit(status);
}
