/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	Copyright (c) 1986 by National Semiconductor Corp.	*/
/*	All Rights Reserved					*/

#ident	"@(#)edio.c	1.6	10/20/86 Copyright (c) 1986 by National Semiconductor Corp."

/* This file contains the interface subroutines for accessing
 * the hdelog driver and, indirectly through that driver,
 * for accessing the equipped disks for physical device I/O.
 * These interface subroutines maintain a discipline of
 * using at most one equipped disk at a time.
 */

#define	HDELOG			/* don't depend on the makefile	*/

#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/disk.h"
#include "sys/hdelog.h"
#include "sys/hdeioctl.h"
#include "edio.h"
#include "hdecmds.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

extern char *malloc(), *realloc();
extern int errno;
extern char *cmdname;
extern char *sys_errlist[];
dev_t edisopen = -1;
long edsecsz;
daddr_t edpdsno;
int edisfix;
daddr_t edenddad;

struct	eddata edpdsec;		/* physical description sector	*/
struct	eddata eddmap;		/* machine form of defect map	*/
struct	eddata eddm;		/* standard form of defect map	*/
struct	eddata edhdel;		/* hard disk error log		*/
struct	eddata edvtoc;		/* vtoc sector			*/
#ifdef	HDELOG
struct	eddata badmap;		/* current bad map layout	*/
#endif	/* HDELOG */

static struct hdearg edarg, edzarg;

static int hdelfdes = -1;
int edcnt;
dev_t *edtable;
struct hdeedd *ededdp;

eddevck(dev)		/* return: 0 == good, -1 == bad */
register dev;
{
	register int i;

	if (hdelfdes < 0) {	/* need to do edinit */
		edinit();
	}
	for (i = 0; i < edcnt; i++)
		if (dev == edtable[i]) return(0);
	return(-1);
}

edinit()
{
	register int i;

	if ((hdelfdes = open("/dev/hdelog", O_RDWR)) == -1) {
		fprintf(stderr, "%s: open of /dev/hdelog failed: %s\n",
			cmdname,
			sys_errlist[errno]);
		exit(ERREXIT);
	}
	edarg = edzarg;
	if (ioctl(hdelfdes, HDEGEDCT, &edarg) < 0) {
		fprintf(stderr, "%s: cannot get equipped disk count\n",cmdname);
		hdeerror(&edarg);
		exit(ERREXIT);
	}
	edcnt = edarg.hdersize;
	if (edcnt <= 0) {
		fprintf (stderr, "%s: no equipped disks?\n", cmdname);
		exit(ERREXIT);
	}
	if (!(edtable = (dev_t *) malloc(edcnt * (sizeof *edtable)))) {
		fprintf(stderr, "%s: malloc of edtable failed\n", cmdname);
		exit(ERREXIT);
	}
	if (!(ededdp = (struct hdeedd *) malloc(edcnt * (sizeof *ededdp)))) {
		fprintf(stderr, "%s: malloc of ededd failed\n", cmdname);
		exit(ERREXIT);
	}
	edarg = edzarg;
	edarg.hdebody.hdegeqdt.hdeeqdct = edcnt;
	edarg.hdebody.hdegeqdt.hdeudadr = ededdp;
	if (ioctl(hdelfdes, HDEGEQDT, &edarg) < 0) {
		fprintf(stderr,"%s: cannot get equipped disk table\n", cmdname);
		hdeerror(&edarg);
		exit(ERREXIT);
	}
	for (i = 0; i < edcnt; i++) edtable[i] = ededdp[i].hdeedev;
}

