# ifndef NOSCCS
static char *sccsid = "@(#)cmdfile.c	1.7	4/25/86";
static char *cpyrid = "@(#)Copyright (C) 1985 by National Semiconductor Corp.";
# endif NOSCCS

/*    char ** cmdfile(arg0,envname,def_suffix_list,malloc_ptr)

   Find the absolute unix pathnames of some files, using either an environment
   variable or the location of the currently executing program.  A pointer to 
   a zero-terminated array of string pointers (ala argv[]) is returned.  Each 
   string contains a single complete pathname.

   A user-specified memory allocator is called; "malloc" may be specified
   unless the program calls brk()/sbrk() in the VMS environment (see below).

   1.  If envname is the name of a defined environment variable, then
       the strings in the variable are returned.  Multiple strings in
       the environment variable may separated by ':' in unix and commas
       in VMS, e.g. "/a/b:/c/d" or "dev:[dir1],dev:[dir2]".
       The strings are returned separately in the result array, transformed
       to Unix format if needed.

   2.  If envname is 0 or an undefined name, then result strings are formed by
       concatenating the directory containing the currently executing program
       to the string(s) in def_suffix_list.  This is the way default paths are 
       formed.  Def_suffix_list always contains ':'-separated substrings
       using Unix path notation.  Note that it may mention directories
       such as "../lib".

       The directory containing the currently executing program is inferred
       from arg0, which should be argv[0] as passed to main().  This is
       usually the name of the current program when invoked by the user.  
       If arg0 contains a directory reference, then the final filename 
       component is removed and the remaining part concatenated with
       the suffixes in def_suffix_list.

       Otherwise, the PATH environment variable is used to search for a 
       directory containing an executable file of the name <arg0>, in the 
       manner of execvp(3). 

       If def_suffix_list contains null string(s), then the corresponding
       result(s) are just the directory part with a trailing slash (/).
    
   PARENT DIRECTORY REFERENCES REMOVED:
       "foo/../" in any final result is collapsed out.  This avoids 
       unnecessary directory traversals when the string is used to open a 
       file (but is usually incorrect if foo is a symbolic link!).
       If there are several ".." references in the same string, only the
       first is removed.
       
   MEMORY ALLOCATOR and VMS:
       The malloc_ptr argument must be the address of a routine which
       allocates memory.  It takes a single integer argument, the number
       of bytes to allocated, and returns a char pointer to the allocated 
       storage.  This may simply be "malloc" in Unix and VMS programs which 
       do not call brk() or sbrk().  However, the VMS C library does not allow 
       malloc() and sbrk() to be called in the same program, and so programs 
       using brk()/sbrk() must provide an alternative allocator.  Such 
       programs usually already contain such a routine, but if not, the 
       following may be used.  It allocates space from a statically declared
       array:

	#define ARYSZ 200  /* Size of fixed static storage area 
	static char mem[ARYSZ];
	static char *next_mem = mem;
	
	static char * static_malloc(n)
	{
		char *result;
		while ((int)next_mem & 3) next_mem++;  /* 4-byte alignment 
		if ((next_mem+n) >= (mem+ARYSZ)) return(0); 
		result = next_mem;
		next_mem += n;
		return(result);
	}

   VMS: In VMS arg0 will be the fully-qualified file name of the program,
	e.g. "device:[dir1.dir2]foo.exe".  THIS IS TRANSFORMED INTO THE
	EQUIVALENT UNIX-STYLE PATH (e.g. "/device/dir1/dir2/").
	Since the VMS VAX/C run-time library accepts unix path names (including 
	the execve service), the calling program can always process 
	unix-style file names, and work properly in either environment.

    EXAMPLE:
	The linker, ld, is typically installed as /bin/ld and searches for
	libraries in /lib and /usr/lib.  However, it may be installed in
	a private directory structure, and should search for libraries
	relative to that structure.  In addition, if the environment variable
	LIBDIR is defined, its value(s) override all defaults.

	ld would contain the following call:
		char *libdirlist[];
		...	
      		libdirlist = cmdfile(argv[0],"LIBDIR","../lib:../usr/lib");

        To search for library "libc.a" (for example), ld would contain 
	code similar to this:
		char **dirname;
		char path[MAX];
		...
		for (dirname=&libdirlist[0]; *dirname != 0; dirname++) {
			strcpy(path,*dirname);
			strcat(path,"libc.a"); 
			...try using path to open directory...
		}
	
	In a default resident installation, libdirlist would receive the
	strings "/bin/../lib" and "/bin/../usr/lib".  If ld were installed
	as a private version in /PVT/bin, then libdirlist would receive
	the strings "/PVT/bin/../lib" and "/PVT/bin/../usr/lib".  
	Finally, if the LIBDIR environment variable were defined to be
	"/lib:/usr/lib", then the default resident libraries would be
	used regardless of the location of the ld program.
 */

