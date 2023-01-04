/*  @(#)v0.c	1.2 */

/*
 * Utilitiy functions used by vdi routines.
 */

#include <sys/vdicomm.h>

#define VDIFNAME "/dev/vdi"  /* file name of vdi communication device */
#define VDIBUFLEN 1024  /* number of 16 bit values in std vdi buffer */

struct vdi_parm {
	int type;
	int val;
};

short *vdi_bufstart;  /* start of vdi buffer */
short *vdi_bufend;  /* end of vdi buffer */
short *vdi_bufptr;  /* next available space in vdi buffer */
short *vdi_link;  /* pointer to link for previous command */
char vdi_buffering;  /* true if buffering is turned on */
int vdi_fn = -1;  /* file number for vdi communications */
int (*vdi_procname)();  /* vdi output procedure */
short vdi_buffer[VDIBUFLEN];  /* std vdi buffer */
char vdi_initialized;  /* a vdi_open call has been made */

int vdi_write();

vdi_open(immediate, buffer, len, procname)
int immediate;
char *buffer;
int len;
int (*procname)();
{

	if (vdi_initialized) {
		return(-1);
	}
	if (procname == 0) {
		if ((vdi_fn = open(VDIFNAME, 2)) < 0) {
			return(-1);
		}
	}
	vdi_buffering = immediate ? 0 : 1;
	vdi_procname = procname ? procname : vdi_write;
	if (buffer == 0) {
		vdi_bufptr = vdi_bufstart = vdi_buffer;
		vdi_bufend = &vdi_bufstart[VDIBUFLEN];
	} else {
		vdi_bufptr = vdi_bufstart = (short *)buffer;
		vdi_bufend = &vdi_bufstart[len>>1];
	}
	vdi_initialized = 1;
	return(0);
}

vdi_close()
{
	return(vdi_flush());
}

vdi_flush()
{
	struct vdi_ctrl ctrl;
	int err;

	if (vdi_bufstart == vdi_bufptr) {
		return(0);
	}
	err = (*vdi_procname)((char *)vdi_bufstart,
			      (vdi_bufptr - vdi_bufstart) * 2);
	vdi_bufptr = vdi_bufstart;
	vdi_link = 0;
	return(err);
}

vdi_write(buf, len)
char *buf;
int len;
{
	struct vdi_ctrl ctrl;

	ctrl.buf = buf;
	ctrl.len = len;
	if (ioctl(vdi_fn, VDICMD, &ctrl) != 0) {
		return(-1);
	} else {
		return(0);
	}
}

vdi_cmd(cmd, nparm, datalen, outputs)
int cmd;
int nparm;
int datalen;
int outputs;
{
	register struct vdi_parm *p;
	register int totlen;
	struct vdi_parm *parmstart, *parmend;
	short *requeststart;
	register short *rp, *pp;

	if (!vdi_initialized) {
		if (vdi_open(1, 0, 0, 0)) {
			return(-1);
		}
		vdi_initialized = 1;
	}
	totlen = 3 + nparm + datalen;
	if (totlen > (vdi_bufend - vdi_bufptr)) {
		if (vdi_flush())
			return(-1);

		/* check for request bigger than whole buffer */
		if (totlen > (vdi_bufend - vdi_bufptr)) {
			return(-1);
		}
	}
	parmstart = (struct vdi_parm *)((&outputs)+1);
	parmend = &parmstart[nparm - 1];
	rp = requeststart = vdi_bufptr;
	if (vdi_link) {
		*vdi_link = (rp - vdi_bufstart)<<1;
	}
	*rp++ = cmd;
	*rp++ = 0;
	*rp++ = 0;
	vdi_link = &requeststart[2];
	pp = &requeststart[3];
	rp = &requeststart[3+nparm];
	for (p = parmend; p >= parmstart; p--) {
		if (p->type == 0) {
			*pp++ = p->val;
		} else if (p->type > 0) {
			outputs = 1;
			*pp++ = (rp - vdi_bufstart)<<1;
			rp += p->type;
		} else {
			*pp++ = (rp - vdi_bufstart)<<1;
			memcpy(rp, p->val, (-p->type)*2);
			rp += -p->type;
		}
	}
	vdi_bufptr = rp;
	if (!vdi_buffering || outputs) {
		if (vdi_flush())
			return(-1);
		if (outputs) {
			for (p = parmend, pp = &requeststart[3];
			     p >= parmstart; p--, pp++) {
				if (p->type > 0) {
					memcpy(p->val, &vdi_bufstart[*pp>>1], (p->type)*2);
				}
			}
		}
		return(requeststart[1]);
	}
	return(0);
}
