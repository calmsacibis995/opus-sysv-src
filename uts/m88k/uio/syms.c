#include "sys/param.h"
#include "sys/types.h"
#include "sys/psw.h"
#include "sys/sysmacros.h"
#include "sys/immu.h"
#include "sys/systm.h"
#include "sys/pfdat.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/map.h"
#include "sys/buf.h"
#include "sys/reg.h"
#include "sys/utsname.h"
#include "sys/tty.h"
#include "sys/var.h"
#include "sys/debug.h"
#include "sys/cmn_err.h"
#include "sys/tuneable.h"
#include "sys/inode.h"
#include "sys/file.h"
#include "sys/fs/s5inode.h"
#include "sys/iorb.h"
#include "sys/commpage.h"
#include "sys/cp.h"
#include "sys/mount.h"
#include "sys/nserve.h"
#include "sys/sema.h"
#include "sys/comm.h"
#include "sys/adv.h"
#include "sys/cirmgr.h"
#include "sys/stream.h"
#include "sys/callo.h"
#include "sys/sysinfo.h"
#include "sys/fcntl.h"
#include "sys/flock.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include "sys/shm.h"
#include "sys/sem.h"

extern int nadvertise, nsrmount;
extern char strlofrac, strmedfrac;
extern rfs_vlow, rfs_vhigh, rfsize;
extern short dufstyp;
extern struct shmid_ds	shmem[];	/* shared memory headers */
extern struct shminfo	shminfo;	/* shared memory info structure */
extern struct semid_ds	sema[];		/* semaphore data structures */
extern struct seminfo seminfo;		/* param information structure */
extern struct msqid_ds	msgque[];	/* msg queue headers */
extern struct msginfo	msginfo;	/* message parameters */
extern rcstat, clstat, rsstat, svstat;	/* nfs */
extern	dkstat, dk_info, dktab;		/* disk driver */
extern	ethdix, ex_inited;		/* exos stuff */
extern int arptab_size, rthashsize;	/* bsd stuff */
extern arptab[], rthost[], rtnet[];	/* more bsd stuff */
extern nfile, physmem, mbstat, ipstat, tcpstat;
extern tcb, udb, udpstat, rawcb, ifnet, icmpstat;
extern rtstat, unixsw[], ksptbl;
extern divcnt, aligncnt, fpuicnt, fpupcnt;
extern	fltin[];
extern con_tty[];
extern  freemem;
extern ec_softc[];
extern con_info[], as_info[], PT_info[];
extern char msgbuf[];

