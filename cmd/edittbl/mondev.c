/* SID @(#)mondev.c	1.29 */

/*	MONDEV.C
 *
 *	Master Device Table
 *
 *	04/08/87	added more tape devices
 *	04/01/87	I/O version
 *	11/20/86	add Xwindow driver
 *	11/11/86	add 24 computone drivers
 *	 9/23/86	turn off XT disk driver for milo
 *	 8/18/86	add computone
 *	 7/24/86	add 3rd Hostess
 *	 6/28/85	initial version
 */

#include "mon.h"
#include "typ.h"
#include "defs.h"

/*	The <devices> table defines all devices available to the monitor.
 *	config() uses this table to build the working device table from
 *		the device names in the configuration file.
 *	New drivers must be entered in this table in order to be recognized
 *		by the system.  Enter new devices before the "end" entry,
 *		and add an <extern struct dev> declaration.
 *	Devices such as 'console' that have multiple subdevices (control,
 *		input and output in the case of the console) must have
 *		an entry for each subdevice.  Multiple disk devices,
 *		which have distinct names, are not considered subdevices.
 */

extern struct dev dt_clk;
extern struct dev dt_cc0, dt_ci0, dt_co0, dt_ccd;
extern struct dev dt_cc1, dt_ci1, dt_co1;
extern struct dev dt_cc2, dt_ci2, dt_co2;
extern struct dev dt_cc3, dt_ci3, dt_co3;
extern struct dev dt_dos;
extern struct dev dt_fil0, dt_fil1, dt_fil2, dt_fil3;
extern struct dev dt_fil4, dt_fil5, dt_fil6, dt_fil7;
extern struct dev dt_fil8, dt_fil9, dt_fil10, dt_fil11;
extern struct dev dt_fil12, dt_fil13, dt_fil14, dt_fil15;
extern struct dev dt_fils, dt_fila, dt_filb;
extern struct dev dt_c10, dt_c11, dt_c12, dt_c13;
extern struct dev dt_d10, dt_d11, dt_d12, dt_d13;
extern struct dev dt_e10, dt_f11;
extern struct dev dt_asi1, dt_aso1, dt_asc1;
extern struct dev dt_asi2, dt_aso2, dt_asc2;
extern struct dev dt_asia1, dt_asoa1, dt_asca1;
extern struct dev dt_asia2, dt_asoa2, dt_asca2;
extern struct dev dt_asia3, dt_asoa3, dt_asca3;
extern struct dev dt_asia4, dt_asoa4, dt_asca4;
extern struct dev dt_asib1, dt_asob1, dt_ascb1;
extern struct dev dt_asib2, dt_asob2, dt_ascb2;
extern struct dev dt_asib3, dt_asob3, dt_ascb3;
extern struct dev dt_asib4, dt_asob4, dt_ascb4;
extern struct dev dt_asih1, dt_asoh1, dt_asch1;
extern struct dev dt_asih2, dt_asoh2, dt_asch2;
extern struct dev dt_asih3, dt_asoh3, dt_asch3;
extern struct dev dt_asih4, dt_asoh4, dt_asch4;
extern struct dev dt_asih5, dt_asoh5, dt_asch5;
extern struct dev dt_asih6, dt_asoh6, dt_asch6;
extern struct dev dt_asih7, dt_asoh7, dt_asch7;
extern struct dev dt_asih8, dt_asoh8, dt_asch8;
extern struct dev dt_asig1, dt_asog1, dt_ascg1;
extern struct dev dt_asig2, dt_asog2, dt_ascg2;
extern struct dev dt_asig3, dt_asog3, dt_ascg3;
extern struct dev dt_asig4, dt_asog4, dt_ascg4;
extern struct dev dt_asig5, dt_asog5, dt_ascg5;
extern struct dev dt_asig6, dt_asog6, dt_ascg6;
extern struct dev dt_asig7, dt_asog7, dt_ascg7;
extern struct dev dt_asig8, dt_asog8, dt_ascg8;
extern struct dev dt_asii1, dt_asoi1, dt_asci1;
extern struct dev dt_asii2, dt_asoi2, dt_asci2;
extern struct dev dt_asii3, dt_asoi3, dt_asci3;
extern struct dev dt_asii4, dt_asoi4, dt_asci4;
extern struct dev dt_asii5, dt_asoi5, dt_asci5;
extern struct dev dt_asii6, dt_asoi6, dt_asci6;
extern struct dev dt_asii7, dt_asoi7, dt_asci7;
extern struct dev dt_asii8, dt_asoi8, dt_asci8;

