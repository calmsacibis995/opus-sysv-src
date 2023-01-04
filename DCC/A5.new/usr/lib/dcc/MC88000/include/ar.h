#ifndef __Iar
#define __Iar

struct ar_hdr {
	char	ar_name[16];	/* '/' term.	*/
	char	ar_date[12];	/* decimal	*/
	char	ar_uid[6];	/* decimal	*/
	char	ar_gid[6];	/* decimal	*/
	char	ar_mode[8];	/* octal	*/
	char	ar_size[10];	/* decimal	*/
	char	ar_fmag[2];	/* ARFMAG	*/
};

#define ARFMAG	"`\012"
#define ARMAG	"!<arch>\012"
#define SARMAG	8

#endif
