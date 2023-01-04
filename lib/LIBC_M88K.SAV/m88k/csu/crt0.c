/*
**	%Q% %W%
**
**	crt0.c
**
**	C runtime startup code without profiling support
*/

extern void exit();
extern int main();

char **environ;

void
_start(argc, argv, envp)
register int argc;
register char *argv[];
register char *envp[];
{
	environ = envp;
	exit(main(argc, argv, envp));
}
