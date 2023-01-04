/*  @(#)dvrcgi.c	1.9 */

/*	dvrcgi.c
 *
 *	GSS CGI driver
 *
 *	10/27/86	Lattice long names
 *	10/01/86	fix logic that track open workstations
 *			fix vsm_locator parameters
 *			fix vqtf_description parameters
 *	12/17/85	initial version
 */

#include <dos.h>
#include "sys.h"
#include "mon.h"

#define OLD_VDI		/* include old vdi-only calls */

#define vb_pixels				vb_pixel
#define vcp_bitmap				vcp_bitm
#define vc_bitmap				vc_bitma
#define vc_cursor				vc_curso
#define v_curdown				v_curdow
#define v_curhome				v_curhom
#define v_curleft				v_curlef
#define v_curright				v_currig
#define vd_bitmap				vd_bitma
#define vd_cursor				vd_curso
#define vs_curaddress			vs_curad
#define v_enter_cur				v_enter_
#define v_exit_cur				v_exit_c
#define v_hardcopy				v_hardco
#define vrq_choice				vrq_choi
#define vsm_choice				vsm_choi
#define vrq_locator				vrq_loca
#define vsm_locator				vsm_loca
#define vrq_string				vrq_stri
#define vsm_string				vsm_stri
#define vrq_valuator			vrq_valu
#define vsm_valuator			vsm_valu
#define vq_chcells				vq_chcel
#define vqa_position			vqa_posi
#define vqa_length				vqa_leng
#define vqb_format				vqb_form
#define vqb_pixels				vqb_pixe
#define vq_cellarray			vq_cella
#define vqc_rectangle			vqc_rect
#define vq_curaddress			vq_curad
#define vqf_attributes			vqf_attr
#define vqt_attributes			vqt_attr
#define vql_attributes			vql_attr
#define vqm_attributes			vqm_attr
#define vq_cursor				vq_curso
#define vq_displays				vq_displ
#define vqd_bitmap				vqd_bitm
#define vqf_representation		vqf_repr
#define vqg_cursor				vqg_curs
#define vqt_extent				vqt_exte
#define vqtf_description		vqtf_des
#define vqtf_metrics			vqtf_met
#define vqtf_character			vqtf_cha
#define vqt_representation		vqt_repr
#define vqi_extent				vqi_exte
#define vqi_pixels				vqi_pixe
#define vqo_pattern				vqo_patt
#define vql_representation		vql_repr
#define vqm_representation		vqm_repr
#define vi_pixels				vi_pixel
#define v_cellarray				v_cellar
#define v_curtext				v_curtex
#define v_ellipse				v_ellips
#define vel_pieslice			vel_pies
#define v_fillarea				v_fillar
#define v_pieslice				v_piesli
#define v_pmarker				v_pmarke
#define vrd_curkeys				vrd_curk
#define vr_defaults				vr_defau
#define vsd_bitmap				vsd_bitm
#define vsg_cursor				vsg_curs
#define vsa_color				vsa_colo
#define vsa_spacing				vsa_spac
#define vsa_overstrike			vsa_over
#define vsa_passthru			vsa_pass
#define vsa_position			vsa_posi
#define vsa_quality				vsa_qual
#define vsa_supersub			vsa_supe
#define vsa_underline			vsa_unde
#define vsb_color				vsb_colo
#define vst_height				vst_heig
#define vsc_rectangle			vsc_rect
#define vsc_table				vsc_tabl
#define vcur_color				vcur_col
#define vsf_representation		vsf_repr
#define vsf_color				vsf_colo
#define vsf_interior			vsf_inte
#define vsf_style				vsf_styl
#define vst_alignment			vst_alig
#define vst_representation		vst_repr
#define vst_color				vst_colo
#define vst_rotation			vst_rota
#define vsi_extent				vsi_exte
#define vsl_cross				vsl_cros
#define vs_editchars			vs_editc
#define vs_penspeed				vs_pensp
#define vsl_color				vsl_colo
#define vsl_width				vsl_widt
#define vsl_representation		vsl_repr
#define vsm_color				vsm_colo
#define vsm_height				vsm_heig
#define vsm_representation		vsm_repr
#define vsul_type				vsul_typ
#define vswr_mode				vswr_mod
#ifdef OLD_VDI
	/*
	 *	interface to old VDI calls not officially supported by CGI
	 *
	 *	These may disappear in future GSS releases.
	 */
