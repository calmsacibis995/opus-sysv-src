/* SID @(#)doslib.c	1.6 */

/*
 * Library of dos calls
 *
 *	 4/27/88	return dos errors as negative ints
 *			use current include files
 *			add opmon profiling support
 *	 6/03/86	return DOS exit code from syspause()
 */

#include <sys/iorb.h>
#include <sys/doscomm.h>
#include <fcntl.h>
#include <ctype.h>
extern int errno;

int dosfn = -1;  /* file number of dos device */
int doserrno;  /* > 0 if unix error < 0 if dos error */
struct io_gen doscb;  /* control block for dos calls */
union {
	struct sys_ctrl sctrl;	
	struct dos_ctrl dctrl;
} doscmd;

#define DOSFNAME "/dev/dos"  /* device which executes dos commands */

/*
 * Call ioctl to execute a dos command. Initializes access to
 * /dev/dos if necessary.
 */
static
docmd(cmd, arg)
int cmd;
int arg;
{

	if (dosfn < 0) {
		if ((dosfn = open(DOSFNAME, O_RDWR)) < 0) {
			doserrno = errno;
			return(-1);
		}
	}
	if (ioctl(dosfn, cmd, arg) < 0) {
		doserrno = errno;
		return(-1);
	}
	return(0);
}

/*
 * Open a dos file and return the dos handle.
 */
dosopen(fname, access)
char *fname;
int access;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x3d<<8 | access&0xff;
	IORB->io_dx = 0;
	CTRL->reloc = RELOCDX;
	CTRL->buf = fname;
	CTRL->len = strlen(fname)+1;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(IORB->io_ax);
}

/*
 * Close a dos file.
 */
dosclose(handle)
int handle;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x3e<<8;
	IORB->io_bx = handle;
	CTRL->len = 0;
	CTRL->reloc = 0;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(IORB->io_ax);
}

/*
 * Read a dos file.
 */
dosread(handle, buf, len)
int handle;
char *buf;
int len;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x3f<<8;
	IORB->io_bx = handle;
	IORB->io_cx = len;
	IORB->io_dx = 0;
	CTRL->reloc = RELOCDX;
	CTRL->buf = buf;
	CTRL->len = len;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(IORB->io_ax);
}

/*
 * Write a dos file.
 */
doswrite(handle, buf, len)
int handle;
char *buf;
int len;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x40<<8;
	IORB->io_bx = handle;
	IORB->io_cx = len;
	IORB->io_dx = 0;
	CTRL->reloc = RELOCDX;
	CTRL->buf = buf;
	CTRL->len = len;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(IORB->io_ax);
}

/*
 * Seek a dos file.
 */
doslseek(handle, offset, method)
int handle;
int offset;
int method;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x42<<8 | method&0xff;
	IORB->io_bx = handle;
	IORB->io_cx = (offset>>16)&0xffff;
	IORB->io_dx = offset&0xffff;
	CTRL->len = 0;
	CTRL->reloc = 0;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return((IORB->io_dx<<16)|IORB->io_ax);
}

/*
 * Delete a dos file
 */
dosunlink(fname)
char *fname;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x41<<8;
	IORB->io_dx = 0;
	CTRL->reloc = RELOCDX;
	CTRL->buf = fname;
	CTRL->len = strlen(fname)+1;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(0);
}

/*
 * Set the access permissions of a dos file.
 */
doschmod(fname, func, attributes)
char *fname;
int func;
int attributes;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x43<<8 | func&1;
	IORB->io_cx = attributes;
	IORB->io_dx = 0;
	CTRL->reloc = RELOCDX;
	CTRL->buf = fname;
	CTRL->len = strlen(fname)+1;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(IORB->io_cx);
}

/*
 * Ioctl.
 */
dosioctl(handle, cmd, buf, len)
int handle;
int cmd;
char *buf;
int len;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x44<<8 | cmd&0xff;
	IORB->io_bx = handle;
	IORB->io_cx = len;
	IORB->io_dx = 0;
	CTRL->reloc = 0;
	CTRL->len = 0;
	switch (cmd&0xff) {

		case 1:
		case 0xb:
		IORB->io_dx = *(short *)buf;
		break;

		case 2:
		case 3:
		case 4:
		case 5:
		CTRL->buf = buf;
		CTRL->len = len;
		CTRL->reloc = RELOCDX;
		break;
	}
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	switch (cmd&0xff) {

		case 0:
		case 9:
		case 0xa:
		*(short *)buf = IORB->io_dx;
		break;
	}
	return(IORB->io_ax);
}

/*
 * Create a dos file.
 */
doscreat(fname, attributes)
char *fname;
int attributes;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x3c<<8;
	IORB->io_cx = attributes;
	IORB->io_dx = 0;
	CTRL->reloc = RELOCDX;
	CTRL->buf = fname;
	CTRL->len = strlen(fname)+1;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(IORB->io_ax);
}

/*
 * Remove a dos directory.
 */
