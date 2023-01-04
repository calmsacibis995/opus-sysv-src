#define WSTOPPED	0177

#define WNOHANG		0x0040
#define WUNTRACED	0x0004
#define	WIFEXITED(s)	(WTERMSIG(s) == 0)
#define	WEXITSTATUS(s)	(((s) & 0xff00) >> 8)
#define	WIFSIGNALED(s)	(!WIFEXITED(s) && !WIFSTOPPED(s))
#define	WTERMSIG(s)	((s) & 0x7f)
#define	WIFSTOPPED(s)	((WTERMSIG(s) == 0x7f) && (((s) & 0x80) == 0))
#define	WSTOPSIG(s)	(WIFSTOPPED(s) ? WEXITSTATUS(s) : 0)
