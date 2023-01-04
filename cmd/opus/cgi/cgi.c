/* %Q% %W% */

/*
 * Utilitiy functions used by cgi routines.
 *
 *	10/01/86	cgi_cmd() now gets char counts
 *	10/07/86	add comments
 */

#include <stdio.h>
#include <cgi.h>

#define CGIFNAME "/dev/cgi"  /* file name of cgi communication device */
#define CGIBUFLEN 1024  /* number of 16 bit values in std cgi buffer */

struct cgi_parm {
	int type;
	int val;
};

short *cgi_bufstart;	/* start of cgi buffer */
short *cgi_bufend;	/* end of cgi buffer */
short *cgi_bufptr;	/* next available space in cgi buffer */
short *cgi_link;	/* pointer to link for previous command */
int cgi_buffering;	/* true if buffering is turned on */
int cgi_fn = -1;	/* file number for cgi communications */
int (*cgi_procname)();	/* cgi output procedure */
short cgi_buffer[CGIBUFLEN];	/* std cgi buffer */
int cgi_initialized;	/* a cgi_open call has been made */

int cgi_write();

cgi_open(immediate, buffer, len, procname)
int immediate;
char *buffer;
int len;
int (*procname)();
{
	if (cgi_initialized) {
		return(-1);
	}
	if (procname == 0) {
		if ((cgi_fn = open(CGIFNAME, 2)) < 0) {
			return(-1);
		}
	}
	cgi_buffering = immediate ? 0 : 1;
	cgi_procname = procname ? procname : cgi_write;
	if (buffer == 0) {
		cgi_bufptr = cgi_bufstart = cgi_buffer;
		cgi_bufend = &cgi_bufstart[CGIBUFLEN];
	} else {
		cgi_bufptr = cgi_bufstart = (short *)buffer;
		cgi_bufend = &cgi_bufstart[len>>1];
	}
	cgi_link = NULL;
	cgi_initialized = 1;
	return(0);
}

cgi_close()
{
	return(cgi_flush());
}

cgi_flush()
{
	int err;

	if (cgi_bufstart == cgi_bufptr) {
		return(0);
	}
	err = (*cgi_procname)((char *)cgi_bufstart,
			      (cgi_bufptr - cgi_bufstart) * 2);
	cgi_bufptr = cgi_bufstart;
	cgi_link = NULL;
	return(err);
}

cgi_write(buf, len)
char *buf;
int len;
{
	if (read(cgi_fn, buf, len) != len) {
		return(-1);
	}
	return(0);
}

cgi_cmd(cmd, nparm, imm, parm0)
int cmd;
int nparm;
int imm;
struct cgi_parm parm0;
{
	register struct cgi_parm *p;
	register short *rp, *pp;
	struct cgi_parm *firstparm;
	register struct cgi_parm *lastparm;
	short *requeststart;
	int totlen, err, cnt;

	if (!cgi_initialized && cgi_open(1, 0, 0, 0))
		return(-1);

	/*  firstparm/lastparm point to first/last cgi parameter on stack
	 */
	firstparm = &parm0;
	lastparm = &firstparm[nparm - 1];

	/*  totlen is total length of command buffer (in shorts)
	 */
	totlen = 3 + nparm;
	for (p = firstparm; p <= lastparm; ++p) {
		totlen += ((p->type>0 ? p->type : -p->type) + 1) >> 1;
	}

	/*  flush buffer if required for space
	 */
	if (totlen > (cgi_bufend - cgi_bufptr)) {
		if (cgi_flush())
			return(-1);

		/* check for request bigger than whole buffer */
		if (totlen > (cgi_bufend - cgi_bufptr)) {
			return(-1);
		}
	}

	/*  requeststart will point to beginning of this command buffer
	 *  rp will track entries into the buffer
	 *  pp will track cgi parameters while rp tracks parameter buffers
	 *
	 *  buf[0] = command code
	 *  buf[1] = status return
	 *  buf[2] = link to next command, if any
	 *  buf[3] = first cgi parameter, if any
	 *  ...
	 *  buf[3+nparm] = first cgi parameter buffer, if any
	 *			(used for pointer parameters)
	 *  ...
	 */
	rp = requeststart = cgi_bufptr;
	if (cgi_link) {
		*cgi_link = (rp - cgi_bufstart)<<1;
	}
	*rp++ = cmd;
	*rp++ = 0;
	cgi_link = rp;
	*rp++ = 0;
	pp = rp;
	rp = pp + nparm;	/* make space for parameters */

	/*  put parameters in buffer
	 *  if type == 0 parameter is simple value (short)
	 *  if type > 0, parameter is pointer to (char buffer[type])
	 *		 and cgi returns data in buffer
	 *  if type < 0, parameter is pointer to (char buffer[type])
	 *		 and we pass data to cgi in buffer
	 */
	for (p = firstparm; p <= lastparm; ++p) {
		if (p->type == 0) {
			*pp++ = p->val;
		} else if (p->type > 0) {
			*pp++ = (char *)rp - (char *)cgi_bufstart;
			rp += (p->type+1) >> 1;
			imm = 1;	/* force immediate execution */
		} else {
			int cnt = -p->type;
			*pp++ = (char *)rp - (char *)cgi_bufstart;
			memcpy(rp, p->val, cnt);	/* copy data to cgi */
			rp += (cnt+1) >> 1;
		}
	}
	cgi_bufptr = rp;

	/*  execute command if necessary
	 *  (buffering off; immediate command; command with inputs)
	 */
	if (!cgi_buffering || imm) {
		if (cgi_flush())
			return(-1);

		/*  scan for data returned by cgi
		 */
		if (imm) {
			for (p = firstparm, pp = &requeststart[3];
			     p <= lastparm; ++p, ++pp) {
				if (p->type > 0) {
					memcpy(p->val, &cgi_bufstart[*pp>>1], p->type);
				}
			}
		}
		return(requeststart[1]);	/* return status word */
	}
	return(0);	/* return 0 if command was buffered */
}