struct ksym {
	char 	*name;
	long 	val;
} ksyms[] = {
	"_v", (long) &v,
	"_proc", (long) &proc,
	"_swplo", (long) &swplo,
	"_region", (long) &region,
	"_inode", (long) &inode,
	"_pfdat", (long) &pfdat,
	"_callout", (long) callout,
	"_u", (long) &u,
	"_tune", (long) &tune,
	"_sysinfo", (long) &sysinfo,
	"_time", (long) &time,
	"_sema", (long) sema,
	"_msginfo", (long) &msginfo,
	"_shminfo", (long) &shminfo,
	"_seminfo", (long) &seminfo,
	"_msgque", (long) msgque, 
	"_shmem", (long) shmem,
	"_utsname", (long) &utsname,
	"_minfo", (long) &minfo,
	"_syserr", (long) &syserr,
	"_freemem", (long) &freemem,
	"_selwait", (long) &selwait,
	"_kdebug", (long) &kdebug,
	"_con_tty", (long) con_tty,
	"_msgbuf", (long) msgbuf,
#if !WS
	"_file", (long) &file,
	"_mount", (long) mount,
	"_bufz", (long) &bufz,
	"_flckinfo", (long) &flckinfo,
#if m88k
	"_divcnt", (long) &divcnt,
	"_aligncnt", (long) &aligncnt,
	"_fpuicnt", (long) &fpuicnt,
	"_fpupcnt", (long) &fpupcnt,
	"_fltin", (long) fltin,
	"_nfile", (long) &nfile,
	"_physmem", (long) &physmem,
	"_mbstat", (long) &mbstat,
	"_ipstat", (long) &ipstat,
	"_tcb", (long) &tcb,
	"_tcpstat", (long) &tcpstat,
	"_udb", (long) &udb,
	"_udpstat", (long) &udpstat,
	"_rawcb", (long) &rawcb,
	"_ifnet", (long) &ifnet,
	"_icmpstat", (long) &icmpstat,
	"_rtstat", (long) &rtstat,
	"_unixsw", (long) unixsw,
	"_sstbl", (long) &sstbl,
	"_kptbl", (long) &kptbl,
	"_ksptbl", (long) &ksptbl,
	"_con_info", (long) con_info,
	"_as_info", (long) as_info,
	"_PT_info", (long) PT_info,
#else
	"_gdp", (long) gdp,
	"_maxgdp", (long) &maxgdp,
	"_queue", (long) queue,
	"_rcvd", (long) rcvd,
	"_nrcvd", (long) &nrcvd,
	"_sndd", (long) sndd,
	"_nsndd", (long) &nsndd,
	"_srmount", (long) srmount,
	"_nsrmount", (long) &nsrmount,
	"_stream", (long) streams,
	"_advertise", (long) advertise,
	"_nadvertise", (long) &nadvertise,
	"_dufstyp", (long) &dufstyp,
	"_nmuxlink", (long) &nmuxlink,
	"_nstrpush", (long) &nstrpush,
	"_nstrevent", (long) &nstrevent,
	"_maxsepgcnt", (long) &maxsepgcnt,
	"_strlofrac", (long) &strlofrac,
	"_strmedfrac", (long) &strmedfrac,
	"_strmsgsz", (long) &strmsgsz,
	"_strctlsz", (long) &strctlsz,
	"_nrduser", (long) &nrduser,
	"_minserve", (long) &minserve,
	"_maxserve", (long) &maxserve,
	"_rfsize", (long) &rfsize,
	"_rfs_vhigh", (long) &rfs_vhigh,
	"_rfs_vlow", (long) &rfs_vlow,
	"_ethdix", (long) &ethdix,
	"_ex_inited", (long) &ex_inited,
	"_dinfo", (long) &dinfo,
	"_mblock", (long) mblock,
	"_dblock", (long) dblock,
#endif /* m88k */
	"_rcstat", (long) &rcstat,
	"_clstat", (long) &clstat,
	"_rsstat", (long) &rsstat,
	"_svstat", (long) &svstat,
	"_pbufz", (long) &pbufz,
	"_dkstat", (long) &dkstat,
	"_dk_info", (long) &dk_info,
	"_dktab", (long) &dktab,
	"_arptab", (long) arptab,
	"_arptab_size", (long) &arptab_size,
	"_rthost", (long) rthost,
	"_rtnet", (long) rtnet,
	"_rthashsize", (long) &rthashsize,
	"_ec_softc", (long) ec_softc,
#endif	/* !WS */
	0,	0
};

#define SYMNMLEN	8
#define SYMBUFSZ	(((sizeof ksyms/sizeof (struct ksym)) * SYMESZ) + 256)
#if clipper && !m88k
#define	SYMESZ		18
#define	MAGIC		0577
#endif
#if ns32000
#define SYMESZ		20
#define	MAGIC		0524
#endif
#if m88k
#define SYMESZ		20
#define	MAGIC		0555
#endif
#define	C_EXT		2
#define F_EXEC		2

struct filehdr {
	unsigned short	f_magic;	/* magic number */
	unsigned short	f_nscns;	/* number of sections */
	long		f_timdat;	/* time & date stamp */
	long		f_symptr;	/* file pointer to symtab */
	long		f_nsyms;	/* number of symtab entries */
	unsigned short	f_opthdr;	/* sizeof(optional hdr) */
	unsigned short	f_flags;	/* flags */
} *hdr;

