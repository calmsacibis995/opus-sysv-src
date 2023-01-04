/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/errlst.c	1.13.1.1"
/*LINTLIBRARY*/
char	*sys_errlist[] = {
/* 00 */	"Error 0",
/* 01 */	"Not owner",
/* 02 */	"No such file or directory",
/* 03 */	"No such process",
/* 04 */	"Interrupted system call",
/* 05 */	"I/O error",
/* 06 */	"No such device or address",
/* 07 */	"Arg list too long",
/* 08 */	"Exec format error",
/* 09 */	"Bad file number",
/* 10 */	"No child processes",
/* 11 */	"No more processes",
/* 12 */	"Not enough space",
/* 13 */	"Permission denied",
/* 14 */	"Bad address",
/* 15 */	"Block device required",
/* 16 */	"Device busy",
/* 17 */	"File exists",
/* 18 */	"Cross-device link",
/* 19 */	"No such device",
/* 20 */	"Not a directory",
/* 21 */	"Is a directory",
/* 22 */	"Invalid argument",
/* 23 */	"File table overflow",
/* 24 */	"Too many open files",
/* 25 */	"Not a typewriter",
/* 26 */	"Text file busy",
/* 27 */	"File too large",
/* 28 */	"No space left on device",
/* 29 */	"Illegal seek",
/* 30 */	"Read-only file system",
/* 31 */	"Too many links",
/* 32 */	"Broken pipe",
/* 33 */	"Argument out of domain",
/* 34 */	"Result too large",
/* 35 */	"No message of desired type",
/* 36 */	"Identifier removed",
/* 37 */	"Channel number out of range",
/* 38 */	"Level 2 not synchronized",
/* 39 */	"Level 3 halted",
/* 40 */	"Level 3 reset",
/* 41 */	"Link number out of range",
/* 42 */	"Protocol driver not attached",
/* 43 */	"No CSI structure available",
/* 44 */	"Level 2 halted",
/* 45 */	"Deadlock situation detected/avoided",
/* 46 */	"No record locks available",
/* 47 */	"Error 47",
/* 48 */	"Error 48",
/* 49 */	"Error 49",
/* 50 */	"Bad exchange descriptor",		
/* 51 */	"Bad request descriptor",	
/* 52 */	"Message tables full",	
/* 53 */	"Anode table overflow",
/* 54 */	"Bad request code",
/* 55 */	"Invalid slot",	
/* 56 */	"File locking deadlock",
/* 57 */	"Bad font file format",
/* 58 */	"Error 58",
/* 59 */	"Error 59",
/* 60 */	"Not a stream device",	
/* 61 */	"No data available",
/* 62 */	"Timer expired",
/* 63 */	"Out of stream resources",
/* 64 */	"Machine is not on the network",
/* 65 */	"Package not installed",
/* 66 */	"Object is remote",
/* 67 */	"Link has been severed",
/* 68 */	"Advertise error",
/* 69 */	"Srmount error",
/* 70 */	"Communication error on send",	
/* 71 */	"Protocol error",
/* 72 */	"Error 72",
/* 73 */	"Error 73",
/* 74 */	"Multihop attempted",
/* 75 */	"Error 75",
/* 76 */	"Cross mount point",
/* 77 */	"Not a data message",		
/* 78 */ 	 "Name is too long",		    /* ENAMETOOLONG */
/* 79 */	"Error 79",
/* 80 */	"Name not unique on network",
/* 81 */	"File descriptor in bad state",
/* 82 */	"Remote address changed",
/* 83 */	"Can not access a needed shared library",
/* 84 */	"Accessing a corrupted shared library",
/* 85 */	".lib section in a.out corrupted",
/* 86 */	"Attempting to link in more shared libraries than system limit",
/* 87 */	"Can not exec a shared library directly",
/* 88 */        "",	/* NULL */
/* 89 */	 "No such system call",		    /* ENOSYS */
/* 90 */	 "Too many levels of Symbolic  links",    /* ELOOP */
/* 91 */	"ERESTART",	/* ERESTART */
/* 92 */        "",	/* NULL */
/* 93 */        "",	/* NULL */
/* 94 */        "",	/* NULL */
/* 95 */        "",	/* NULL */
/* 96 */        "",	/* NULL */
/* 97 */        "",	/* NULL */
/* 98 */        "",	/* NULL */
/* 99 */        "",	/* NULL */
/* 100 */        "",	/* NULL */
/* 101 */        "",	/* NULL */
/* 102 */        "",	/* NULL */
/* 103 */        "",	/* NULL */
/* 104 */        "",	/* NULL */
/* 105 */        "",	/* NULL */
/* 106 */        "",	/* NULL */
/* 107 */        "",	/* NULL */
/* 108 */        "",	/* NULL */
/* 109 */        "",	/* NULL */
/* 110 */        "",	/* NULL */
/* 111 */        "",	/* NULL */
/* 112 */        "",	/* NULL */
/* 113 */        "",	/* NULL */
/* 114 */        "",	/* NULL */
/* 115 */        "",	/* NULL */
/* 116 */        "",	/* NULL */
/* 117 */        "",	/* NULL */
/* 118 */        "",	/* NULL */
/* 119 */        "",	/* NULL */
/* 120 */        "",	/* NULL */
/* 121 */        "",	/* NULL */
/* 122 */        "",	/* NULL */
/* 123 */        "",	/* NULL */
/* 124 */        "",	/* NULL */
/* 125 */        "",	/* NULL */
/* 126 */        "",	/* NULL */
/* 127 */        "",	/* NULL */
/* 128 */        "Operation now in progress", 	   /* EINPROGRESS */
/* 129 */	 "Operation already in progress",  /* EALREADY */	
/* 130 */	 "Socket operation on non-socket", /* ENOTSOCK */
/* 131 */	 "Destination address required",   /* EDESTADDRREQ */
/* 132 */	 "Message too long",		   /* EMSGSIZE */
/* 133 */ 	 "Protocol wrong type for socket", /* EPROTOTYPE */
/* 134 */	 "Protocol not available",	   /* ENOPROTOOPT */
/* 135 */ 	 "Protocol not supported",	   /* EPROTONOSUPPORT */
/* 136 */	 "Socket type not supported",	   /* ESOCKTNOSUPPORT */
/* 137 */	 "Operation not supported on socket", /* EOPNOTSUPP */
/* 138 */	 "Protocol family not supported",  /* EPFNOSUPPORT */
/* 139 */ 	 "Proto fam doesn't support addr fam", /* EAFNOSUPPORT */
/* 140 */	 "Address already in use",	   /* EADDRINUSE */
/* 141 */	 "Can't assign requested address", /* EADDRNOTAVAIL */
/* 142 */	 "Network is down",		   /* ENETDOWN */
/* 143 */	 "Network is unreachable",	   /* ENETUNREACH */
/* 144 */	 "Network dropped connection on reset", /* ENETRESET */
/* 145 */	 "Software caused connection abort",    /* ECONNABORTED */
/* 146 */ 	 "Connection reset by peer",	    /* ECONNRESET */
/* 147 */ 	 "No buffer space available",	    /* ENOBUFS */
/* 148 */	 "Socket is already connected",	    /* EISCONN */
/* 149 */	 "Socket is not connected",	    /* ENOTCONN */
/* 150 */	 "Can't send after socket shutdown", /* ESHUTDOWN */
/* 151 */	 "Too many references: can't splice", /* ETOOMANYREFS */
/* 152 */	 "Connection timed out",	     /* ETIMEDOUT */
/* 153 */	 "Connection refused",		    /* ECONNREFUSED */
/* 154 */        "",	/* NULL */
/* 155 */        "",	/* NULL */
/* 156 */	 "Host is down",		    /* EHOSTDOWN */
/* 157 */	 "Host is unreachable",		    /* EHOSTUNREACH  */
/* 158 */	 "Directory not Empty",		    /* ENOTEMPTY */
/* 159 */	 "Too many processes",		    /* EPROCLIM  */
/* 160 */	 "Too many users",		    /* EUSERS */
/* 161 */	 "Disk quota exceeded",		    /* EDQUOT */
/* 162 */ 	 "ESTALE",			    /* ESTALE */
/* 163 */ 	 "EPOWERFAIL",			    /* EPOWERFAIL */

};
int	sys_nerr = { sizeof(sys_errlist)/sizeof(sys_errlist[0]) };