#define vs_curmode				vs_curmo
#define vq_curmode				vq_curmo
#define v_get_pels				v_get_pe
#define v_put_pels				v_put_pe
#define v_copy_pels				v_copy_p
#endif



#define UNIT 15		/* default gn/unit */
#define MAX_OPEN 16

extern ushort cfgargs;
extern long cfgarg[];

short ct_cgi();
void sv_cgi(), cgi_ctl(), cgi_doit();

static short cgi_flag = 0;		/* 0 if no cgi DOS driver */
static ushort cgi_int = 0;
static ushort base, cgi_off;
static ushort unit = UNIT;
static int opens[MAX_OPEN];
static int opencnt = 0;

extern void _gssvdi();
static void (*call_cgi)() = _gssvdi;

static struct iorb *rqb_cgi[4];		/* request queue */
static struct pq rq_cgi = {0,0,4,rqb_cgi};

struct dev dt_cgi = {GEN, 0, NULL, NULL, &rq_cgi, NULL,
		    ct_cgi, sv_cgi};

/*	ct_cgi(cmd, dt, arg)
 *
 *	system controls
 */
short
ct_cgi(cmd, dt, arg)
short cmd;
struct dev *dt;
char *arg;
{
	ushort val;
	short code;
	uchar *int_cgi;
	ushort cgi_vec, vec_seg, vec_off;
	static struct key keys[] = {
		{ "int", KEY_POS|KEY_SHORT|1 },
		{ NULL, KEY_END }
	};


	switch (cmd) {

	case CT_ARG:

		if (cfgargs)
			unit = cfgarg[0];

		while (code = ga_keys(keys, &val)) switch (code) {

		case 1:
			cgi_int = val;
			break;

		default:
			return(1);
		}
		break;

	case CT_INIT:
	case CT_REINIT:
	case CT_UNPAUSE:
		int_cgi = (uchar *)call_cgi + 0x19;
		if (*int_cgi != 0xCD) {		/* should be INT instruction */
			echos("opmon: cgi: can't find <int cgi>\r\n");
			return(0);
		}
		++int_cgi;
		if (cgi_int)
			*int_cgi = cgi_int;	/* change interrupt vector */
		cgi_vec = *int_cgi * 4;
		vec_off = xwpeek(cgi_vec, 0);	/* cgi service offset */
		vec_seg = xwpeek(cgi_vec+2, 0);	/* cgi service segment */
		if ( (vec_off || vec_seg) && vec_seg != 0xF000 )
			cgi_flag = 1;		/* cgi DOS driver present */
		break;

	default:
		break;
	}
	return(0);
}

/*	sv_cgi(dt)
 *
 *	service next request
 */
static void
sv_cgi(dt)
struct dev *dt;
{
	short i, cmd;
	struct iorb *iorb;

	if ( dt_cgi.act || (iorb = qgp(dt_cgi.rqq)) == NULL )
		return;

	curiorb = iorb;
	cmd = iorb->cmd;
	switch (cmd) {

	case PC_NOP:	/* nop */
	case PC_RST:	/* reset */
		break;

	case PC_STAT:	/* status */
		iorb->dst = unit;
		break;

	case PC_READ:	/* read is normal operation */
		if (chkac(iorb)) {
			iorb->err = E_MA;	/* bad address */
			break;
		}
		if (cgi_flag) {
			cgi_off = base = iorb->addr;	/* buffer */
			dt_cgi.act = iorb;
			cgi_doit(dt);		/* start service */
			return;
		}
		iorb->err = E_NA;
		break;

	case PC_IOCTL:	/* i/o control */
		if (iorb->blk == 0xfffe) { /* open */
			if (opencnt++ == 0)
				for (i=0; i<MAX_OPEN; ++i)
					opens[i] = -1;

		} else if (iorb->blk == 0xffff) { /* close */
			if (--opencnt == 0)
				for (i=0; i<MAX_OPEN; ++i)
					if (opens[i] != -1) {
						v_clswk(opens[i]);
						opens[i] = -1;
					}
		}
		break;

	default:
		iorb->err = E_CMD;
	}

