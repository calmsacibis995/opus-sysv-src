#include <stdio.h>
#include <ctype.h>

#define LITTLENDIAN 0
#define BIGENDIAN 1

#if NS32000
#define ns32000 1
#undef clipper
#undef m88k
#endif

#if CLIPPER
#undef ns32000
#define clipper 1
#undef m88k
#endif

#if M88K
#undef ns32000
#undef clipper
#define m88k 1
#endif

#include "a.out.h"

FILHDR fhdr;
SCNHDR thdr,dhdr,bhdr;
AOUTHDR ahdr;
long strtol();
long getloc();
long lseek();
long xread();
long xwrite();
int swap_away = 0;
int debug = 0;

#define SWAP(x) \
	{ \
		if (sizeof(x) == 2) \
			swapw(&(x)); \
		else if (sizeof(x) == 4) \
			swapl(&(x)); \
	}

#define  ISNAME(x) \
		(((((x) & N_TMASK) == (DT_ARY << N_BTSHFT)) || \
		(((x) & N_TMASK) == (DT_PTR << N_BTSHFT)) || \
		(((x) & N_TMASK) == (DT_NON << N_BTSHFT))) && \
		((((x) & N_BTMASK) == T_STRUCT) || \
		(((x) & N_BTMASK) == T_UNION) || \
		(((x) & N_BTMASK) == T_ENUM))) \

main(argc,argv)

int argc;
char *argv[];
{
	long newpos, pos;
	long symptr;
	long nscns;
	long nsyms;
	short opthdr;
	unsigned char byte;
	int i,opcode,format;
	long offset,count;
	int ans;
	int fd;
	int host, not_host;
	char *arg0;
	char *swapfile;
	extern int optind;
	extern char *optarg;
	short magic;
	int targetfmt;
	int byteorder;
	int lflag = 0;
	int bflag = 0;
	if ( argc < 3 )
		usage(argv[0]);
	arg0 = argv[0];

/*
 *	'-l' puts the executable into littlendian byte order
 *	'-b' puts the executable into bigendian byte order
 *	'-n' puts the executable into National 32000 byte ordering
 *	'-m' puts the executable into Motorola 88000 byte ordering
 *	'-d' turns on debugging
 */
	while( (ans = getopt( argc, argv, "bdlmnw")) != EOF ) {
		switch( ans ) {

		case 'b':	/* bigendian byte order */
		case 'm':	/* motorola byte order */
			bflag = 1;
			targetfmt = BIGENDIAN;
			break;

		case 'l':	/* littlendian byte order */
		case 'n':	/* national byte order */
			lflag = 1;
			targetfmt = LITTLENDIAN;
			break;

		case 'd':	/* turn on debugging info */
			debug = 1;
			break;

		default:
			usage(argv[0]);
		}
	}
	/*
 	*	Change argv to point to the file.
 	*/
	argv = &argv[optind];
	argc -= optind;

	if (lflag && bflag) {
		printf("%s: cannot have both the [-b|-m] and [-l|-n] options\n", arg0);
		usage(arg0);
	}


	/*
	 *	Open /dev/unix to determine host type.
	 */
	fd = open("/dev/unix", 0);
	if (fd == -1) {
		fprintf(stderr,"%s: cannot open /dev/unix\n",arg0);
		exit(1);
	}

	/*
	 *	Determine host type.
	 */
	xread(fd, &fhdr, sizeof(fhdr), 0);
	magic = fhdr.f_magic;
	if (magic == NS32GMAGIC || magic == NS32SMAGIC || magic == CLIPPERMAGIC) {
		host = LITTLENDIAN;
		not_host = BIGENDIAN;
	} else if (magic == MC88MAGIC) {
		host = BIGENDIAN;
		not_host = LITTLENDIAN;
	} else {
		printf("magic is: 0x%x\n", magic);
    		fprintf(stderr,"%s: /dev/unix is not in proper format\n", arg0);
		exit(1);
	}
	close(fd);

	/*
	 *	Open the file to be processed.
	 */
	fd = open(argv[0], 2);
	if (fd == -1) {
		fprintf(stderr,"%s: cannot open %s\n",arg0, argv[0]);
		exit(1);
	}

	/*
	 *	Determine file type.
	 */
	xread(fd, &fhdr, sizeof(fhdr), 0);
	magic = fhdr.f_magic;
	if (magic == NS32GMAGIC || magic == NS32SMAGIC || magic == MC88MAGIC || magic == CLIPPERMAGIC) {
		byteorder = host;
	} else {
		SWAP(magic);
		if (magic == NS32GMAGIC || magic == NS32SMAGIC || magic == MC88MAGIC || magic == CLIPPERMAGIC)
			byteorder = not_host;
		else {
    			fprintf(stderr,"%s: %s is not a binary file\n", arg0, argv[0]);
		    	exit(0);
		}
		SWAP(magic);
	}

	/*
	 *	Exit if desired format is equal to the file type, else
	 *	set swap_away the file type is the same as the host type
	 */
	if (targetfmt == byteorder) {
    	fprintf(stderr,"%s: %s is already in the desired byte ordering.\n", arg0, argv[0]);
		exit(0);
	} else if (byteorder == host)
		swap_away++;

	if (swap_away) {
		symptr = fhdr.f_symptr;
		nscns = fhdr.f_nscns;
		nsyms = fhdr.f_nsyms;
		opthdr = fhdr.f_opthdr;
	}
	SWAP(fhdr.f_magic);
	SWAP(fhdr.f_nscns);
	SWAP(fhdr.f_timdat);
	SWAP(fhdr.f_symptr);
	SWAP(fhdr.f_nsyms);
	SWAP(fhdr.f_opthdr);
	if (swap_away) {
		fhdr.f_flags = fhdr.f_flags ^ (F_AR32WR | F_AR32W);
		SWAP(fhdr.f_flags);
	} else {
		SWAP(fhdr.f_flags);
		fhdr.f_flags = fhdr.f_flags ^ (F_AR32WR | F_AR32W);
	}
	if (!swap_away) {
		symptr = fhdr.f_symptr;
		nscns = fhdr.f_nscns;
		nsyms = fhdr.f_nsyms;
		opthdr = fhdr.f_opthdr;
	}
	newpos = xwrite(fd, &fhdr, sizeof(fhdr), 0);

	xread(fd, &ahdr, opthdr, newpos);
	SWAP(ahdr.magic);
	SWAP(ahdr.vstamp);
	SWAP(ahdr.tsize);
	SWAP(ahdr.dsize);
	SWAP(ahdr.bsize);
#if ns32000
	SWAP(ahdr.msize);
	SWAP(ahdr.mod_start);
#endif
	SWAP(ahdr.entry);
	SWAP(ahdr.text_start);
	SWAP(ahdr.data_start);
#if clipper
	SWAP(ahdr.cliflags);
	SWAP(ahdr.tcache);
	SWAP(ahdr.dcache);
	SWAP(ahdr.scache);
#endif
#if ns32000
	SWAP(ahdr.entry_mod);
	SWAP(ahdr.flags);
#endif
	newpos = xwrite(fd, &ahdr, opthdr, newpos);

	for (i=0; i<nscns; i++) {
		xread(fd, &thdr, sizeof(dhdr), newpos);
		swapsect(fd, &thdr, newpos);
		newpos = newpos + sizeof(dhdr);
	}

	swapsyms(fd, nsyms, symptr);
	close(fd);
}

