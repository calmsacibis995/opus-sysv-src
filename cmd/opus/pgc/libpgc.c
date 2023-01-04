/* SID @(#)libpgc.c	1.5 */

/*
 * IBM PGC library
 */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "pgc.h"

/* globals */

unsigned char *pgc_bufstart = NULL;	/* start of pgc command buffer */
unsigned char *pgc_bufptr = NULL;	/* curpt in pgc command buffer */
unsigned char *pgc_bufend = NULL;	/* end of pgc command buffer */
int pgc_fn = -1;			/* file id for pgc device */
unsigned char pgc_mallocbuf = 0;	/* buffer gotten with malloc */
int (*pgc_err)();			/* error handler */

/* forwards and externals */

int pgc_error();
extern int errno;

/*
 * Initialize access to the pgc driver and buffering for commands.
 * <buf> is the address of a user
 * supplied buffer that is <len> long. If <buf> == NULL or <len> <= 0.
 * then a default buffer of PGC_DEFAULT bytes is created with malloc.
 * <err> is a pointer to a user supplied error handler. If there is an
 * error during a pgc library command then <err> will be called.
 * If <err> is NULL then a default error handler is used.
 */
pgc_open(buf, len, err)
unsigned char *buf;
int len;
int (*err)();
{

	/* check for non initial call */
	if (pgc_fn >= 0) {
		pgc_bufwrite(0);  /* flush any pending data */
		if (pgc_mallocbuf) {
			free(pgc_bufstart);
		}
	}

	/* set up error handler */
	if (err != NULL) {
		pgc_err = err;
	} else {
		pgc_err = pgc_error;
	}

	/* set up buffer */
	if (buf == NULL || len <= 0) {
		len = PGC_DEFAULT;
		pgc_bufstart = (unsigned char *)malloc(len);
		if (pgc_bufstart == NULL) {
			errno = ENOMEM;
			(*pgc_err)();
			return;
		}
		pgc_mallocbuf = 1;
	} else {
		pgc_bufstart = buf;
		pgc_mallocbuf = 0;
	}
	pgc_bufptr = pgc_bufstart;
	pgc_bufend = &pgc_bufstart[len];

	/* open pgc device */
	if (pgc_fn < 0) {
		pgc_fn = open(PGC_DEVNAME, O_RDWR);
		if (pgc_fn < 0) {
			if (pgc_mallocbuf) {
				free(pgc_bufstart);
			}
			(*pgc_err)();
			return;
		}
	}
}

/*
 * Close the pgc device and free resources.
 */
pgc_close()
{

	/* flush pending commands */
	pgc_bufwrite(0);

	/* free buffer */
	if (pgc_mallocbuf) {
		free(pgc_bufstart);
	}

	/* set pointers so the users pgm will trip up if they are used */
	pgc_bufstart = pgc_bufend = pgc_bufptr = NULL;

	/* close the pgc file */
	close(pgc_fn);
	pgc_fn = -1;
}

/*
 * Write the pgc buffer out to the device.
 * <n> is the number of bytes that the caller expects to put into the
 * buffer after this call completes. If <n> is greater that the total
 * size of the buffer then error trap. Also if there is a file error
 * writing out the buffer then error trap.
 * Note that this routine checks to see if the library has been initialized
 * and if it hasn't then a default initialization is done.
 */
int
pgc_bufwrite(n)
int n;
{
	register int len, rlen;

	if (pgc_fn < 0) {
		pgc_open(NULL, 0, NULL);
	}
	len = pgc_bufptr - pgc_bufstart;
	if (len > 0) {
#if 0
		pgc_hexdump("write\n", pgc_bufstart, len);
#else
		rlen = pgc_writefifo(pgc_bufstart, len);
		if (len != rlen) {
			if (errno != EINTR) {
				(*pgc_err)();
				return;
			}
		}
#endif
	}
	pgc_bufptr = pgc_bufstart;
	if (n > (pgc_bufend - pgc_bufstart)) {
		errno = ENOMEM;
		(*pgc_err)();
		return;
	}
}