/* #define DEBUG 1 */

#if VMS
#   ifndef HOSTVMS
#      define HOSTVMS 1
#   endif
#endif

#if HOSTVMS || HOSTSYS5
#  define index strchr
#  define rindex strrchr
#  define strcmpn strncmp 
#endif

char *getenv();
char *index();
char *rindex();

#if HOSTVMS
#  include <stsdef.h>
#  define ERROR_EXITVAL STS$K_ERROR
#  define ENV_DELIM ','
#  define X_OK 1  /* access() mode bit */
#else
   /* Unix host */
#  define ERROR_EXITVAL 1
#  define ENV_DELIM ':'
#  if HOST_5_2
#     include <sys/types.h>
#     define X_OK 1  /* access() mode bit */
#  endif
#  include <sys/file.h>
#endif

#include <stdio.h>

static panic(s) char *s; 
   { fprintf(stderr,"PANIC in cmdfile: %s\n",s); exit(ERROR_EXITVAL); }

#define MAXCMDLEN 128

#if HOSTVMS
/* Convert a VMS path name to Unix convention in place.  If only a directory
   is specified (ends in ']'), then the result ends in '/'.  Otherwise
   the final file-name part is appended.
   
   The string may shrink, but never grows.  
 */
static void
vms2unix(path)
	char *path;
{
	char vmspath[MAXCMDLEN];
	register char *delim, *rbracket, *cp;
#if DEBUG
	int len = strlen(path);
	printf("### vms2unix(\"%s\")\n",path);
#endif
	/* "dev:[d1.d2]file.ext" -> "/dev/d1/d2/file.ext"   (unchanged len)*/
	strcpy(vmspath,path);
	delim = index(vmspath,':');
	rbracket = index(vmspath,']');
	if ((delim==0) && (rbracket==0)) return; /* No directories to xlate */
        if ((delim==0) || (rbracket==0) || (delim[1] != '['))
	       panic("bad VMS pathname");
        strcpy(path,"/");
        strncat(path,vmspath,delim-vmspath);  /* copy "device" */
        strcat(path,"/");
	delim++; /* now points at '[' */

	/* Collect all of the dot-separated directory names */
	do {
	   cp=index(delim+1,'.'); 
	   if ((cp==0) || (cp>rbracket)) cp=rbracket;
	   strncat(path,delim+1,cp-delim-1);  
	   strcat(path,"/");
	   delim = cp;
	} while (*delim != ']');
	strcat(path,delim+1);  /* copy file name part (if present) */
#ifdef DEBUG
	if (strlen(path) != len) panic("vms2unix -- bad length");
#endif
} /* vms2unix */	
#endif /* HOSTVMS */

/* get_cmddir determines the directory containing the current command.
   The result is always in Unix format (even if in VMS), with a 
   trailing slash.   The result is stored in the caller-supplied array which 
   must be at least MAXCMDLEN bytes long.

   Information about PATH is cached, so PATH must not change between calls.
*/

int cached_path_index = -1;  /* Index in PATH of known cmddir */

static void
get_cmddir(arg0, result)
	register char *arg0, *result;
{
	int len;
	register char *cp;
	int arg0len = strlen(arg0); 
#if HOSTVMS
	if (cp = index(arg0,':')) { /* VMS path, e.g. "device:[d1.d2]f.exe" */
		if (strlen(arg0) >= MAXCMDLEN) panic("vms arg0 too long");
		strcpy(result,arg0);
		vms2unix(result);
		rindex(result,'/')[1] = '\0';  /* lop off after directory */
		return;
	}
#else
	if (cp = rindex(arg0,'/')) { /* Explicit unix directory given */
		len = cp+1-arg0; 
		if (len >= MAXCMDLEN) panic("arg0 too long");
		strncpy(result,arg0,len);
		result[len] = '\0';
		return;
	} else 
	{ 
		register char *path;
			 char *whole_path;
		path = getenv("PATH"); if (path==NULL) path = ":/bin:/usr/bin";
		whole_path = path;
		if (cached_path_index > 0) path += cached_path_index;
		do {
			register char *cp;
			char delim;
			for (cp=path; (*cp!=':') && (*cp!=0); cp++) ;
			len = cp-path;
			if ((len==1 && *path=='.')||(len==2 && 
                           strcmp(path,"./",3)==0)) {
#ifdef HOSTSYS5
				getcwd(result,1024);
#else
				getwd(result);
#endif
				len = strlen(result);
			} else {
				strncpy(result,path,len);
			}
			if (arg0len+len+2>=MAXCMDLEN)panic("dir path too long");
			result[len] = '/';          
			strcpy(result+len+1,arg0); 
			if ((cached_path_index>=0) || 0==access(result,X_OK)) { 
				/* SUCCESS */
				result[len+1]='\0'; /* return dir part only */
#ifdef DEBUG
				if (strlen(result) > arg0len+len+1) 
					panic("pth ovf");
#endif
				cached_path_index = path-whole_path;
				return;
			}
			path = cp+1;  /* Advance to next field (if any) */
		} while (path[-1]==':');
	}
#endif
	panic("Unable to find command directory");
} /* get_cmddir */