dosrmdir(fname)
char *fname;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x3a<<8;
	IORB->io_dx = 0;
	CTRL->reloc = RELOCDX;
	CTRL->buf = fname;
	CTRL->len = strlen(fname)+1;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(0);
}

/*
 * Create a dos directory.
 */
dosmkdir(fname)
char *fname;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x39<<8;
	IORB->io_dx = 0;
	CTRL->reloc = RELOCDX;
	CTRL->buf = fname;
	CTRL->len = strlen(fname)+1;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(0);
}

/*
 * Change to a new current dos directory.
 */
doschdir(fname)
char *fname;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x3b<<8;
	IORB->io_dx = 0;
	CTRL->reloc = RELOCDX;
	CTRL->buf = fname;
	CTRL->len = strlen(fname)+1;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(0);
}

/*
 * Rename a dos file.
 */
dosrename(oldname, newname)
char *oldname;
char *newname;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;
	char names[2][128];  /* temp area for passing names */

	IORB->io_ax = 0x56<<8;
	IORB->io_dx = 0;
	IORB->io_di = 128;
	CTRL->reloc = RELOCDX|RELOCDI;
	CTRL->buf = names[0];
	CTRL->len = sizeof(names);
	CTRL->dptr = (struct io_gen *)IORB;
	strncpy(names[0], oldname, 127);
	names[0][127] = 0;
	strncpy(names[1], newname, 127);
	names[1][127] = 0;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(0);
}

/*
 * Find first/next matching file
 */
dosfindfirst(dos_find)
struct dos_find *dos_find;
{
	return(dosfind(dos_find, 0x4e));
}

dosfindnext(dos_find)
struct dos_find *dos_find;
{
	return(dosfind(dos_find, 0x4f));
}

dosfind(dos_find, cmd)
register struct dos_find *dos_find;
int cmd;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;
	register int i;
	int c, lc, uc;

	IORB->io_ax = cmd<<8;
	IORB->io_bx = ((char *)dos_find->dta) - ((char *)dos_find);
	IORB->io_dx = ((char *)dos_find->path) - ((char *)dos_find);
	IORB->io_cx = dos_find->attr;
	CTRL->reloc = RELOCBX|RELOCDX;
	CTRL->buf = (char *)dos_find;
	CTRL->len = sizeof(struct dos_find);
	CTRL->dptr = (struct io_gen *)IORB;
	dos_find->fill[0] = 0;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}

	/* check lower- & uppercase chars in search path */
	for (i=lc=uc=0; c = dos_find->path[i]; ++i) {
		if (islower(c)) {
			++lc;
			break;	/* say no more */
		} else if (isupper(c))
			++uc;
	}
	/* if any lowercase or no uppercase, convert result to lowercase */
	if (lc != 0 || uc == 0) {
		for (i=0; c = dos_find->name[i]; ++i) {
			dos_find->name[i] = tolower(c);
		}
	}
	return(0);
}

/*
 * Get dos disk free space.
 */
dosdiskfree(drive, sectperclust, numclust, totclust, bytespersect)
int drive;
int *sectperclust, *numclust, *totclust, *bytespersect;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0x36<<8;
	IORB->io_dx = drive;
	CTRL->len = 0;
	CTRL->reloc = 0;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	if (IORB->io_ax == 0xffff) {
		doserrno = -15;  /* drive not found */
		return(-1);
	}
	*sectperclust = IORB->io_ax;
	*numclust = IORB->io_bx;
	*totclust = IORB->io_dx;
	*bytespersect = IORB->io_cx;
	return(0);
}

/*
 * Change to new default drive.
 */
doschdrv(drive)
int drive;
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0xe<<8;
	IORB->io_dx = drive&0xff;
	CTRL->len = 0;
	CTRL->reloc = 0;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(IORB->io_ax&0xff);
}

/*
 * Wait for return to foreground mode
 */
doswaitfg()
{
	register struct io_dos *IORB = (struct io_dos *)&doscb;
	register struct dos_ctrl *CTRL = &doscmd.dctrl;

	IORB->io_ax = 0xb<<8;
	CTRL->len = 0;
	CTRL->reloc = 0;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(DOSCMD, CTRL) < 0) {
		return(-1);
	}
	if (IORB->io_error == E_DEV) {
		doserrno = -((int) IORB->io_ax);
		return(-1);
	}
	return(0);
}

/*
 * Pause to dos
 */
syspause(exitcode)
int exitcode;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_PAUSE;
	IORB->io_memaddr = exitcode&0xff;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(IORB->io_devstat);
}

/*
 * Exit to dos
 */
sysexit(exitcode)
int exitcode;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_DOSRTN;
	IORB->io_memaddr = exitcode&0xff;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(0);
}

/*
 * Fetch dos command line
 */
syscmdline(buf, len)
char *buf;
int len;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_CMDLINE;
	IORB->io_memaddr = 0;
	IORB->io_bcount = len;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = buf;
	CTRL->len = len;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(IORB->io_bcount);
}

/*
 * Execute a Lattice c fork call.
 */
