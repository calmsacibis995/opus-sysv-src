#define CHAR_BIT	8
#define CHAR_MAX	SCHAR_MAX
#define CHAR_MIN	SCHAR_MIN
#define MB_LEN_MAX	5
#define INT_MAX		LONG_MAX
#define INT_MIN		LONG_MIN
#define LONG_MAX	2147483647
#define LONG_MIN	(-2147483648)
#define SCHAR_MAX	127
#define SCHAR_MIN	(-128)
#define SHRT_MAX	32767
#define SHRT_MIN	(-32768)
#define UCHAR_MAX	255
#define UINT_MAX	ULONG_MAX
#define ULONG_MAX	4294967295
#define USHRT_MAX	65535

#define DBL_DIG		15
#define DBL_MAX		1.7976931348623145E308
#define DBL_MIN		2.2250738585072016E-308
#define FLT_DIG		6
#define FLT_MAX		3.40282347E38F
#define FLT_MIN		1.17549435E-38F
#define HUGE_VAL	__HUGE_VAL

#define ARG_MAX		8
#define CHILD_MAX	50
#define CLK_TCK		64
#define FCHR_MAX	ULONG_MAX
#define OPEN_MAX	72
#define LINK_MAX	1000
#define NAME_MAX	14
#define PATH_MAX	1024
#define PASS_MAX	8
#define PID_MAX		30000
#define PIPE_BUF	5120
#define PIPE_MAX	5120
#define STD_BLK		2048
#define SYS_NMLN	9
#define UID_MAX		30000
#define	USI_MAX		ULONG_MAX
#define WORD_BIT	32

#ifdef m88k
#undef NGROUPS_MAX
#undef ARG_MAX
#undef CHILD_MAX
#undef OPEN_MAX
#undef LINK_MAX
#undef NAME_MAX
#undef PATH_MAX
#undef PIPE_BUF
#undef MAX_CANON
#undef MAX_INPUT
#endif

#define _POSIX_NGROUPS_MAX	0
#define _POSIX_ARG_MAX		4096
#define _POSIX_CHILD_MAX	6
#define _POSIX_OPEN_MAX		16
#define _POSIX_LINK_MAX		8
#define _POSIX_MAX_CANON	255
#define _POSIX_MAX_INPUT	255
#define _POSIX_NAME_MAX		14
#define _POSIX_PATH_MAX		255
#define _POSIX_PIPE_BUF		512