swapsect(fd, secthdr, fptr)
long fptr;
SCNHDR *secthdr;
int fd;
{
	char *sstart;
	long newpos;
	long relptr;
	int nreloc;
	long svaddr;
	long scnptr;
	struct reloc reloc;
	long tmp;
	long type;
	long vaddr;
	long pos;
	int i;

	xread(fd, secthdr, sizeof(SCNHDR), fptr);
	if (swap_away) {
		scnptr = secthdr->s_scnptr;
		relptr = secthdr->s_relptr;
		nreloc = secthdr->s_nreloc;
		svaddr = secthdr->s_vaddr;
	}

	SWAP(secthdr->s_paddr);
	SWAP(secthdr->s_vaddr);
	SWAP(secthdr->s_size);
	SWAP(secthdr->s_scnptr);
	SWAP(secthdr->s_relptr);
	SWAP(secthdr->s_lnnoptr);
	SWAP(secthdr->s_nreloc);
	SWAP(secthdr->s_nlnno);
	SWAP(secthdr->s_flags);

	if (!swap_away) {
		scnptr = secthdr->s_scnptr;
		relptr = secthdr->s_relptr;
		nreloc = secthdr->s_nreloc;
		svaddr = secthdr->s_vaddr;
	}
	xwrite(fd, secthdr, sizeof(SCNHDR), fptr);
	
	if (!relptr)
		return;
	newpos = lseek(fd, relptr, 0);
	for (i=0; i<nreloc; i++) {
		newpos = lseek(fd, 0L, 1);
		xread(fd, &reloc, RELSZ, newpos);
		SWAP(reloc.r_vaddr);
		SWAP(reloc.r_symndx);
		SWAP(reloc.r_type);
#if m88k
		SWAP(reloc.r_offset);
#endif
		xwrite(fd, &reloc, sizeof(reloc), newpos);
	}
}