struct syment
{
	union
	{
		char		_n_name[SYMNMLEN];	/* old COFF version */
		struct
		{
			long	_n_zeroes;	/* new == 0 */
			long	_n_offset;	/* offset into string table */
		} _n_n;
		char		*_n_nptr[2];	/* allows for overlaying */
	} _n;
	unsigned long		n_value;	/* value of symbol */
	short			n_scnum;	/* section number */
	unsigned short		n_type;		/* type and derived type */
	char			n_sclass;	/* storage class */
	char			n_numaux;	/* number of aux. entries */
#if ns32000
	char			n_env;		/* symbol interp environ */
	char			n_dummy;	/* currently unused */
#endif
};

#define n_name		_n._n_name
#define n_nptr		_n._n_nptr[1]
#define n_zeroes	_n._n_n._n_zeroes
#define n_offset	_n._n_n._n_offset

struct scnhdr {
	char		s_name[8];	/* section name */
	long		s_paddr;	/* physical address */
	long		s_vaddr;	/* virtual address */
	long		s_size;		/* section size */
	long		s_scnptr;	/* file ptr to raw data for section */
	long		s_relptr;	/* file ptr to relocation */
	long		s_lnnoptr;	/* file ptr to line numbers */
#if m88k
	long		s_nreloc;	/* number of relocation entries */
	long		s_nlnno;	/* number of line number entries */
#else
	unsigned short	s_nreloc;	/* number of relocation entries */
	unsigned short	s_nlnno;	/* number of line number entries */
#endif
	long		s_flags;	/* flags */
#if ns32000
	long		s_symptr;	/* file pointer to symbol table */
					/*   entries */
	unsigned short	s_modno;	/* module number of module associated */
					/*   with this section */
	unsigned short	s_pad;		/* padding to 4 byte multiple */
#endif
	};

unsigned char symbuf[((sizeof ksyms/sizeof (struct ksym)) * SYMESZ) + 512 ];
struct scnhdr *scns;
struct syment *symstart;

symread()
{
	register n;

	n = u.u_count;

	if ( u.u_offset >= SYMBUFSZ  || hdr == NULL || symstart == NULL )
		u.u_error = EIO;
	else if (copyout( &symbuf[u.u_offset], u.u_base, n ))
		u.u_error = ENXIO;
	else {
		u.u_offset += n;
		u.u_base += n;
		u.u_count -= n;
	}
}

syminit() {
	register struct syment *dst;
	register struct ksym *src;
	register unsigned char *p;
	int nsyms, len;
	struct syment localsym;
	unsigned char *strtab;

	hdr = (struct filehdr *) symbuf;
	scns = (struct scnhdr *) &hdr[1];
	symstart = (struct syment *) &scns[3];
	
	nsyms = sizeof(ksyms) / sizeof(struct ksym);
	nsyms--;
	strtab = (unsigned char *) symstart;
	strtab += SYMESZ*nsyms;

	p = strtab + 4;
	dst = symstart;
	src = ksyms;

	while ( src->name ) {
		if ( (len = strlen(src->name)) <= 8 )
			strncpy(localsym.n_name, src->name, 8);
		else {
			strcpy(p, src->name);
			localsym.n_zeroes = 0;
			localsym.n_offset = p - strtab;
			p += len + 2;
			ASSERT(p < &symbuf[SYMBUFSZ]);
		}
		localsym.n_value = src->val;
		localsym.n_sclass = C_EXT;

		localsym.n_numaux = 0;
		localsym.n_type = 0;
#if m88k
		localsym.n_scnum = 1;
#else
		localsym.n_scnum = 0;
#endif
		bcopy(&localsym, dst, SYMESZ);

		src++;
		dst = (struct syment *) ((char *) dst + SYMESZ);
	}
	ASSERT((unsigned char *) dst == strtab);
	
	hdr->f_magic = MAGIC;
	hdr->f_symptr = (unsigned char *) symstart - symbuf;
	hdr->f_nsyms = nsyms;
	hdr->f_nscns = 3;
	hdr->f_flags = F_EXEC;
	len = p - strtab;
	bcopy( &len, strtab, 4 );

	kstrcpy( scns[0].s_name , ".text" );
	scns[0].s_flags = 1;
	kstrcpy( scns[1].s_name , ".data" );
	scns[1].s_flags = 1;
	kstrcpy( scns[2].s_name , ".bss" );
	scns[2].s_flags = 1;
}
