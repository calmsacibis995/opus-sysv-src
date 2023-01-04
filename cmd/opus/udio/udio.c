/*
 *	udiodaemon - monitors udio channel and exec's a shell
 */

#include 	"stdio.h"
#include	"fcntl.h"



extern char *		ctime();
extern char *		strtok();
extern char *		strcat();
extern char *		strcpy();
char name[32];
#define MAXBUF	1024
char buf[MAXBUF];


main(argc, argv)
	int		argc;
	char *		argv[];
{
	register int	n, ctrl;

	strcpy(name, *argv);
	strcat(name, ": ");

	switch ( fork() )
	{
	 case -1:	fatal("cannot fork\n");
	 default:	_exit(0);
	 case 0:	;
	}

	setpgrp();
	chdir("/");

	for(;;) {
		if((ctrl = open("/dev/udio", O_RDWR)) < 0)
			fatal("cannot open network control channel");

		if((n = read(ctrl, buf, MAXBUF)) < 0) {
			close(ctrl);
			continue;
		}
		buf[n] = 0;

		switch ( fork() )
		{
		 case 0:
			connect(ctrl);
			_exit(2);

		 case -1:
			continue;
		}

		while(wait(0) >= 0);
		close(ctrl);
	}
}

connect(ctrl)
{
	char **		envp;
	extern char **	setvars();
	int fd;

	setpgrp();
	close(0);close(1);close(2); 

	dup(ctrl); dup(ctrl); dup(ctrl);
	close(ctrl);

	chdir("/");
	setgid(0);
	setuid(0);
	sync();

	execl("/bin/sh", "sh", "-c", buf, 0);
}

fatal(s)
	char	*s;
{
	extern errno;
	char errbuf[64];

	sprintf(errbuf, " errno = %d\n", errno );
	write(2, name, strlen(name));
	write(2, s, strlen(s));
	write(2, errbuf, strlen(errbuf));
	_exit(1);
}

exit(){}
