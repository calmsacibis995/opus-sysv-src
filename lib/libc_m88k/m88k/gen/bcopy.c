/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)bcopy.c	5.1 (Berkeley) 1/27/87";
#endif /* LIBC_SCCS and not lint */

/*
 * bcopy -- vax movc3 instruction
 */
_bcopy(src, dst, length)
	register char *src, *dst;
	register int length;
{
	if (length && src != dst)
		if ((unsigned int)dst < (unsigned int)src)
			if (((int)src | (int)dst | length) & 3)
				do	/* copy by bytes */
					*dst++ = *src++;
				while (--length);
			else {
				length >>= 2;
				do {	/* copy by longs */
					*((long *)dst) = *((long *)src);
					dst += sizeof(long)/sizeof(char);
					src += sizeof(long)/sizeof(char);
				} while (--length);
			}
		else {			/* copy backwards */
			src += length;
			dst += length;
			if (((int)src | (int)dst | length) & 3)
				do	/* copy by bytes */
					*--dst = *--src;
				while (--length);
			else {
				length >>= 2;
				do {	/* copy by longs */
					src -= sizeof(long)/sizeof(char);
					dst -= sizeof(long)/sizeof(char);
					*((long *)dst) = *((long *)src);
				} while (--length);
			}
		}
	return(0);
}