	iodone(iorb);
	if (rq_cgi.get != rq_cgi.put)
		SCHED(svq, sv_cgi, dt);	/* schedule cgi service */
}

/*	cgi_doit(dt)
 *
 *	cgi service
 */
static void
cgi_doit(dt)
struct dev *dt;
{
	struct iorb *iorb;
	int s;
	short i, *v;

	if ( (iorb = dt_cgi.act) == NULL)
		return;

	curiorb = iorb;
	s = 0;
	v = (short *)makeptr(cgi_off, cpseg);

#define v1 v[3]
#define v2 v[4]
#define v3 v[5]
#define v4 v[6]
#define v5 v[7]
#define v6 v[8]
#define v7 v[9]
#define v8 v[10]

#define p1 v1+base,cpseg
#define p2 v2+base,cpseg
#define p3 v3+base,cpseg
#define p4 v4+base,cpseg
#define p5 v5+base,cpseg
#define p6 v6+base,cpseg
#define p7 v7+base,cpseg
#define p8 v8+base,cpseg

	switch(v[0]) {	/* switch on cgi command code */

	case 1:		/* application data */
		s = v_appl(v1, p2, v3, p4); break;
	case 2:		/* byte pixel array */
		s = vb_pixels(v1, p2, v3, v4, p5, p6, p7); break;
	case 3:	 	/* clear workstation */
		s = v_clrwk(v1); break;
	/* case 4-6 illegal */
	case 7:		/* close workstation */
		s = v_clswk(v1);
		for (i=0; i<MAX_OPEN; ++i) {
			if (opens[i] == v1) {
				opens[i] = -1;
				break;
			}
		}
		break;
	/* case 8 illegal */
	case 9:		/* copy bitmap */
		s = vcp_bitmap(v1, v2, p3, p4); break;
	/* case 10 illegal */
	case 11:	/* create bitmap */
		s = vc_bitmap(v1, p2, v3, p4); break;
	case 12:	/* create cursor */
		s = vc_cursor(v1, v2, v3, v4, v5, p6); break;
	case 13:	/* cursor down */
		s = v_curdown(v1); break;
	case 14:	/* cursor home */
		s = v_curhome(v1); break;
	case 15:	/* cursor left */
		s = v_curleft(v1); break;
	case 16:	/* cursor right */
		s = v_curright(v1); break;
	case 17:	/* cursor up */
		s = v_curup(v1); break;
	case 18:	/* delete bitmap */
		s = vd_bitmap(v1, v2); break;
	case 19:	/* delete cursor description */
		s = vd_cursor(v1, v2); break;
	/* case 20 illegal */
	case 21:	/* direct cursor address */
		s = vs_curaddress(v1, v2, v3); break;
	/* case 22 illegal */
	case 23:	/* display graphics input cursor */
		s = v_dspcur(v1, v2, v3); break;
	case 24:	/* enter cursor addressing mode */
		s = v_enter_cur(v1); break;
	case 25:	/* erase to end of line */
		s = v_eeol(v1); break;
	case 26:	/* erase to end of screen */
		s = v_eeos(v1); break;
	case 27:	/* exit cursor addressing mode */
		s = v_exit_cur(v1); break;
	/* case 28 illegal */
	case 29:	/* hardcopy */
		s = v_hardcopy(v1); break;
	/* case 30-31 illegal */
	case 32:	/* input choice (request mode) */
		s = vrq_choice(v1, v2, p3); break;
	case 33:	/* input choice (sample mode) */
		s = vsm_choice(v1, p2); break;
	case 34:	/* input locator (request mode) */
		s = vrq_locator(v1, p2, v3, v4, v5, p6, p7); break;
	case 35:	/* input locator (sample mode) */
		s = vsm_locator(v1, p2, p3, p4, p5, p6); break;
	case 36:	/* input string (request mode) */
		s = vrq_string(v1, v2, v3, p4, p5); break;
	case 37:	/* input string (sample mode) */
		s = vsm_string(v1, v2, v3, p4, p5); break;
	case 38:	/* input valuator (request mode) */
		s = vrq_valuator(v1, v2, v3, p4); break;
	case 39:	/* input valuator (sample mode) */
		s = vsm_valuator(v1, p2); break;
	case 40:	/* inquire addressable character cells */
		s = vq_chcells(v1, p2, p3); break;
	case 41:	/* inquire alpha text capabilities */
		s = vqa_cap(v1, p2); break;
	case 42:	/* inquire alpha text cell location */
		s = vqa_cell(v1, v2, v3, p4, p5, p6); break;
	case 43:	/* inquire alpha text font capabilities */
		s = vqa_font(v1, v2, v3, p4); break;
	case 44:	/* inquire alpha text position */
		s = vqa_position(v1, p2, p3); break;
	case 45:	/* inquire alpha text string length */
		s = vqa_length(v1, p2); break;
	case 46:	/* inquire background mode */
		s = vqb_mode(v1); break;
	case 47:	/* inquire bitmap formats */
		s = vqb_format(v1); break;
	case 48:	/* inquire byte pixel array */
		s = vqb_pixels(v1, p2, v3, v4, p5, p6, p7); break;
	case 49:	/* inquire cell array */
		s = vq_cellarray(v1, p2, v3, v4, p5, p6, p7, p8); break;
	case 50:	/* inquire clip rectangle */
		s = vqc_rectangle(v1, p2); break;
	case 51:	/* inquire color representation */
		s = vq_color(v1, v2, v3, p4); break;
	case 52:	/* inquire current cursor text address */
		s = vq_curaddress(v1, p2, p3); break;
	case 53:	/* inquire current fill area attributes */
		s = vqf_attributes(v1, p2); break;
	case 54:	/* inquire current graphic text attributes */
		s = vqt_attributes(v1, p2); break;
	case 55:	/* inquire current polyline attributes */
		s = vql_attributes(v1, p2); break;
	case 56:	/* inquire current polymarker attributes */
		s = vqm_attributes(v1, p2); break;
	case 57:	/* inquire cursor description */
		s = vq_cursor(v1, v2, p3, p4, p5, p6); break;
	case 58:	/* inquire displayable bitmaps */
		s = vq_displays(v1, v2, p3); break;
	case 59:	/* inquire drawing bitmap */
		s = vqd_bitmap(v1, p2, p3); break;
	/* case 60 illegal */
	case 61:	/* inquire fill area representation */
		s = vqf_representation(v1, v2, p3); break;
	case 62:	/* inquire graphic input cursor */
		s = vqg_cursor(v1, p2); break;
	case 63:	/* inquire graphic text extent */
		s = vqt_extent(v1, v2, v3, p4, p5, p6, p7, p8); break;
	case 64:	/* inquire graphic font description */
		s = vqtf_description(v1, v2, v3, p4, p5); break;
	case 65:	/* inquire graphic text font metrics */
		s = vqtf_metrics(v1, v2, p3); break;
	case 66:	/* inquire graphic text font character */
		s = vqtf_character(v1, v2, v3, p4); break;
	case 67:	/* inquire graphic text representation */
		s = vqt_representation(v1, v2, p3); break;
	case 68:	/* inquire input extent */
		s = vqi_extent(v1, p2); break;
	case 69:	/* inquire integer pixel array */
		s = vqi_pixels(v1, p2, v3, v4, p5, p6, p7); break;
	case 70:	/* inquire optimum pattern size */
		s = vqo_pattern(v1, p2); break;
	case 71:	/* inquire polyline representation */
		s = vql_representation(v1, v2, p3); break;
	case 72:	/* inquire polymarker representation */
		s = vqm_representation(v1, v2, p3); break;
	/* case 73 illegal */
	case 74:	/* inquire error */
		s = vq_error(); break;
	case 75:	/* inquire viewable displays */
		s = -1; break;
		/* not in beta library
		s = vq_viewable(v1, v2, v3, p4, p5, p6, p7); break;
		*/
	case 76:	/* integer pixel array */
		s = vi_pixels(v1, p2, v3, v4, p5, p6, p7); break;
	/* case 77 illegal */
	case 78:	/* message */
		s = v_msg(v1, p2, v3); break;
	/* case 79 illegal */
	case 80:	/* open workstation */
		s = v_opnwk(p1, p2, p3);
		if (s == 0) {
			for (i=0; i<MAX_OPEN; ++i) {
				if (opens[i] == -1) {
					opens[i] = *((short *)makeptr(p2));
					break;
				}
			}
		}
		break;
	case 81:	/* output alpha text */
		s = v_atext(v1, p2, p3, p4); break;
	case 82:	/* output arc */
		s = v_arc(v1, v2, v3, v4, v5, v6); break;
	case 83:	/* output bar */
		s = v_bar(v1, p2); break;
	case 84:	/* output cell array */
		s = v_cellarray(v1, p2, v3, v4, v5, v6, p7); break;
	case 85:	/* output circle */
		s = v_circle(v1, v2, v3, v4); break;
	case 86:	/* output cursor addressable text */
		s = v_curtext(v1, p2); break;
	case 87:	/* output ellipse */
		s = v_ellipse(v1, v2, v3, v4, v5); break;
	case 88:	/* output elliptical arc */
		s = vel_arc(v1, v2, v3, v4, v5, v6, v7); break;
	case 89:	/* output elliptical pie slice */
		s = vel_pieslice(v1, v2, v3, v4, v5, v6, v7); break;
	case 90:	/* output filled area */
		s = v_fillarea(v1, v2, p3); break;
	case 91:	/* output graphic text */
		s = v_gtext(v1, v2, v3, p4); break;
	case 92:	/* output pie slice */
		s = v_pieslice(v1, v2, v3, v4, v5, v6); break;
	case 93:	/* output polyline */
		s = v_pline(v1, v2, p3); break;
	case 94:	/* output polymarkers */
		s = v_pmarker(v1, v2, p3); break;
	/* case 95-97 illegal */
	case 98:	/* read cursor movement keys */
		s = vrd_curkeys(v1, v2, p3, p4); break;
	/* case 99-102 illegal */
	case 103:	/* remove graphic input cursor */
		s = v_rmcur(v1); break;
	/* case 104 illegal */
	case 105:	/* reset to defaults */
		s = vr_defaults(v1); break;
	case 106:	/* reverse video off */
		s = v_rvoff(v1); break;
	case 107:	/* reverse video on */
		s = v_rvon(v1); break;
	/* case 108 illegal */
	case 109:	/* select drawing bitmap */
		s = vsd_bitmap(v1, v2); break;
	case 110:	/* select graphic input cursor */
		s = vsg_cursor(v1, v2); break;
	/* case 111-112 illegal */
	case 113:	/* set alpha text color index */
		s = vsa_color(v1, v2); break;
	case 114:	/* set alpha text font and size */
		s = vsa_font(v1, v2, v3, p4); break;
	case 115:	/* set alpha text line spacing */
		s = vsa_spacing(v1, v2); break;
	case 116:	/* set alpha text overstrike mode */
		s = vsa_overstrike(v1, v2); break;
	case 117:	/* set alpha text pass through mode */
		s = vsa_passthru(v1, v2); break;
	case 118:	/* set alpha text position */
		s = vsa_position(v1, v2, v3, p4, p5); break;
	case 119:	/* set alpha text quality */
		s = vsa_quality(v1, v2); break;
	case 120:	/* set alpha text subscript superscript mode */
		s = vsa_supersub(v1, v2); break;
	case 121:	/* set alpha text underline mode */
		s = vsa_underline(v1, v2); break;
	case 122:	/* set background color index */
		s = vsb_color(v1, v2); break;
	case 123:	/* set background mode */
		s = vsb_mode(v1, v2); break;
	case 124:	/* set character height */
		s = vst_height(v1, v2, p3, p4, p5); break;
	case 125:	/* set clip rectangle */
		s = vsc_rectangle(v1, p2); break;
	case 126:	/* set color representation */
		s = vs_color(v1, v2, p3, p4); break;
	case 127:	/* set color table */
		s = vsc_table(v1, v2, v3, p4); break;
	case 128:	/* set cursor text attributes */
		s = vcur_att(v1, p2, p3); break;
	case 129:	/* set cursor text color index */
		s = vcur_color(v1, v2, v3, p4, p5); break;
	case 130:	/* set fill area representation */
		s = vsf_representation(v1, v2, p3); break;
	case 131:	/* set fill color index */
		s = vsf_color(v1, v2); break;
	case 132:	/* set fill interior style */
		s = vsf_interior(v1, v2); break;
	case 133:	/* set fill style index */
		s = vsf_style(v1, v2); break;
	case 134:	/* set graphic text alignment */
		s = vst_alignment(v1, v2, v3, p4, p5); break;
	case 135:	/* set graphic text font */
		s = vst_font(v1, v2); break;
	case 136:	/* set graphic text representation */
		s = vst_representation(v1, v2, p3); break;
	case 137:	/* set graphic text color index */
		s = vst_color(v1, v2); break;
	case 138:	/* set graphic text string baseline rotation */
		s = vst_rotation(v1, v2); break;
	case 139:	/* set input extent */
		s = vsi_extent(v1, p2); break;
	case 140:	/* set line cross section */
		s = vsl_cross(v1, v2); break;
	case 141:	/* set line edit characters */
		s = vs_editchars(v1, v2, v3); break;
	case 142:	/* set pen speed */
		s = vs_penspeed(v1, v2); break;
	case 143:	/* set polyline color index */
		s = vsl_color(v1, v2); break;
	case 144:	/* set polyline line type */
		s = vsl_type(v1, v2); break;
	case 145:	/* set polyline line width */
		s = vsl_width(v1, v2); break;
	case 146:	/* set polyline representation */
		s = vsl_representation(v1, v2, p3); break;
	case 147:	/* set polymarker color index */
		s = vsm_color(v1, v2); break;
	case 148:	/* set polymarker height */
		s = vsm_height(v1, v2); break;
	case 149:	/* set polymarker representation */
		s = vsm_representation(v1, v2, p3); break;
	case 150:	/* set polymarker type */
		s = vsm_type(v1, v2); break;
	case 151:	/* set user polyline type */
		s = vsul_type(v1, v2, v3); break;
	case 152:	/* set writing mode */
		s = vswr_mode(v1, v2); break;
	case 153:	/* update workstation */
		s = v_updwk(v1); break;
	/* case 154-155 illegal */

	default:

#ifdef OLD_VDI
	/*
	 *	interface to old VDI calls not officially supported by CGI
	 *
	 *	These may disappear in future GSS releases.
	 */
	switch(v[0]) {	/* switch on vdi command code */

	case 201:	/* set cursor text mode */
		s = vs_curmode(v1, v2); break;
	case 202:	/* inquire cursor text mode */
		s = vq_curmode(v1); break;
	case 203:	/* set graphic color burst mode */
		s = vs_gclbu(v1, v2); break;
	case 204:	/* inquire graphic color burst mode */
		s = vq_gclbu(v1); break;
	case 205:	/* get pels */
		s = v_get_pels(v1, p2, p3); break;
	case 206:	/* put pels */
		s = v_put_pels(v1, p2, p3); break;
	case 207:	/* copy pels */
		s = v_copy_pels(v1, p2); break;
	case 208:	/* copy page */
		s = vc_page(v1, v2, v3); break;
	case 209:	/* inquire page */
		s = vq_page(v1, p2, p3); break;
	case 210:	/* set page */
		s = vs_page(v1, p2, p3, p4, p5); break;

	default:
		iorb->err = E_CMD;	/* illegal command code */
	}
#else
		iorb->err = E_CMD;	/* illegal command code */
#endif

	}
	v[1] = s;
	if (s < 0) {	/* cgi error */
		iorb->dst = -s;
		iorb->err = E_DEVICE;
	}
	if (!iorb->err && (cgi_off = v[2])) {	/* more to do */
		cgi_off += base;
		curiorb = NULL;
		SCHED(svq, cgi_doit, dt);	/* reschedule ourselves */
	} else {
		dt_cgi.act = NULL;
		iodone(iorb);
		if (rq_cgi.get != rq_cgi.put)
			SCHED(svq, sv_cgi, dt);	/* schedule cgi service */
	}
}
