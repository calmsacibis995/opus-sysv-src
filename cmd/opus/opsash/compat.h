#if pm200 || pm300 || pm400
#define PCCMD COMMPAGE
#define io_index dix
#define io_cmd	cmd
#define io_error err
#define io_bcount cnt
#define io_memaddr buf
#define io_devstat dst
#define io_status st
#define pc_kflag kflag
#define pc_kchar kchar
#define pc_sflag sflag
#define pc_schar schar

/* #define io_gen iorb */
/* #define io_sys iorb */
#define io_clk iorb
#define io_conin iorb
#define io_conout iorb
#define io_hdisk iorb
#define io_fdisk iorb
#define io_mt iorb
#define io_serin iorb
#define io_serout iorb
/* #define io_dos iorb */
#define io_lpt iorb
#define io_gn iorb

#endif
#if !pm100 && !pm200 && !pm300 && !pm400
#include "must/define/cpu/type"
#endif
