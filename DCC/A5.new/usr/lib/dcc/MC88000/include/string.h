#ifndef __Istring
#define __Istring

#define NULL	0

typedef unsigned int size_t;		/* sizeof type			*/

void	*memcpy(void *, const void *, size_t),
	*memccpy(void *, const void *, int, size_t),
	*memmove(void *, const void *, size_t),
	*memchr(const void *, int, size_t),
	*memset(const void *, int, size_t);
char	*strcpy(char *, const char *),
	*strncpy(char *, const char *, size_t),
	*strcat(char *, const char *),
	*strncat(char *, const char *, size_t),
	*strchr(const char *, int),
	*strpbrk(const char *, const char *),
	*strrchr(const char *, int),
	*strstr(const char *, const char *),
	*strtok(char *, const char *),
	*strerror(int);
int	memcmp(const void *, const void *, size_t),
	strcmp(const char *, const char *),
	strcoll(const char *, const char *),
	strncmp(const char *, const char *, size_t);
size_t	strxfrm(char *, const char *, size_t),
	strcspn(const char *, const char *),
	strspn(const char *, const char *),
	strlen(const char *);

#endif
