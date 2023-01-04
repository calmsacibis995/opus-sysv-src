#include <stdio.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/cmn_err.h>

main(argc, argv)
register argc;
register char **argv;
{
	register i, fd, flag, bufsz;
	register char *buf;
	struct stat sb;

	if (fork())
		exit(0);

	setsid();		/* new process group */

	if (argc > 1) {
		flag = O_WRONLY|O_CREAT;
		i = argc - 1;
		while (i > 1) {
			if (strcmp(argv[i], "sync") == 0) {
				i--;
				flag |= O_SYNC;
			}
			if (strcmp(argv[i], "trunc") == 0) {
				i--;
				flag |= O_TRUNC;
			}
			if (strcmp(argv[i], "append") == 0) {
				i--;
				flag |= O_APPEND;
			}
		}
		fd = open (argv[1], flag, 0644);
		if (fd == -1) {
			perror(argv[0]);
			exit(1);
		}
	}
	else
		fd = 1;
	bufsz = _sys_local(0xf00, 18, 2);	/* get buffer size */
	buf = (char *) malloc(bufsz);
	if (buf == NULL) {
		printf("%s: malloc(%d) failed\n", argv[0], bufsz);
		exit(1);
	}
	i = _sys_local(0xf00, 18, 1, 0, 0);	/* get old value */
	i |= PRW_BUF;				/* enable buffering */
	sys_local(0xf00, 18, 1, 0, i);		/* set new value */
	while (1)  {
		i = _sys_local(0xf00,18, 0, buf, bufsz);
		if (i == -1) {
			perror("sys_local");
			exit(1);
		}
		if (i > 0) {
			flag = write (fd, buf, i);
			if (flag == -1)
				perror("write");
		}
		if (argc > 2 && fstat(fd, &sb) == 0) {
			if (sb.st_size > 1024*1024) {
				fprintf(stderr, "%s: file %s too large\n",
					argv[0], argv[1]);
				fprintf(stderr, "Exiting\n");
				exit(2);
			}
		}
	}
}
