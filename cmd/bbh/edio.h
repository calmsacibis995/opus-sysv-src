/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	Copyright (c) 1986 by National Semiconductor Corp.	*/
/*	All Rights Reserved					*/

#ident	"@(#)edio.h	1.4	10/30/86 Copyright (c) 1986 by National Semiconductor Corp."

struct eddata {
	char	*badr;
	int	bsz;
	int	csz;
	int	valid;
};

extern int edcnt;
extern dev_t *edtable;
extern struct hdeedd *ededdp;

extern struct eddata edpdsec;
extern struct eddata eddmap;		/* machine form of defect map */
extern struct eddata eddm;		/* standard form of defect map */
extern struct eddata edhdel;
extern struct eddata edvtoc;
extern long edsecsz;
extern daddr_t edenddad;
extern daddr_t edpdsno;

struct dskaddr	{
	short	cylno;
	char	trkno;
	char	secno;
};

union dskaval {
	struct	dskaddr cts;
	long	lval;
};

struct hddmap {
	union dskaval frmadr;
	union dskaval toadr;
};

struct hddm {
	daddr_t	frmblk;
	daddr_t	toblk;
};

#ifdef	HDELOG
#define edpdp	((struct disktab *)edpdsec.badr)
#define edvtocp	((struct disktab *)edpdsec.badr)
#define eddmcnt	(edpdp->dt_defectsz/sizeof(struct hddmap))
#define eddmapp	((struct hddmap *)eddmap.badr)
#define eddmp	((struct hddm *)eddm.badr)
#else	/* HDELOG */
#define edpdp	((struct pdsector *)edpdsec.badr)
#define edvtocp	((struct vtoc *)edvtoc.badr)
#define eddmcnt	(edpdp->pdinfo.defectsz/sizeof(struct hddmap))
#define eddmapp	((struct hddmap *)eddmap.badr)
#define eddmp	((struct hddm *)eddm.badr)
#endif	/* HDELOG */
#define edhdelp	((struct hdedl *)edhdel.badr)