/* Concatenate subfield n (first=0) from list to dest */
static void
subfield_cat(dest,list,list_delim,n)
	char *dest;
	register char *list;
	char list_delim;
	int n;
{
	while (n>0) {
		list = index(list,list_delim);
		if (list==0) panic("subfield_cat bug");
		list++;
		n--;
	}	
	dest = index(dest,'\0');
	while ( (*list) && (*list != list_delim) ) *dest++ = *list++;
	*dest = '\0';
} /* subfield_cat */

/* Return the number of subfields in a string.  A null string is 
   considered to have one (null) subfield */
static int
num_subfields(s,list_delim) 
	char *s;
	char list_delim;
{
	register n;
	n = 1;
	while (s=index(s,list_delim)) { n++; s++; }
	return(n);	
} /* num_subfields */

/* Construct array of pointers to isolated subfield strings, prefixing
   a constant prefix to each.  If in VMS environment and list_delim
   is ENV_DELIM, then convert the results to unix pathname syntax.
   This is the case of environment variable override.  Otherwise
   list_delim is ':', and list comes from def_prefix_list, which
   always has the unix syntax.

   Prefix has already been converted to unix syntax if appropriate.
   
   The first substring of the form "foo/../" is squished out if present.
   N.B. THIS IS INCORRECT if foo is a symbolic link.

   This is the guts of cmdfile().
*/
static char ** 
form_cmdfile_strings(prefix,list,list_delim,malloc_ptr)
	char *prefix, *list;
	char list_delim;
	char * (*malloc_ptr)();
{
	register char *space;
	register char *sq1,*sq2;
	char **result;
	int  aggregate_len;
	int  n = num_subfields(list,list_delim);
	aggregate_len = ((n+1) * sizeof(char *)) +
			((n*strlen(prefix)) + strlen(list) + n);
	result = (char **) (*malloc_ptr)( aggregate_len );
	if (result==0) panic("Memory allocation failed.");
	space = (char *) &result[n+1];

	result[n] = 0;
	while (--n >= 0) {
		result[n] = space;
		strcpy(space,prefix);
		subfield_cat(space,list,list_delim,n);
#if HOSTVMS
		if (list_delim==ENV_DELIM) vms2unix(space);
#endif
		/* Squish out first "/<anything but slash>/.." */
		sq2 = space;
		while ((sq2=index(sq2,'.')) && 
		       ((sq2 == space) || (0!=strcmpn(sq2-1,"/../",4)))) sq2++;
		if (sq2) {
			/* sq2 --> "../";  Find preceeding '/' */
		   	for (sq1 = sq2-2; sq1>=space && *sq1 != '/'; --sq1) ;
			if (*sq1 == '/') {
				sq2 += 2;
				while (*sq1++ = *sq2++) ;  /* squish! */
			}
		}
		space += (1+strlen(space));
	}	
#ifdef DEBUG
	if ((space - (char *)result) > aggregate_len) panic("fcs ovf");
#endif
	return(result);
} /* form_cmdfile_strings */

char **
cmdfile(arg0,envname,def_suffix_list,malloc_ptr)
	char *arg0, *envname, *def_suffix_list;
	char * (*malloc_ptr)();
{
	char *cp;
	/* Ignore environment variable override if zero pointer or empty
	   string.  The empty string check is necessary because we otherwise
	   assume a single null field from an empty string (see cat_subfld).
	*/
	if (envname!=0) cp = getenv(envname); else cp=0;
	if (cp && (*cp != '\0'))
		return( form_cmdfile_strings(
				"",
				cp,
				ENV_DELIM,
				malloc_ptr) );
	else {
		char tmp_cmddir[MAXCMDLEN];
		get_cmddir(arg0,tmp_cmddir);
		return( form_cmdfile_strings(
				tmp_cmddir,
				def_suffix_list,
				':', /* ':' always in def_suffix_list */
				malloc_ptr) );
	}
} /* cmdfile */