swapsyms(fd, nsyms, symptr)
long symptr;
long nsyms;
int fd;
{
	int i,j,k;
	short numaux;
	char sclass;
	short ntype;
	struct syment symbol;
	struct auxent auxent;
	long stringsz;
	int longnames = 0;

	if (!symptr)
		return;
	for (i=0; i<nsyms; i++) {
		xread(fd, &symbol, sizeof(symbol), symptr);
		numaux = symbol.n_numaux;
		SWAP(symbol.n_value); 
		SWAP(symbol.n_scnum);
		if (swap_away)
			ntype = symbol.n_type;
		SWAP(symbol.n_type);
		if (!swap_away)
			ntype = symbol.n_type;
		sclass=symbol.n_sclass;
		if (!symbol.n_name[0]) {
			SWAP(*(long *)&symbol.n_name[4]);
			longnames++;
		}
			
		xwrite(fd, &symbol, sizeof(symbol), symptr);
		symptr += sizeof(symbol);
		for (j=0; j<numaux; j++,i++) {
			xread(fd, &auxent, sizeof(auxent), symptr);
			if (sclass == C_EOS || ISARY(ntype) || ISFCN(ntype) || (ISNAME(ntype)))
				SWAP(auxent.x_sym.x_tagndx);
			if (sclass == C_BLOCK || sclass == C_FCN || ISARY(ntype))
				SWAP(auxent.x_sym.x_misc.x_lnsz.x_lnno);
			if (ISTAG(ntype) || sclass == C_EOS || ISARY(ntype) || ISNAME(ntype))
				SWAP(auxent.x_sym.x_misc.x_lnsz.x_size);
			if (ISFCN(ntype))
				SWAP(auxent.x_sym.x_misc.x_fsize);
			if (ISFCN(ntype))
				SWAP(auxent.x_sym.x_fcnary.x_fcn.x_lnnoptr);
			if (sclass == C_BLOCK || sclass == C_FCN || ISFCN(ntype) || ISTAG(ntype))
				SWAP(auxent.x_sym.x_fcnary.x_fcn.x_endndx);
			if (ISARY(ntype)) {
				for (k=0; k<DIMNUM; k++) {
					SWAP(auxent.x_sym.x_fcnary.x_ary.x_dimen[k]);
				}
			}
			if (ISFCN(ntype)) {
				SWAP(auxent.x_sym.x_fnend.x_fend.x_callseq);
				SWAP(auxent.x_sym.x_fnend.x_fend.x_level);
			}
			if (sclass == C_FILE)
				SWAP(auxent.x_file.x_foff);
			if (sclass == C_STAT) {
				SWAP(auxent.x_scn.x_scnlen);
				SWAP(auxent.x_scn.x_nreloc);
				SWAP(auxent.x_scn.x_nlinno);
				SWAP(auxent.x_scn.x_linoptr);
			}	
			xwrite(fd, &auxent, sizeof(auxent), symptr);
			symptr += sizeof(auxent);
		}
	}
	if (longnames) {
		if (xread(fd, &stringsz, sizeof(long), symptr)) {
			SWAP(stringsz);
			xwrite(fd, &stringsz, sizeof(long), symptr);
		}
	}
}

swapl(val)
register unsigned char *val;
{
	register swap;

	swap = val[0];
	val[0] = val[3];
	val[3] = swap;
	swap = val[1];
	val[1] = val[2];
	val[2] = swap;
}

swapw(val)
unsigned char *val;
{
	int swap;

	swap = val[0];
	val[0] = val[1];
	val[1] = swap;
}

long
xread(fd, addr, count, seekval)
int fd;
long addr;
long count;
long seekval;
{
	int retval;

	if (debug)
		printf("xread(%x,%x,%x,%x)\n",fd,addr,count,seekval);
	if (lseek(fd, seekval, 0) == -1) {
		perror("swapbin: xread lseek");
		exit(1);
	}
	if ((retval = read(fd, addr, count)) != count) {
		perror("swapbin: xread read");
		exit(1);
	}
	if (debug)
		printf("\tread %x\n",retval);
	return(seekval + count);
}

long
xwrite(fd, addr, count, seekval)
int fd;
long addr;
long count;
long seekval;
{
	if (lseek(fd, seekval, 0) == -1) {
		perror("swapbin: xwrite lseek");
		exit(1);
	}
	if (write(fd, addr, count) != count) {
		perror("swapbin: xwrite write");
		exit(1);
	}
	return(seekval + count);
}

usage(arg0)
char *arg0;
{
	printf("usage: %s -l|b <filename>\n", arg0);
#if clipper
	printf("  a.out format: clipper\n");
#elif ns32000
	printf("  a.out format: 32000\n");
#elif m88k
	printf("  a.out format: 88000\n");
#endif
	exit(1);
}