edopen(dev)
dev_t dev;
{
	if (edisopen != -1) {
		fprintf(stderr, "%s: equipped disk still open\n", cmdname);
		if (edisopen == dev) {
			fprintf(stderr, "%s: it is same as new disk\n",
				cmdname);
			return(0);
		}
		fprintf(stderr, "%s: it is different, so close old one\n",
			cmdname);
		edclose();
	}
	if (eddevck(dev)) {
		fprintf(stderr, "%s: bad equipped disk dev\n", cmdname);
		exit(ERREXIT);
	}
	if ((edpdsno = edgetpdsno(dev)) == -1) {
		fprintf(stderr, "%s: cannot get pd sector number");
		fprintf(stderr, ": disk dev %d/%d\n", major(dev), minor(dev));
		return(-1);
	}
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdeddev = dev;
	if (ioctl(hdelfdes, HDEOPEN, &edarg) < 0) {
		fprintf(stderr, "%s: open of equipped disk %d/%d failed\n",
			cmdname,
			major(dev), minor(dev));
		perror("HDEOPEN");
		hdeerror(&edarg);
		return(-1);
	}
	edisopen = dev;
	edarg.hdebody.hdedskio.hdeuaddr = (char *) &edsecsz;
	edarg.hdebody.hdedskio.hdebcnt = sizeof edsecsz;
	if (ioctl(hdelfdes, HDEGETSS, &edarg) < 0) {
		fprintf (stderr, "%s: cannot get sector size", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n", major(dev), minor(dev));
		perror("HDEGETSS");
		hdeerror(&edarg);
		edclose();
		return(-1);
	}
	doalloc(&edpdsec, edsecsz, "pdsec");
	edarg.hdebody.hdedskio.hdeuaddr = edpdsec.badr;
	edarg.hdebody.hdedskio.hdebcnt = edsecsz;
	if (ioctl(hdelfdes, HDERDPD, &edarg) < 0) {
		fprintf(stderr, "%s: read of pdsec failed", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n", major(dev), minor(dev));
		perror("HDERDPD");
		hdeerror(&edarg);
		edclose();
		return(-1);
	}
	edpdsec.valid = 1;
#ifdef	HDELOG
	if (edpdp->dt_sanity == VALID_PD) {
		edenddad = edpdp->dt_cyls * edpdp->dt_tracks *
			edpdp->dt_sectors;
#else	/* HDELOG */
	if (edpdp->pdinfo.sanity == VALID_PD) {
		edenddad = edpdp->pdinfo.cyls * edpdp->pdinfo.tracks *
			edpdp->pdinfo.sectors;
#endif	/* HDELOG */
	} else {
		edenddad = -1;
#ifdef	DEBUG
		fprintf(stderr,"%s: invalid sanity in physical description\n",
			cmdname);
#endif	/* DEBUG */
	}
#ifdef	DEBUG
	fprintf(stderr,"%s: equipped disk open succeeded: disk dev %d/%d\n",
			cmdname,major(edisopen), minor(edisopen));
#endif	/* DEBUG */
	return(0);
}

hdeerror(err)
struct hdearg *err;
{
	fprintf(stderr,"HDE-ERROR: ");
	switch (err->hderval)
	{
		case HDE_UNKN:
			printf("UNKN - unknown");
			break;

		case HDE_SZER:
			printf("SZER - size error");
			break;

		case HDE_NODV:
			printf("NODV - no device");
			break;

		case HDE_BADO:
			printf("BADO - bad open");
			break;

		case HDE_NOTO:
			printf("NOTO - not open");
			break;

		case HDE_NOTD:
			printf("NOTD - not daemon (hdelogger)");
			break;

		case HDE_TWOD:
			printf("TWOD - more than one daemon (hdelogger)");
			break;

		case HDE_NOTF:
			printf("NOTF - not fixer (hdefix)");
			break;

		case HDE_TWOF:
			printf("TWOF - more than one fixer (hdefix)");
			break;

		case HDE_CANT:
			printf("CANT - can't do it");
			break;

		case HDE_IOE :
			printf("IOE  - physical I/O error");
			break;

		case HDE_BADR:
			printf("BADR - bad address");
			break;

		default	     :
			printf("???? - invalid error code");
			break;
	}
	printf("\n");
}

edclose()
{
	if (edisopen == -1) return(0);
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdeddev = edisopen;
	if (ioctl(hdelfdes, HDECLOSE, &edarg) < 0) {
		fprintf(stderr, "%s: equipped disk close failed", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		perror("HDECLOSE");
		hdeerror(&edarg);
	}
	edpdsec.valid = 0;
	eddmap.valid = 0;
	eddm.valid = 0;
	edhdel.valid = 0;
	edvtoc.valid = 0;
#ifdef	DEBUG
	fprintf(stderr,"%s: equipped disk close succeeded: disk dev %d/%d\n",
			cmdname, major(edisopen), minor(edisopen));
#endif	/* DEBUG */
	edisopen = -1;
}

edpdck()
{
	if (!edpdsec.valid) {
		fprintf(stderr, "%s: don't have a valid copy of pdsec\n",
			cmdname);
		return(-1);
	}
#ifdef	HDELOG
	if (edpdp->dt_sanity != VALID_PD) {
#else	/* HDELOG */
	if (edpdp->pdinfo.sanity != VALID_PD) {
#endif	/* HDELOG */
		fprintf(stderr, "%s: pdsec not in valid format", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		return(-1);
	}
	return(0);
}

edgetdm()
{
	register int sz;
	register struct hddmap *mapp;
	register struct hddm *mp;
	register int cylsz, trksz;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
#ifdef	HDELOG
	if (((long)edpdp->dt_defectst < (long)1) ||
		((long)edpdp->dt_defectsz <= (long)0)) {
		fprintf(stderr, "%s: no defect map area on disk\n", cmdname);
		eddm.valid = 0;
		eddmap.valid = 0;
		return(-1);
	}
#endif	/* HDELOG */
	if (eddm.valid) return(0);
	if(!eddmap.valid) {
		edallocdm();
		edarg = edzarg;
		edarg.hdebody.hdedskio.hdebcnt = eddmap.csz;
		edarg.hdebody.hdedskio.hdeuaddr = eddmap.badr;
#ifdef	HDELOG
		edarg.hdebody.hdedskio.hdedaddr = edpdp->dt_defectst;
#else	/* HDELOG */
		edarg.hdebody.hdedskio.hdedaddr = edpdp->pdinfo.defectst;
#endif	/* HDELOG */
		edarg.hdebody.hdedskio.hdeddev = edisopen;
		if (ioctl(hdelfdes, HDERDISK, &edarg) < 0) {
			fprintf(stderr, "%s: read of defect map failed",
				cmdname);
			fprintf(stderr, ": disk dev %d/%d\n",
				major(edisopen), minor(edisopen));
			perror("HDERDISK edgetdm");
			hdeerror(&edarg);
			return(-1);
		}
		eddmap.valid = 1;
	}
#ifdef	HDELOG
	sz = edpdp->dt_defectsz / sizeof(struct hddmap);
	mapp = eddmapp;
	trksz = edpdp->dt_sectors;
	cylsz = edpdp->dt_tracks * trksz;
#else	/* HDELOG */
	sz = edpdp->pdinfo.defectsz / sizeof(struct hddmap);
	mapp = eddmapp;
	trksz = edpdp->pdinfo.sectors;
	cylsz = edpdp->pdinfo.tracks * trksz;
#endif	/* HDELOG */
	for (mp = eddmp; sz > 0; mapp++, mp++, --sz) {
		if (mapp->frmadr.lval == -1) {
			mp->frmblk = -1;
			mp->toblk = -1;
			continue;
		}
		mp->frmblk = mapp->frmadr.cts.secno +
			mapp->frmadr.cts.trkno * trksz +
			mapp->frmadr.cts.cylno * cylsz;
		if (mapp->toadr.lval == -1) {
			fprintf(stderr, "%s: bad defect map entry in slot %d\n",
				cmdname, mp - eddmp);
			fprintf(stderr, "\tfrom block %d has not to block\n",
				mp->frmblk);
			mp->toblk = -1;
			continue;
		}
		mp->toblk = mapp->toadr.cts.secno +
			mapp->toadr.cts.trkno * trksz +
			mapp->toadr.cts.cylno * cylsz;
	}
	eddm.valid = 1;
	return(0);
}

edallocdm()
{
	register int sz;

	if(edpdck()) return(-1);
#ifdef	HDELOG
	sz = (edpdp->dt_defectsz + edsecsz-1)/edsecsz;
#else	/* HDELOG */
	sz = (edpdp->pdinfo.defectsz + edsecsz-1)/edsecsz;
#endif	/* HDELOG */
	sz = sz * edsecsz;
	doalloc(&eddmap, sz, "eddmap");
	doalloc(&eddm, sz, "eddm");
	return(0);
}

edputdm(force)
{
	register int sz;
	register struct hddmap *mapp;
	register struct hddm *mp;
	register int cylsz, trksz;
	register int i;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
	if (!eddm.valid) {
		fprintf(stderr, "%s: eddm not valid\n", cmdname);
		return(-1);
	}
	if (!eddmap.valid) {
		fprintf(stderr, "%s: eddmap not valid\n", cmdname);
		return(-1);
	}
	if(edpdck()) return(-1);
	mapp = eddmapp;
	mp = eddmp;
#ifdef	HDELOG
	trksz = edpdp->dt_sectors;
	cylsz = edpdp->dt_tracks * trksz;
#else	/* HDELOG */
	trksz = edpdp->pdinfo.sectors;
	cylsz = edpdp->pdinfo.tracks * trksz;
#endif	/* HDELOG */
	for (sz = eddmcnt; sz > 0; mapp++, mp++, --sz) {
		if (mp->frmblk == -1) {
			mapp->frmadr.lval = -1;
			mapp->toadr.lval = -1;
			continue;
		}
		mapp->frmadr.cts.cylno = i = mp->frmblk / cylsz;
		i = mp->frmblk - (i * cylsz);
		mapp->frmadr.cts.trkno = i / trksz;
		mapp->frmadr.cts.secno = i % trksz;
		if (mp->toblk == -1) {
			fprintf("%s: slot %d of new defect table is bad\n",
				cmdname, sz - eddmcnt);
			fprintf(stderr, "from block %d has no to block\n",
				mp->frmblk);
			if (force) {
				mapp->toadr.lval = -1;
				continue;
			}
			fprintf(stderr, "defect map not written\n");
			eddm.valid = 0;
			eddmap.valid = 0;
			return(-1);
		}
		mapp->toadr.cts.cylno = i = mp->toblk / cylsz;
		i = mp->toblk - (i * cylsz);
		mapp->toadr.cts.trkno = i / trksz;
		mapp->toadr.cts.secno = i % trksz;
	}
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdebcnt = eddmap.csz;
	edarg.hdebody.hdedskio.hdeuaddr = eddmap.badr;
#ifdef	HDELOG
	edarg.hdebody.hdedskio.hdedaddr = edpdp->dt_defectst;
#else	/* HDELOG */
	edarg.hdebody.hdedskio.hdedaddr = edpdp->pdinfo.defectst;
#endif	/* HDELOG */
	edarg.hdebody.hdedskio.hdeddev = edisopen;
	if (ioctl(hdelfdes, HDEWDISK, &edarg) < 0) {
		fprintf(stderr, "%s: write of defect map failed", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		perror("HDEWDISK edputdm");
		hdeerror(&edarg);
		return(-1);
	}
	return(0);
}

edputpd()
{
	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
	if (!edpdsec.valid) {
		fprintf(stderr, "%s: don't have valid copy of pdsec", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		return(-1);
	}
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdebcnt = edsecsz;
	edarg.hdebody.hdedskio.hdeuaddr = edpdsec.badr;
	edarg.hdebody.hdedskio.hdeddev = edisopen;
	if (ioctl(hdelfdes, HDEWRTPD, &edarg) < 0) {
		fprintf(stderr, "%s: write of pdsec failed", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		perror("HDEWRTPD");
		hdeerror(&edarg);
		return(-1);
	}
}

daddr_t
ctstodad(cyl, trk, sec)
{
	register int cylsz, trksz;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
	if (edpdck()) {
		return(-1);
	}
#ifdef	HDELOG
	if (cyl < 0 || cyl >= edpdp->dt_cyls) {
		fprintf(stderr, "%s: cyl=%d out of range 0 to %d\n",
				cmdname,
				cyl, edpdp->dt_cyls-1);
		return(-1);
	}
	trksz = edpdp->dt_sectors;
	cylsz = edpdp->dt_tracks;
#else	/* HDELOG */
	if (cyl < 0 || cyl >= edpdp->pdinfo.cyls) {
		fprintf(stderr, "%s: cyl=%d out of range 0 to %d\n",
				cmdname,
				cyl, edpdp->pdinfo.cyls-1);
		return(-1);
	}
	trksz = edpdp->pdinfo.sectors;
	cylsz = edpdp->pdinfo.tracks;
#endif	/* HDELOG */
	if (trk < 0 || trk >= cylsz) {
		fprintf(stderr, "%s: trk=%d out of range 0 to %d\n",
			cmdname,
			trk, cylsz-1);
		return(-1);
	}
	if (sec < 0 || sec >= trksz) {
		fprintf(stderr, "%s: sec=%d out of range 0 to %d\n",
			cmdname,
			sec, trksz-1);
		return(-1);
	}
	cylsz *= trksz;
	return(sec + trk*trksz + cyl*cylsz);
}

dadcheck(dad)
daddr_t dad;
{

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
	if (edpdck()) {
		return(-1);
	}
	if (dad < 0 || dad >= edenddad) {
		fprintf(stderr, "%s: disk address=%d out of range\n",
			cmdname, dad);
		return(-1);
	}
	return(0);
}

edgetel()
{
	register int sz;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
#ifdef	HDELOG
	if (((long)edpdp->dt_errlogst < (long)1) ||
		((long)edpdp->dt_errlogsz <= (long)0)) {
#ifdef	DEBUG
		fprintf(stderr, "%s: no hard error log area on disk\n",cmdname);
#endif	/* DEBUG */
		edhdel.valid = 0;
		return(-1);
	}
#endif	/* HDELOG */
	if (edhdel.valid) return(0);
	if (edpdck()) return(-1);
	sz = edallocel();
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdebcnt = sz;
	edarg.hdebody.hdedskio.hdeuaddr = edhdel.badr;
#ifdef	HDELOG
	edarg.hdebody.hdedskio.hdedaddr = edpdp->dt_errlogst;
#else	/* HDELOG */
	edarg.hdebody.hdedskio.hdedaddr = edpdp->pdinfo.errlogst;
#endif	/* HDELOG */
	edarg.hdebody.hdedskio.hdeddev = edisopen;
	if (ioctl(hdelfdes, HDERDISK, &edarg) < 0) {
		fprintf(stderr, "%s: read of error log failed", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		perror("HDERDISK edgetel");
		hdeerror(&edarg);
		return(-1);
	}
	edhdel.valid = 1;
	return(0);
}

edallocel()
{
	register int sz;

#ifdef	HDELOG
	sz = (edpdp->dt_errlogsz + edsecsz -1)/edsecsz;
#else	/* HDELOG */
	sz = (edpdp->pdinfo.errlogsz + edsecsz -1)/edsecsz;
#endif	/* HDELOG */
	sz = sz * edsecsz;
	doalloc(&edhdel, sz, "edhdel");
	return(sz);
}

edvalel(prt)
{
	register int i;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
#ifdef	HDELOG
	if (((long)edpdp->dt_errlogst < (long)1) ||
		((long)edpdp->dt_errlogsz <= (long)0)) {
#ifdef	DEBUG
		fprintf(stderr, "%s: no hard error log area on disk\n",cmdname);
#endif	/* DEBUG */
		edhdel.valid = 0;
		return(-1);
	}
#endif	/* HDELOG */
	if (edpdck()) return(-1);
	if (prt)
		printf("%s: initializing error log on disk maj=%d min=%d\n",
			cmdname, major(edisopen), minor(edisopen));
	i = edallocel();
	memset(edhdel.badr, 0, i);
	edhdelp->l_valid = HDEDLVAL;
	edhdelp->l_vers = HDEDLVER;
	edhdelp->createdt = time(0);
	edhdel.valid = 1;
	return(edputel());
}

edputel()
{
	register int sz;
	register int cylsz, trksz;
	register int i;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
	if (!edhdel.valid) {
		fprintf(stderr, "%s: edhdel not valid\n", cmdname);
		return(-1);
	}
	if(edpdck()) {
		return(-1);
	}
	if (edhdelp->l_valid != HDEDLVAL)
		fprintf(stderr, "%s: writing invalid error log on disk maj=%d min=%d\n",
			cmdname, major(edisopen), minor(edisopen));
	else edhdelp->lastchg = time(0);
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdebcnt = edhdel.csz;
	edarg.hdebody.hdedskio.hdeuaddr = edhdel.badr;
#ifdef	HDELOG
	edarg.hdebody.hdedskio.hdedaddr = edpdp->dt_errlogst;
#else	/* HDELOG */
	edarg.hdebody.hdedskio.hdedaddr = edpdp->pdinfo.errlogst;
#endif	/* HDELOG */
	edarg.hdebody.hdedskio.hdeddev = edisopen;
	if (ioctl(hdelfdes, HDEWDISK, &edarg) < 0) {
		fprintf(stderr, "%s: write of error log failed", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		perror("HDEWDISK edputel");
		hdeerror(&edarg);
		return(-1);
	}
	return(0);
}

edgetvt()
{
	register int sz;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
#ifdef	HDELOG
	if ((long)edpdp->dt_logicalst < (long)1) {
#ifdef	DEBUG
		fprintf(stderr, "%s: no vtoc info area on disk\n", cmdname);
#endif	/* DEBUG */
		edvtoc.valid = 0;
		return(-1);
	}
#endif	/* HDELOG */
	if (edvtoc.valid) return(0);
	if (edpdck()) return(-1);
	sz = edsecsz;
	if (sz < 512) sz = ((512 + edsecsz -1)/edsecsz) * edsecsz;
	doalloc(&edvtoc, sz, "edvtoc");
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdebcnt = sz;
	edarg.hdebody.hdedskio.hdeuaddr = edvtoc.badr;
#ifdef	HDELOG
	edarg.hdebody.hdedskio.hdedaddr = edpdp->dt_logicalst +(sz/edsecsz);
#else	/* HDELOG */
	edarg.hdebody.hdedskio.hdedaddr = edpdp->pdinfo.logicalst +(sz/edsecsz);
#endif	/* HDELOG */
	edarg.hdebody.hdedskio.hdeddev = edisopen;
	if (ioctl(hdelfdes, HDERDISK, &edarg) < 0) {
		fprintf(stderr, "%s: read of vtoc failed", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		perror("HDERDISK edgetvt");
		hdeerror(&edarg);
		return(-1);
	}
	edvtoc.valid = 1;
	return(0);
}

edputvt()
{
	register int sz;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
	if (!edvtoc.valid) {
		fprintf(stderr, "%s: edvtoc not valid\n", cmdname);
		return(-1);
	}
	if(edpdck()) return(-1);
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdebcnt = edvtoc.csz;
	edarg.hdebody.hdedskio.hdeuaddr = edvtoc.badr;
#ifdef	HDELOG
	edarg.hdebody.hdedskio.hdedaddr = edpdp->dt_logicalst +
		(edvtoc.csz/edsecsz);
#else	/* HDELOG */
	edarg.hdebody.hdedskio.hdedaddr = edpdp->pdinfo.logicalst +
		(edvtoc.csz/edsecsz);
#endif	/* HDELOG */
	edarg.hdebody.hdedskio.hdeddev = edisopen;
	if (ioctl(hdelfdes, HDEWDISK, &edarg) < 0) {
		fprintf(stderr, "%s: write of vtoc failed", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		perror("HDEWDISK edputvt");
		hdeerror(&edarg);
		return(-1);
	}
	return(0);
}

edread(blk, blkcnt, cadr)
char *cadr;
{
	register int sz;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
	sz = edsecsz * blkcnt;
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdebcnt = sz;
	edarg.hdebody.hdedskio.hdeuaddr = cadr;
	edarg.hdebody.hdedskio.hdedaddr = blk;
	edarg.hdebody.hdedskio.hdeddev = edisopen;
	if (ioctl(hdelfdes, HDERDISK, &edarg) < 0) {
		fprintf(stderr, "%s: read of blocks %d to %d failed",
			cmdname, blk, blk + blkcnt -1);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		perror("HDERDISK edread");
		hdeerror(&edarg);
		return(-1);
	}
	return(0);
}

edwrite(blk, blkcnt, cadr)
char *cadr;
{
	register int sz;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
	sz = edsecsz * blkcnt;
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdebcnt = sz;
	edarg.hdebody.hdedskio.hdeuaddr = cadr;
	edarg.hdebody.hdedskio.hdedaddr = blk;
	edarg.hdebody.hdedskio.hdeddev = edisopen;
	if (ioctl(hdelfdes, HDEWDISK, &edarg) < 0) {
		fprintf(stderr, "%s: write of blocks %d to %d failed",
			cmdname, blk, blk + blkcnt -1);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		perror("HDEWDISK edwrite");
		hdeerror(&edarg);
		return(-1);
	}
	return(0);
}

edmlogr(mlogrp, lcnt)
struct hdedata *mlogrp;
{

	if (hdelfdes < 0) {	/* need to do edinit */
		edinit();
	}
	edarg = edzarg;
	edarg.hdebody.hdeelogr.hdelrcnt = lcnt;
	edarg.hdebody.hdeelogr.hdeuladr = mlogrp;
	if (ioctl(hdelfdes, HDEMLOGR, &edarg) < 0) {
		fprintf(stderr, "%s: manual log request failed\n", cmdname);
		perror("HDEMLOGR");
		hdeerror(&edarg);
		exit(ERREXIT);
	}
	return(0);
}

edfixlk()
{

	if (hdelfdes < 0) {	/* need to do edinit */
		edinit();
	}
	edarg = edzarg;
	if (ioctl(hdelfdes, HDEFIXLK, &edarg) < 0) {
		fprintf(stderr, "%s: fix lock call failed\n", cmdname);
		if (edarg.hderval == HDE_TWOF)
			fprintf(stderr,
				"%s: cannot run two hdefix processes at same time.\n",
				cmdname);
		perror("HDEFIXLK");
		hdeerror(&edarg);
		exit (ERREXIT);
	}
	edisfix = 1;
	return(0);
}

edfixul()
{
	if (!edisfix) {
		fprintf(stderr, "%s: have not done a fixlk\n", cmdname);
		return(-1);
	}
	edarg = edzarg;
	if (ioctl(hdelfdes, HDEFIXUL, &edarg) < 0) {
		fprintf(stderr, "%s: fix unlock failed\n", cmdname);
		perror("HDEFIXUL");
		hdeerror(&edarg);
		return(-1);
	}
	edisfix = 0;
	return(0);
}

eddemonck(sp)
char *sp;
{
	if (!edisfix && getppid() != 1) {
		fprintf(stderr,
			"%s: must be hdefix or hdelogger demon to %s\n",
			cmdname,
			sp);
		return(-1);
	}
	return(0);
}

edgetrct()
{

	if (hdelfdes < 0) {	/* need to do edinit */
		edinit();
	}
	if (eddemonck("get report count")) return(-1);
	edarg = edzarg;
	errno = 2;
	if (ioctl(hdelfdes, HDEGERCT, &edarg) < 0) {
		if (errno == EINTR) return(-2);
		fprintf(stderr, "%s: attempt to get report count failed\n",
			cmdname);
		perror("HDEGERCT");
		hdeerror(&edarg);
		return(-1);
	}
	return(edarg.hdersize);
}

edgetrpts(rp, rcnt)
struct hdedata *rp;
{
	if (hdelfdes < 0) {	/* need to do edinit */
		edinit();
	}
	if (eddemonck("get reports")) return(-1);
	edarg = edzarg;
	edarg.hdebody.hdeelogr.hdelrcnt = rcnt;
	edarg.hdebody.hdeelogr.hdeuladr = rp;
	errno = 0;
	if (ioctl(hdelfdes, HDEGEREP, &edarg) < 0) {
		if (errno == EINTR) return(-2);
		fprintf(stderr, "%s: attempt to get reports failed\n",
			cmdname);
		perror("HDEGEREP");
		hdeerror(&edarg);
		return(-1);
	}
	return(edarg.hdersize);
}

edclrrpts(rp, rcnt)
struct hdedata *rp;
{
	if (hdelfdes < 0) {	/* need to do edinit */
		edinit();
	}
	if (eddemonck("clear reports")) return(-1);
	edarg = edzarg;
	edarg.hdebody.hdeelogr.hdelrcnt = rcnt;
	edarg.hdebody.hdeelogr.hdeuladr = rp;
	if (ioctl(hdelfdes, HDECEREP, &edarg) < 0) {
		fprintf(stderr, "%s: attempt to clear reports failed\n",
			cmdname);
		perror("HDECEREP");
		hdeerror(&edarg);
		return(-1);
	}
	if (edarg.hdersize != rcnt) {
		fprintf(stderr, "%s: cleared only %d reports out of %d\n",
			cmdname, edarg.hdersize, rcnt);
	}
	return(edarg.hdersize);
}

edrptslp()
{
	if (hdelfdes < 0) {	/* need to do edinit */
		edinit();
	}
	if (eddemonck("sleep for reports")) return(-1);
	edarg = edzarg;
	errno = 0;
	if (ioctl(hdelfdes, HDEERSLP, &edarg) < 0) {
		if (errno == EINTR) return(-2);
		fprintf(stderr, "%s: attempt to sleep for reports failed\n",
			cmdname);
		perror("HDEERSLP");
		hdeerror(&edarg);
		return(-1);
	}
	return(edarg.hdersize);
}

daddr_t
edgetpdsno(dev)
{
	register int i;

	for (i = 0; i < edcnt; i++)
		if (dev == edtable[i])
			return(ededdp[i].hdepdsno);
	return(-1);
}

doalloc(edp, sz, sp)
register struct eddata *edp;
char *sp;
{

	if (sz > edp->bsz) {
		if (edp->bsz) {
			if (!(edp->badr = realloc(edp->badr, sz))) {
				fprintf(stderr,
					"%s: realloc of %s failed",
					cmdname, sp);
				if (edisopen == -1) {
					fprintf(stderr, ": no disk open\n");
					exit(ERREXIT);
				}
				fprintf(stderr,
					": disk dev %d/%d\n",
					major(edisopen),
					minor(edisopen));
				exit(ERREXIT);
			}
		} else
		if (!(edp->badr = malloc(sz))) {
			fprintf(stderr,
				"%s: malloc of %s failed",
				cmdname, sp);
			if (edisopen == -1) {
				fprintf(stderr, ": no disk open\n");
				exit(ERREXIT);
			}
			fprintf(stderr,
				": disk dev %d/%d\n",
				major(edisopen),
				minor(edisopen));
			exit(ERREXIT);
		}
		edp->bsz = sz;
	}
	edp->csz = sz;
	return(0);
}

#ifdef	HDELOG
edgetmap()
{
register int sz;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
	if (badmap.valid)
		return(0);
	if (edpdck())
		return(-1);
	sz = edsecsz;
	if (sz < 512)
		sz = ((512 + edsecsz - 1)/edsecsz) * edsecsz;
	sz *= MAXBADSEC;
	doalloc(&badmap, sz, "badmap");
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdebcnt = sz;
	edarg.hdebody.hdedskio.hdeuaddr = badmap.badr;
	edarg.hdebody.hdedskio.hdedaddr = 1;
	edarg.hdebody.hdedskio.hdeddev = edisopen;
	if (ioctl(hdelfdes, HDERDISK, &edarg) < 0) {
		fprintf(stderr, "%s: read of badmap failed", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		perror("HDERDISK edgetmap");
		hdeerror(&edarg);
		return(-1);
	}
	badmap.valid = 1;
	return(0);
}

edputmap()
{
register int sz;

	if (edisopen == -1) {
		fprintf(stderr, "%s: no disk is open\n", cmdname);
		return(-1);
	}
	if (!badmap.valid) {
		fprintf(stderr, "%s: badmap not valid\n", cmdname);
		return(-1);
	}
	if (edpdck())
		return(-1);
	edarg = edzarg;
	edarg.hdebody.hdedskio.hdebcnt = badmap.csz;
	edarg.hdebody.hdedskio.hdeuaddr = badmap.badr;
	edarg.hdebody.hdedskio.hdedaddr = 1;
	edarg.hdebody.hdedskio.hdeddev = edisopen;
	if (ioctl(hdelfdes, HDEWDISK, &edarg) < 0) {
		fprintf(stderr, "%s: write of badmap failed", cmdname);
		fprintf(stderr, ": disk dev %d/%d\n",
			major(edisopen), minor(edisopen));
		perror("HDEWDISK edputmap");
		hdeerror(&edarg);
		return(-1);
	}
	return(0);
}
#endif	/* HDELOG */