extern struct dev dt_ctmi0, dt_ctmo0, dt_ctmc0;
extern struct dev dt_ctmi1, dt_ctmo1, dt_ctmc1;
extern struct dev dt_ctmi2, dt_ctmo2, dt_ctmc2;
extern struct dev dt_ctmi3, dt_ctmo3, dt_ctmc3;
extern struct dev dt_ctmi4, dt_ctmo4, dt_ctmc4;
extern struct dev dt_ctmi5, dt_ctmo5, dt_ctmc5;
extern struct dev dt_ctmi6, dt_ctmo6, dt_ctmc6;
extern struct dev dt_ctmi7, dt_ctmo7, dt_ctmc7;

extern struct dev dt_ctmi8, dt_ctmo8, dt_ctmc8;
extern struct dev dt_ctmi9, dt_ctmo9, dt_ctmc9;
extern struct dev dt_ctmi10, dt_ctmo10, dt_ctmc10;
extern struct dev dt_ctmi11, dt_ctmo11, dt_ctmc11;
extern struct dev dt_ctmi12, dt_ctmo12, dt_ctmc12;
extern struct dev dt_ctmi13, dt_ctmo13, dt_ctmc13;
extern struct dev dt_ctmi14, dt_ctmo14, dt_ctmc14;
extern struct dev dt_ctmi15, dt_ctmo15, dt_ctmc15;

extern struct dev dt_ctmi16, dt_ctmo16, dt_ctmc16;
extern struct dev dt_ctmi17, dt_ctmo17, dt_ctmc17;
extern struct dev dt_ctmi18, dt_ctmo18, dt_ctmc18;
extern struct dev dt_ctmi19, dt_ctmo19, dt_ctmc19;
extern struct dev dt_ctmi20, dt_ctmo20, dt_ctmc20;
extern struct dev dt_ctmi21, dt_ctmo21, dt_ctmc21;
extern struct dev dt_ctmi22, dt_ctmo22, dt_ctmc22;
extern struct dev dt_ctmi23, dt_ctmo23, dt_ctmc23;

extern struct dev dt_opc;
extern struct dev dt_lpt1, dt_lpt2, dt_lpt3;
extern struct dev dt_flpa, dt_flpb;
extern struct dev dt_ver;
extern struct dev dt_udio;

extern struct dev dt_c10, dt_c11, dt_c12, dt_c13;
extern struct dev dt_d10, dt_d11, dt_d12, dt_d13;
extern struct dev dt_e10, dt_f10;
extern struct dev dt_net;
extern struct dev dt_eth;
extern struct dev dt_cms0;
extern struct dev dt_cms1;
extern struct dev dt_arc0;
extern struct dev dt_arc1;
extern struct dev dt_odi;
extern struct dev dt_cmt;
extern struct dev dt_pgc;
extern struct dev dt_Xc_her, dt_Xc_vik, dt_Xc_mf8, dt_Xi;
extern struct dev dt_pip;
extern struct dev dt_cgi;

