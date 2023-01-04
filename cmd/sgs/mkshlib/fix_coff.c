	/*
	 * Program to patch coff files for mkshlib
	 */

#include <stdio.h>
#include <filehdr.h>
#include <scnhdr.h>
#include <syms.h>

fix_coff(fname, fixtype)
char *fname;
char fixtype;
{

	struct  scnhdr  scnhdrbuf;
	struct  filehdr filehdrbuf;
	FILE            *coff_file;
	char            *oldname, *newname;
	long            newflags;
	int		i;

	/* 
	 *
	 *      -i : make the .text section into the .init section
	 *      -l : make the .data section into the .lib section
	 *
	 */


	switch( fixtype ) {
		case 'i':
			oldname = ".text";
			newname = ".init";
			newflags = STYP_TEXT;
			break;
		case 'l':
			oldname = ".data";
			newname = ".lib";
			newflags = STYP_LIB;
			break;
		default:
			fatal( "Bad flag: %c\n", fixtype);
	}

	/* open coff file for update */
	if ( (coff_file=fopen(fname,"r+"))==NULL)  {
		fatal( "Cannot open coff file: %s\n", fname);
	}

	/* read the file header to get number of sections */
	fread( &filehdrbuf, sizeof(filehdrbuf), 1, coff_file);

	/* search sections sequentially until the desired one is found */
	/* update the required data and write it back */
	for ( i=0; i<filehdrbuf.f_nscns; i++)  {
		fseek( coff_file, sizeof(filehdrbuf)+sizeof(scnhdrbuf)*i, 0);
		fread( &scnhdrbuf, sizeof(scnhdrbuf), 1, coff_file);
		if ( strcmp( scnhdrbuf.s_name, oldname) == 0) {
			strncpy( scnhdrbuf.s_name, newname, SYMNMLEN);
			scnhdrbuf.s_flags  = newflags;
			fseek( coff_file, sizeof(filehdrbuf)+sizeof(scnhdrbuf)*i, 0);
			fwrite( &scnhdrbuf, sizeof(scnhdrbuf), 1, coff_file);
			break;
		}
	}
	fclose(coff_file);
}
