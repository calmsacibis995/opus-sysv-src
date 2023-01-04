/*
 * POSIX/ANSI function '_strstr(s1, s2)'
 *
 * char * _strstr(char *s1, char *s2)
 * Locate the <first> occurence of string s2 in s1  and return
 * a pointer to it (if found). The terminating NULL in s2 is not
 * included. If the the length of s2 is 0 then return (s1).
 * If s2 is not in s1 return NULL.
 */
#include <stdio.h>

char * 
_strstr(s1, s2)
register char *s1, *s2;
{
	if (*s2 == NULL) /* string s2 is of length 0 */
		return (s1);
	while(*s1 != NULL) {
		if (*s1 == *s2) { /* first character matches */
			/* check if the other characters also match */
			if (opusfindstr(s1+1, s2+1) == NULL)
				return (s1);
		}
		s1++;
	}

	return ( (char *) NULL);
}

static
opusfindstr(mstr, pattern)
register char *mstr, *pattern;
{
	/* 
	 * mstr = main string in which we search
	 * pattern = search for this pattern
	 * mstr is a position (>=0) within s1 of the previous function
	 * Search till complete match or one of the strings hits NULL
	 * Returns NULL -- string was found
	 *         ~NULL -- Not found
 	 */

	while (*pattern != NULL) {
		if (*pattern++ == *mstr++) {
			; /* move to next character */
		}
		else return(~NULL); /* match failed on some character */
	}

	return (NULL); /* check if we matched at the end of pattern */
}

