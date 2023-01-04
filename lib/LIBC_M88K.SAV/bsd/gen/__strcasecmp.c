/*
**  Copyright (c) 1983 Regents of the University of California.
**  All rights reserved.  The Berkeley software License Agreement
**  specifies the terms and conditions for redistribution.
*/

#include <stdio.h>

#if m88k
/*
 * Case sensitive string comparison routine
 * 
 * Convert str1 and str2 to lower case.
 * Then call strcmp(str1, str2) to do the comparison
 * Used to compare network addresses that are case independent.
 *
 * Side effects: None
 */

__strcasecmp(str1, str2)
char *str1, *str2;
{
	char tmp1[1024]; 	/* local copy of first string */
	char tmp2[1024]; 	/* local copy  of second string */
	register char *p = tmp1, *q = tmp2;
	register char *pst1 = str1, *pst2 = str2;

	while (*pst1 != NULL) 
		*p++ = _tolower(*pst1++);
	*p =  NULL;

	while (*pst2 != NULL) 
		*q++ = _tolower(*pst2++);
	*q =  NULL;

	return ( _strcmp(tmp1, tmp2));
}
#endif