struct devent devices[] = {

/** Clock*:<clock> */
	{ "clock", &dt_clk },

/** Console*:<console> */
	{ "console", &dt_cc0 }, { "console", &dt_ci0 }, { "console", &dt_co0 },

/** Sessions Consoles:<con?> */
/* BIOS interface */
	{ "con1", &dt_cc1 }, { "con1", &dt_ci1 }, { "con1", &dt_co1 },
	{ "con2", &dt_cc2 }, { "con2", &dt_ci2 }, { "con2", &dt_co2 },
	{ "con3", &dt_cc3 }, { "con3", &dt_ci3 }, { "con3", &dt_co3 },
/* DOS Interface */
	{ "doscon", &dt_ccd }, { "doscon", &dt_ci0 }, { "doscon", &dt_co0 },

/** DOS*:<dos> */
	{ "dos", &dt_dos },

/** DOS File:<dsk/?> */
	{ "fil0", &dt_fil0 },
	{ "fil1", &dt_fil1 },
	{ "fil2", &dt_fil2 },
	{ "fil3", &dt_fil3 },
	{ "fil4", &dt_fil4 },
	{ "fil5", &dt_fil5 },
	{ "fil6", &dt_fil6 },
	{ "fil7", &dt_fil7 },
	{ "fil8", &dt_fil8 },
	{ "fil9", &dt_fil9 },
	{ "fil10", &dt_fil10 },
	{ "fil11", &dt_fil11 },
	{ "fil12", &dt_fil12 },
	{ "fil13", &dt_fil13 },
	{ "fil14", &dt_fil14 },
	{ "fil15", &dt_fil15 },

/** DOS Swap File:<swap> */
	{ "swap", &dt_fils },

/** DOS Diskette*:<flp?> */
	{ "a:",   &dt_fila },
	{ "b:",   &dt_filb },

/** Raw Diskette:<flp?> */
	{ "a:bios", &dt_flpa },
	{ "b:bios", &dt_flpb },

/** BIOS Disk*:<dsk/?> */
	{ "c:", &dt_c10 },
	{ "d:", &dt_d10 },
	{ "e:", &dt_e10 },
	{ "f:", &dt_f10 },
	{ "c:10", &dt_c10 },
	{ "c:11", &dt_c11 },
	{ "c:12", &dt_c12 },
	{ "c:13", &dt_c13 },
	{ "d:10", &dt_d10 },
	{ "d:11", &dt_d11 },
	{ "d:12", &dt_d12 },
	{ "d:13", &dt_d13 },

/** Line Printer:<lp?> */
	{ "lpt1", &dt_lpt1 },
	{ "lpt2", &dt_lpt2 },
	{ "lpt3", &dt_lpt3 },

/** Tape Drive:<mt/?> */
	{ "wangtek", &dt_cms0 },
	{ "wangtek1", &dt_cms1 },
	{ "cms0", &dt_cms0 },
	{ "cms1", &dt_cms1 },
	{ "archive", &dt_arc0 },
	{ "archive1", &dt_arc1 },
	{ "catamount", &dt_cmt },

/** Serial TTY Ports:<tty?> */
/* PC drivers */
	{ "com1", &dt_asi1 }, { "com1", &dt_aso1 }, { "com1", &dt_asc1 },
	{ "com2", &dt_asi2 }, { "com2", &dt_aso2 }, { "com2", &dt_asc2 },
/* AST drivers */
	{ "coma1", &dt_asia1 }, { "coma1", &dt_asoa1 }, { "coma1", &dt_asca1 },
	{ "coma2", &dt_asia2 }, { "coma2", &dt_asoa2 }, { "coma2", &dt_asca2 },
	{ "coma3", &dt_asia3 }, { "coma3", &dt_asoa3 }, { "coma3", &dt_asca3 },
	{ "coma4", &dt_asia4 }, { "coma4", &dt_asoa4 }, { "coma4", &dt_asca4 },
	{ "coma5", &dt_asib1 }, { "coma5", &dt_asob1 }, { "coma5", &dt_ascb1 },
	{ "coma6", &dt_asib2 }, { "coma6", &dt_asob2 }, { "coma6", &dt_ascb2 },
	{ "coma7", &dt_asib3 }, { "coma7", &dt_asob3 }, { "coma7", &dt_ascb3 },
	{ "coma8", &dt_asib4 }, { "coma8", &dt_asob4 }, { "coma8", &dt_ascb4 },
/* Hostess drivers */
	{ "comh1", &dt_asih1 }, { "comh1", &dt_asoh1 }, { "comh1", &dt_asch1 },
	{ "comh2", &dt_asih2 }, { "comh2", &dt_asoh2 }, { "comh2", &dt_asch2 },
	{ "comh3", &dt_asih3 }, { "comh3", &dt_asoh3 }, { "comh3", &dt_asch3 },
	{ "comh4", &dt_asih4 }, { "comh4", &dt_asoh4 }, { "comh4", &dt_asch4 },
	{ "comh5", &dt_asih5 }, { "comh5", &dt_asoh5 }, { "comh5", &dt_asch5 },
	{ "comh6", &dt_asih6 }, { "comh6", &dt_asoh6 }, { "comh6", &dt_asch6 },
	{ "comh7", &dt_asih7 }, { "comh7", &dt_asoh7 }, { "comh7", &dt_asch7 },
	{ "comh8", &dt_asih8 }, { "comh8", &dt_asoh8 }, { "comh8", &dt_asch8 },
	{ "comh9", &dt_asig1 }, { "comh9", &dt_asog1 }, { "comh9", &dt_ascg1 },
	{ "comh10",&dt_asig2 }, { "comh10",&dt_asog2 }, { "comh10",&dt_ascg2 },
	{ "comh11",&dt_asig3 }, { "comh11",&dt_asog3 }, { "comh11",&dt_ascg3 },
	{ "comh12",&dt_asig4 }, { "comh12",&dt_asog4 }, { "comh12",&dt_ascg4 },
	{ "comh13",&dt_asig5 }, { "comh13",&dt_asog5 }, { "comh13",&dt_ascg5 },
	{ "comh14",&dt_asig6 }, { "comh14",&dt_asog6 }, { "comh14",&dt_ascg6 },
	{ "comh15",&dt_asig7 }, { "comh15",&dt_asog7 }, { "comh15",&dt_ascg7 },
	{ "comh16",&dt_asig8 }, { "comh16",&dt_asog8 }, { "comh16",&dt_ascg8 },
	{ "comh17",&dt_asii1 }, { "comh17",&dt_asoi1 }, { "comh17",&dt_asci1 },
	{ "comh18",&dt_asii2 }, { "comh18",&dt_asoi2 }, { "comh18",&dt_asci2 },
	{ "comh19",&dt_asii3 }, { "comh19",&dt_asoi3 }, { "comh19",&dt_asci3 },
	{ "comh20",&dt_asii4 }, { "comh20",&dt_asoi4 }, { "comh20",&dt_asci4 },
	{ "comh21",&dt_asii5 }, { "comh21",&dt_asoi5 }, { "comh21",&dt_asci5 },
	{ "comh22",&dt_asii6 }, { "comh22",&dt_asoi6 }, { "comh22",&dt_asci6 },
	{ "comh23",&dt_asii7 }, { "comh23",&dt_asoi7 }, { "comh23",&dt_asci7 },
	{ "comh24",&dt_asii8 }, { "comh24",&dt_asoi8 }, { "comh24",&dt_asci8 },

/* Computone:<tty?> */
	{ "ctm0",&dt_ctmi0 }, { "ctm0",&dt_ctmo0 }, { "ctm0",&dt_ctmc0 },
	{ "ctm1",&dt_ctmi1 }, { "ctm1",&dt_ctmo1 }, { "ctm1",&dt_ctmc1 },
	{ "ctm2",&dt_ctmi2 }, { "ctm2",&dt_ctmo2 }, { "ctm2",&dt_ctmc2 },
	{ "ctm3",&dt_ctmi3 }, { "ctm3",&dt_ctmo3 }, { "ctm3",&dt_ctmc3 },
	{ "ctm4",&dt_ctmi4 }, { "ctm4",&dt_ctmo4 }, { "ctm4",&dt_ctmc4 },
	{ "ctm5",&dt_ctmi5 }, { "ctm5",&dt_ctmo5 }, { "ctm5",&dt_ctmc5 },
	{ "ctm6",&dt_ctmi6 }, { "ctm6",&dt_ctmo6 }, { "ctm6",&dt_ctmc6 },
	{ "ctm7",&dt_ctmi7 }, { "ctm7",&dt_ctmo7 }, { "ctm7",&dt_ctmc7 },
	{ "ctm8",&dt_ctmi8 }, { "ctm8",&dt_ctmo8 }, { "ctm8",&dt_ctmc8 },
	{ "ctm9",&dt_ctmi9 }, { "ctm9",&dt_ctmo9 }, { "ctm9",&dt_ctmc9 },
	{ "ctm10",&dt_ctmi10 }, { "ctm10",&dt_ctmo10 }, { "ctm10",&dt_ctmc10 },
	{ "ctm11",&dt_ctmi11 }, { "ctm11",&dt_ctmo11 }, { "ctm11",&dt_ctmc11 },
	{ "ctm12",&dt_ctmi12 }, { "ctm12",&dt_ctmo12 }, { "ctm12",&dt_ctmc12 },
	{ "ctm13",&dt_ctmi13 }, { "ctm13",&dt_ctmo13 }, { "ctm13",&dt_ctmc13 },
	{ "ctm14",&dt_ctmi14 }, { "ctm14",&dt_ctmo14 }, { "ctm14",&dt_ctmc14 },
	{ "ctm15",&dt_ctmi15 }, { "ctm15",&dt_ctmo15 }, { "ctm15",&dt_ctmc15 },
	{ "ctm16",&dt_ctmi16 }, { "ctm16",&dt_ctmo16 }, { "ctm16",&dt_ctmc16 },
	{ "ctm17",&dt_ctmi17 }, { "ctm17",&dt_ctmo17 }, { "ctm17",&dt_ctmc17 },
	{ "ctm18",&dt_ctmi18 }, { "ctm18",&dt_ctmo18 }, { "ctm18",&dt_ctmc18 },
	{ "ctm19",&dt_ctmi19 }, { "ctm19",&dt_ctmo19 }, { "ctm19",&dt_ctmc19 },
	{ "ctm20",&dt_ctmi20 }, { "ctm20",&dt_ctmo20 }, { "ctm20",&dt_ctmc20 },
	{ "ctm21",&dt_ctmi21 }, { "ctm21",&dt_ctmo21 }, { "ctm21",&dt_ctmc21 },
	{ "ctm22",&dt_ctmi22 }, { "ctm22",&dt_ctmo22 }, { "ctm22",&dt_ctmc22 },
	{ "ctm23",&dt_ctmi23 }, { "ctm23",&dt_ctmo23 }, { "ctm23",&dt_ctmc23 },

/** Unix-Dos IO:<udio> */
	{ "udio", &dt_udio },

/** GSS-CGI Graphics Interface:<cgi> */
	{ "gsscgi", &dt_cgi },

/** Ethernet LAN Interface:<ether> */
	{ "xln", &dt_eth },

/** X Mouse, Keyboard, and Graphics:<X> */
	{ "viking-1", &dt_Xc_vik }, { "viking-1", &dt_Xi },
	{ "t8", &dt_Xc_mf8 }, { "t8", &dt_Xi },
	{ "hercules", &dt_Xc_her }, { "hercules", &dt_Xi },

/* pip frame grabber board */
	{ "pip", &dt_pip },

/* end of devices */
	{ "end", NULL }
};