/*
 * Default error handler.
 */
pgc_error()
{

	fprintf(stderr, "PGC library error (%d)\n", errno);
	abort();
	exit(1);
}

pgc_hexdump(str, buf, len)
char *str;
char *buf;
int len;
{
	int i;
	char c[16];
	
	fprintf(stderr, "%s", str);
	for (i = 0; i < ((len + 15) & 0xfffffff0); i++) {
		if ((i % 16) == 0) {
			if (i != 0) {
				fprintf(stderr," %.8s %.8s\n", c, &c[8]);
			}
			strncpy(c, "               ", 16);
			if (i < len) {
				fprintf(stderr,"%08x:", i);
			}
		} else if ((i % 8) == 0 && i != 0) {
			fprintf(stderr," ");
		}
		if (i < len) {
			fprintf(stderr," %02x", 0xff & buf[i]);
			c[i % 16] = (buf[i] >= 0x20 && buf[i] < 0x7f) ? buf[i] : '.';
		} else {
			fprintf(stderr,"   ");
			c[i % 16] = ' ';
		}
	}
	fprintf(stderr," %.8s %.8s\n", c, &c[8]);
}

/*
 * Flush all pending writes.
 */
pgc_flush()
{

	if (pgc_fn < 0) {
		pgc_open(NULL, 0, NULL);
	}
	pgc_bufwrite(0);
}

/*
 * Wait for the pgc to execute all data in its write fifo.
 */
pgc_drain()
{

	if (pgc_fn < 0) {
		pgc_open(NULL, 0, NULL);
	}
	pgc_bufwrite(0);
	if (ioctl(pgc_fn, 0, 0) < 0) {
		(*pgc_err)();
	}
}

/*
 * Cause the pgc to coldstart.
 */
pgc_coldstart()
{

	if (pgc_fn < 0) {
		pgc_open(NULL, 0, NULL);
	}
	if (ioctl(pgc_fn, 1, 0) < 0) {
		(*pgc_err)();
	}
}

/*
 * Cause the pgc to warmstart.
 */
pgc_warmstart()
{

	if (pgc_fn < 0) {
		pgc_open(NULL, 0, NULL);
	}
	if (ioctl(pgc_fn, 2, 0) < 0) {
		(*pgc_err)();
	}
}

/*
 * Turn pgc error reporting <onoff>.
 */
pgc_errorenable(onoff)
int onoff;
{

	if (pgc_fn < 0) {
		pgc_open(NULL, 0, NULL);
	}
	if (ioctl(pgc_fn, onoff ? 4 : 3, 0) < 0) {
		(*pgc_err)();
	}
}

/*
 * Write to the pgc write fifo from <buf>
 * which is <len> bytes long.
 * Returns the number of bytes written.
 */
int
pgc_writefifo(buf, len)
char *buf;
int len;
{

	if (pgc_fn < 0) {
		pgc_open(NULL, 0, NULL);
	}
	len = write(pgc_fn, buf, len);
	if (len < 0) {
		(*pgc_err)();
		return(-1);
	}
	return(len);
}

/*
 * Read from the pgc read fifo into <buf>
 * which is <len> bytes long.
 * Returns the number of bytes read.
 */
int
pgc_readfifo(buf, len)
char *buf;
int len;
{

	if (pgc_fn < 0) {
		pgc_open(NULL, 0, NULL);
	}
	lseek(pgc_fn, 0, 0);
	len = read(pgc_fn, buf, len);
	if (len < 0) {
		(*pgc_err)();
		return(-1);
	}
	return(len);
}

/*
 * Read from the pgc error fifo into <buf>
 * which is <len> bytes long.
 * Returns the number of bytes read.
 */
int
pgc_errorfifo(buf, len)
char *buf;
int len;
{

	if (pgc_fn < 0) {
		pgc_open(NULL, 0, NULL);
	}
	lseek(pgc_fn, 1 << 16, 0);
	len = read(pgc_fn, buf, len);
	if (len < 0) {
		(*pgc_err)();
		return(-1);
	}
	return(len);
}