sysexec(buf, len)
char *buf;
int len;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_EXEC;
	IORB->io_memaddr = 0;
	IORB->io_bcount = len;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = buf;
	CTRL->len = len;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(IORB->io_bcount);
}

/*
 * Read a PC I/O byte
 */
sysreadio(adx)
int adx;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_READIO;
	IORB->io_memaddr = adx;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(IORB->io_bcount & 0xff);
}

/*
 * Write a PC I/O byte
 */
syswriteio(adx, val)
int adx;
int val;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_WRITEIO;
	IORB->io_memaddr = adx;
	IORB->io_bcount = val & 0xff;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(0);
}

/*
 * Read a PC I/O word
 */
sysreadiow(adx)
int adx;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_READIOW;
	IORB->io_memaddr = adx;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(IORB->io_bcount & 0xffff);
}

/*
 * Write a PC I/O word
 */
syswriteiow(adx, val)
int adx;
int val;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_WRITEIOW;
	IORB->io_memaddr = adx;
	IORB->io_bcount = val & 0xffff;
	CTRL->dptr = (struct io_gen *)IORB;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(0);
}

#ifndef pm100
/*
 * Read byte string from PC I/O.
 */
sysreadiobs(adx, buf, len)
int adx;
char *buf;
int len;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_READIOBS;
	IORB->io_memaddr = adx;
	IORB->io_bcount = len;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = buf;
	CTRL->len = len;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(IORB->io_bcount);
}

/*
 * Read word string from PC I/O.
 */
sysreadiows(adx, buf, len)
int adx;
char *buf;
int len;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_READIOWS;
	IORB->io_memaddr = adx;
	IORB->io_bcount = len;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = buf;
	CTRL->len = len;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(IORB->io_bcount);
}

/*
 * Write byte string to PC I/O.
 */
syswriteiobs(adx, buf, len)
int adx;
char *buf;
int len;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_WRITEIOBS;
	IORB->io_memaddr = adx;
	IORB->io_bcount = len;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = buf;
	CTRL->len = len;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(IORB->io_bcount);
}

/*
 * Write word string to PC I/O.
 */
syswriteiows(adx, buf, len)
int adx;
char *buf;
int len;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_WRITEIOWS;
	IORB->io_memaddr = adx;
	IORB->io_bcount = len;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = buf;
	CTRL->len = len;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(IORB->io_bcount);
}
#endif

/*
 * Read bytes from PC memory.
 */
sysreadmem(segment, offset, buf, len)
unsigned short segment;
unsigned short offset;
char *buf;
int len;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_READMEM;
	IORB->io_memaddr = 0;
	IORB->io_bcount = len;
	IORB->io_blknum = offset;
	IORB->io_hblknum = segment;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = buf;
	CTRL->len = len;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(IORB->io_bcount);
}

/*
 * Write bytes to PC memory.
 */
syswritemem(segment, offset, buf, len)
unsigned short segment;
unsigned short offset;
char *buf;
int len;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_WRITEMEM;
	IORB->io_memaddr = 0;
	IORB->io_bcount = len;
	IORB->io_blknum = offset;
	IORB->io_hblknum = segment;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = buf;
	CTRL->len = len;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(IORB->io_bcount);
}

/*
 * Get DOS environment variable.
 */
sysgetenv(buf, len)
char *buf;
int len;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_GETENV;
	IORB->io_memaddr = 0;
	IORB->io_bcount = len;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = buf;
	CTRL->len = len;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(0);
}

/*
 * Set DOS environment variable.
 */
sysputenv(buf, len)
char *buf;
int len;
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_PUTENV;
	IORB->io_memaddr = 0;
	IORB->io_bcount = len;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = buf;
	CTRL->len = len;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(0);
}

/*
 * Issue general interrupt.
 */
dosinterrupt(ctrl, iorb)
struct	int_ctrl *ctrl;
struct  io_int *iorb;
{
	ctrl->dptr = (struct io_gen *)iorb;
	if (docmd(INTCMD, ctrl) < 0) {
		return(-1);
	}
	return(0);
}

/*
 * Start profiling.
 */
sysstartprofile()
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_PRFSTART;
	IORB->io_memaddr = 0;
	IORB->io_bcount = 0;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = 0;
	CTRL->len = 0;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(0);
}

/*
 * Stop profiling.
 */
sysstopprofile()
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_PRFSTOP;
	IORB->io_memaddr = 0;
	IORB->io_bcount = 0;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = 0;
	CTRL->len = 0;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(0);
}

/*
 * Save profiling.
 */
syssaveprofile()
{
	register struct io_sys *IORB = (struct io_sys *)&doscb;
	register struct sys_ctrl *CTRL = &doscmd.sctrl;

	IORB->io_cmd = PC_PRFSAVE;
	IORB->io_memaddr = 0;
	IORB->io_bcount = 0;
	CTRL->dptr = (struct io_gen *)IORB;
	CTRL->buf = 0;
	CTRL->len = 0;
	if (docmd(SYSCMD, CTRL) < 0) {
		return(-1);
	}
	return(0);
}
