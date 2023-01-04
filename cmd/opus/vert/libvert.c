/* SID @(#)libvert.c	1.7 */

/*
 * Verticom library
 */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "vert.h"

/* globals */

unsigned char *v_bufstart = NULL;	/* start of verticom command buffer */
unsigned char *v_bufptr = NULL;	/* curpt in verticom command buffer */
unsigned char *v_bufend = NULL;	/* end of verticom command buffer */
int v_fn = -1;			/* file id for verticom device */
unsigned char v_imm = 0;		/* no buffering of commands */
unsigned char v_mallocbuf = 0;	/* buffer gotten with malloc */
int (*v_err)();			/* error handler */

/* forwards and externals */

int v_error();
extern int errno;

/*
 * Initialize access to the verticom driver and buffering for commands.
 * <imm> is 1 if all commands are to be executed
 * immediatly else buffering is done. <buf> is the address of a user
 * supplied buffer that is <len> long. If <buf> == NULL or <len> <= 0.
 * then a default buffer of V_DEFAULT bytes is created with malloc.
 * <err> is a pointer to a user supplied error handler. If there is an
 * error during a verticom library command then <err> will be called.
 * If <err> is NULL then a default error handler is used.
 */
v_open(buf, len, err, imm)
unsigned char *buf;
int len;
int (*err)();
int imm;
{

	/* check for non initial call */
	if (v_fn >= 0) {
		v_bufwrite(0);  /* flush any pending data */
		if (v_mallocbuf) {
			free(v_bufstart);
		}
	}

	/* set up error handler */
	if (err != NULL) {
		v_err = err;
	} else {
		v_err = v_error;
	}

	/* set up buffer */
	if (buf == NULL || len <= 0) {
		len = V_DEFAULT;
		v_bufstart = (unsigned char *)malloc(len);
		if (v_bufstart == NULL) {
			errno = ENOMEM;
			(*v_err)();
			return;
		}
		v_mallocbuf = 1;
	} else {
		v_bufstart = buf;
		v_mallocbuf = 0;
	}
	v_bufptr = v_bufstart;
	v_bufend = &v_bufstart[len];

	/* open verticom device */
	if (v_fn < 0) {
		v_fn = open(V_DEVNAME, O_RDWR);
		if (v_fn < 0) {
			if (v_mallocbuf) {
				free(v_bufstart);
			}
			(*v_err)();
			return;
		}
	}
	v_imm = imm;
}

/*
 * Close the verticom device and free resources.
 */
v_close()
{

	/* flush pending commands */
	v_bufwrite(0);

	/* free buffer */
	if (v_mallocbuf) {
		free(v_bufstart);
	}

	/* set pointers so the users pgm will trip up if they are used */
	v_bufstart = v_bufend = v_bufptr = NULL;

	/* close the verticom file */
	close(v_fn);
	v_fn = -1;
}

/*
 * Write the verticom buffer out to the device.
 * <n> is the number of bytes that the caller expects to put into the
 * buffer after this call completes. If <n> is greater that the total
 * size of the buffer then error trap. Also if there is a file error
 * writing out the buffer then error trap.
 * Note that this routine checks to see if the library has been initialized
 * and if it hasn't then a default initialization is done.
 * There are 3 special values of <n>. If <n> == 0 then the buffer is flushed
 * and reinitialized. If <n> == -1 then the buffer is flushed and the call
 * does not return until all of the buffered graphics output is complete.
 */
int
v_bufwrite(n)
int n;
{
	register int len, rlen;

	if (v_fn < 0) {
		v_open(0, NULL, 0, NULL);
	}
	len = v_bufptr - v_bufstart;
	if (len > 0) {
		if (n < 0 || v_imm) {
			rlen = read(v_fn, v_bufstart, len);
		} else {
			rlen = write(v_fn, v_bufstart, len);
		}
		if (len != rlen) {
			if (errno != EINTR) {
				(*v_err)();
				return;
			}
		}
	} else {
		if (n < 0) {
			ioctl(v_fn, 0, 0);
		}
	}
	v_bufptr = v_bufstart;
	if (n > (v_bufend - v_bufstart)) {
		errno = ENOMEM;
		(*v_err)();
		return;
	}
}

/*
 * Write <buf> for <n> bytes to the frame buffer.
 */
v_write(buf, n)
unsigned char *buf;
register int n;
{
	register unsigned char *p;
	register int len;

	p = buf;
	while (n > 0) {
		len = (n > V_MAXLEN) ? V_MAXLEN : n;
		v_writeframebuf(p, len);
		p += len;
		n -= len;
	}
}

/*
 * Read <buf> from the frame buffer. <n> is initialized to the
 * max size of of <buf>. On return <n> is set to the actual number
 * of bytes transfered.
 */
v_read(buf, n)
unsigned char *buf;
int *n;
{
	register unsigned char *p;
	unsigned char len;
	register int cnt;
	register int actlen;

	p = buf;
	cnt = *n;
	while (cnt > 0) {
		len = (cnt > V_MAXLEN) ? V_MAXLEN : cnt;
		v_readframebuf(p, &len);
		p += len;
		cnt -= len;
		if (len < V_MAXLEN) {
			break;
		}
	}
	*n = *n - cnt;
}

/*
 * Default error handler.
 */
v_error()
{

	fprintf(stderr, "Verticom library error (%d)\n", errno);
	exit(1);
}

v_hexdump(str, buf, len)
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
