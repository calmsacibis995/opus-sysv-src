#ifndef __Idirent
#define __Idirent

#include	<sys/dirent.h>

#define	DIRBUF		8192

typedef struct {
	int	dd_fd;			/* file descriptor	*/
	int	dd_loc;			/* location in buffer	*/
	int	dd_size;		/* bytes in buffer	*/
	char	*dd_buf;		/* points to buffer	*/
}	DIR;

extern int closedir(DIR *);
extern DIR *opendir(char *);
extern struct dirent *readdir(DIR *);
extern off_t telldir(DIR *);
extern void seekdir(DIR *, long);
extern void rewinddir(DIR *);

#endif
