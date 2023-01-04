/*  @(#)v1.c	1.2 */

/*
 * C cover routines for vdi functions.
 */

#include <sys/vdicomm.h>

#define IN(n) (-(n))  /* n short values input to procedure */
#define OUT(n) (n)  /* n short values output from procedure */
#define VAL 0  /* value parameter */
#define IMM 1  /* command cannot be buffered */
#define BUF 0  /* command can be buffered */
#define BTOW(n) (((n)+1)>>1)  /* convert bytes to 16 bit words */
#define STOW(s) ((strlen(s)+2)>>1)  /* convert str len to 16 bit word cnt */

int
v_opnwk(work_in, dev, work_out)
short *work_in;
short *dev;
short *work_out;
{

	return(vdi_cmd(VOPNWK, 3, 19+1+66, IMM,
	               IN(19), work_in,
	               OUT(1), dev,
	               OUT(66), work_out));
}

int
v_clswk(dev)
short dev;
{

	return(vdi_cmd(VCLSWK, 1, 0, IMM,
		       VAL, dev));
}

int
v_clrwk(dev)
short dev;
{

	return(vdi_cmd(VCLRWK, 1, 0, IMM,
		       VAL, dev));
}

int
v_pline(dev, count, xy)
short dev;
short count;
short *xy;
{
	return(vdi_cmd(VPLINE, 3, count*2, BUF,
		       VAL, dev,
		       VAL, count,
	               IN(count*2), xy));
}

int
v_pmarker(dev, count, xy)
short dev;
short count;
short *xy;
{

	return(vdi_cmd(VPMARK, 3, count*2, BUF,
		       VAL, dev,
		       VAL, count,
	               IN(count*2), xy));
}

int
v_circle(dev, x, y, radius)
short dev;
short x;
short y;
short radius;
{

	return(vdi_cmd(VCIRCL, 4, 0, BUF,
		       VAL, dev,
		       VAL, x,
		       VAL, y,
	               VAL, radius));
}

int
v_updwk(dev)
short dev;
{

	return(vdi_cmd(VUPDWK, 1, 0, BUF,
		       VAL, dev));
}

int
vq_chcells(dev, rows, cols)
short dev;
short *rows;
short *cols;
{

	return(vdi_cmd(VQCHCL, 3, 2, IMM,
		       VAL, dev,
		       OUT(1), rows,
		       OUT(1), cols));
}

int
v_exit_cur(dev)
short dev;
{

	return(vdi_cmd(VEXCUR, 1, 0, BUF,
		       VAL, dev));
}

int
v_enter_cur(dev)
short dev;
{

	return(vdi_cmd(VENCUR, 1, 0, BUF,
		       VAL, dev));
}

int
v_curup(dev)
short dev;
{

	return(vdi_cmd(VCURUP, 1, 0, BUF,
		       VAL, dev));
}

int
v_curdown(dev)
short dev;
{

	return(vdi_cmd(VCURDN, 1, 0, BUF,
		       VAL, dev));
}

int
v_curright(dev)
short dev;
{

	return(vdi_cmd(VCURRT, 1, 0, BUF,
		       VAL, dev));
}

int
v_curleft(dev)
short dev;
{

	return(vdi_cmd(VCURLF, 1, 0, BUF,
		       VAL, dev));
}

int
v_curhome(dev)
short dev;
{

	return(vdi_cmd(VCURHM, 1, 0, BUF,
		       VAL, dev));
}

int
v_eeos(dev)
short dev;
{

	return(vdi_cmd(VEREOS, 1, 0, BUF,
		       VAL, dev));
}

int
v_eeol(dev)
short dev;
{

	return(vdi_cmd(VEREOL, 1, 0, BUF,
		       VAL, dev));
}

int
vs_curaddress(dev, row, col)
short dev;
short row;
short col;
{

	return(vdi_cmd(VCURAD, 3, 0, BUF,
		       VAL, dev,
		       VAL, row,
		       VAL, col));

}

int
vq_curaddress(dev, row, col)
short dev;
short *row;
short *col;
{

	return(vdi_cmd(VQCURA, 3, 2, IMM,
		       VAL, dev,
		       OUT(1), row,
		       OUT(1), col));
}

int
v_hardcopy(dev)
short dev;
{

	return(vdi_cmd(VHDCPY, 1, 0, BUF,
		       VAL, dev));
}

int
v_dspcur(dev, x, y)
short dev;
short x;
short y;
{

	return(vdi_cmd(VDSPCR, 3, 0, BUF,
		       VAL, dev,
		       VAL, x,
		       VAL, y));
}

int
vq_color(dev, indin, setflg, rgb)
short dev;
short indin;
short setflg;
short *rgb;
{

	return(vdi_cmd(VQCOLR, 4, 3, IMM,
		       VAL, dev,
		       VAL, indin,
		       VAL, setflg,
		       OUT(3), rgb));
}

int
vq_cellarray(dev, xy, rlen, nrow, erow, ruse, val, color)
short dev;
short *xy;
short rlen;
short nrow;
short *erow;
short *ruse;
short *val;
short *color;
{

	return(vdi_cmd(VQCLRY, 8, 7+(rlen*nrow), IMM,
		       VAL, dev,
		       IN(4), xy,
		       VAL, rlen,
		       VAL, nrow,
		       OUT(1), erow,
		       OUT(1), ruse,
		       OUT(1), val,
		       OUT(rlen*nrow), color));
}

int
vrq_locator(dev, initxy, ink, rubberband, echohandle, finalxy, terminator)
short dev;
short *initxy;
short ink;
short rubberband;
short echohandle;
short *finalxy;
short *terminator;
{

	return(vdi_cmd(VRQLOC, 7, 5, IMM,
		       VAL, dev,
		       IN(2), initxy,
		       VAL, ink,
		       VAL, rubberband,
		       VAL, echohandle,
		       OUT(2), finalxy,
		       OUT(1), terminator));
}

int
vsm_locator(dev, finalxy)
short dev;
short *finalxy;
{

	return(vdi_cmd(VSMLOC, 2, 2, IMM,
		       VAL, dev,
		       OUT(2), finalxy));
}

int
vrq_valuator(dev, initval, echohandle, finalval)
short dev;
short initval;
short echohandle;
short *finalval;
{

	return(vdi_cmd(VRQVAL, 4, 1, IMM,
		       VAL, dev,
		       VAL, initval,
		       VAL, echohandle,
		       OUT(1), finalval));
}

int
vsm_valuator(dev, finalval)
short dev;
short *finalval;
{

	return(vdi_cmd(VSMVAL, 2, 1, IMM,
		       VAL, dev,
		       OUT(1), finalval));
}

int
vrq_choice(dev, initchc, finalchc)
short dev;
short initchc;
short *finalchc;
{

	return(vdi_cmd(VRQCHC, 3, 1, IMM,
		       VAL, dev,
		       VAL, initchc,
		       OUT(1), finalchc));
}

int
vsm_choice(dev, finalchc)
short dev;
short *finalchc;
{

	return(vdi_cmd(VSMCHC, 2, 1, IMM,
		       VAL, dev,
		       OUT(1), finalchc));
}

int
vrq_string(dev, maxlen, echomode, echoxy, str)
short dev;
short maxlen;
short echomode;
short *echoxy;
char *str;
{

	return(vdi_cmd(VRQSTR, 5, 2+BTOW(maxlen), IMM,
		       VAL, dev,
		       VAL, maxlen,
		       VAL, echomode,
		       IN(2), echoxy,
		       OUT(BTOW(maxlen)), str));
}

int
vsm_string(dev, maxlen, echomode, echoxy, str)
short dev;
short maxlen;
short echomode;
short *echoxy;
char *str;
{

	return(vdi_cmd(VSMSTR, 5, 2+BTOW(maxlen), IMM,
		       VAL, dev,
		       VAL, maxlen,
		       VAL, echomode,
		       IN(2), echoxy,
		       OUT(BTOW(maxlen)), str));
}

int
vqa_cap(dev, cap)
short dev;
short *cap;
{

	return(vdi_cmd(VQACAP, 2, 15, IMM,
		       VAL, dev,
		       OUT(15), cap));
}

int
vqa_position(dev, x, y)
short dev;
short *x;
short *y;
{

	return(vdi_cmd(VQAPOS, 3, 2, IMM,
		       VAL, dev,
		       OUT(1), x,
		       OUT(1), y));
}

int
vqa_font(dev, font, size, cap)
short dev;
short font;
short size;
short *cap;
{

	return(vdi_cmd(VQAFNT, 4, 7, IMM,
		       VAL, dev,
		       VAL, font,
		       VAL, size,
		       OUT(7), cap));
}

int
vqa_length(dev, str)
short dev;
char *str;
{

	return(vdi_cmd(VQALEN, 2, STOW(str), IMM,
		       VAL, dev,
		       IN(STOW(str)), str));
}

int
vqa_cell(dev, row, col, prop, xout, yout)
short dev;
short row;
short col;
short *prop;
short *xout;
short *yout;
{

	return(vdi_cmd(VQACEL, 6, 3, IMM,
		       VAL, dev,
		       VAL, row,
		       VAL, col,
		       OUT(1), prop,
		       OUT(1), xout,
		       OUT(1), yout));
}

int
vql_attributes(dev, att)
short dev;
short *att;
{

	return(vdi_cmd(VQLATT, 2, 4, IMM,
		       VAL, dev,
		       OUT(4), att));
}

int
vqm_attributes(dev, att)
short dev;
short *att;
{

	return(vdi_cmd(VQMATT, 2, 4, IMM,
		       VAL, dev,
		       OUT(4), att));
}

int
vqf_attributes(dev, att)
short dev;
short *att;
{

	return(vdi_cmd(VQFATT, 2, 4, IMM,
		       VAL, dev,
		       OUT(4), att));
}

int
vqt_attributes(dev, att)
short dev;
short *att;
{

	return(vdi_cmd(VQTATT, 2, 10, IMM,
		       VAL, dev,
		       OUT(10), att));
}

int
vq_error(dev)
short dev;
{

	return(vdi_cmd(VQERR, 0, 0, IMM));
}

int
v_appl(dev, fname, len, data)
short dev;
char *fname;
short len;
char *data;
{

	return(vdi_cmd(VAPPL, 4, STOW(fname)+len, BUF,
		       VAL, dev,
		       IN(STOW(fname)), fname,
		       VAL, len,
		       IN(len), data));
}

int
v_msg(dev, msg, pause)
short dev;
char *msg;
short pause;
{

	return(vdi_cmd(VMSG, 3, STOW(msg), IMM,
		       VAL, dev,
		       IN(STOW(msg)), msg,
		       VAL, pause));
}

int
v_curtext(dev, str)
short dev;
char *str;
{

	return(vdi_cmd(VCTEXT, 2, STOW(str), BUF,
		       VAL, dev,
		       IN(STOW(str)), str));
}

int
v_rvon(dev)
short dev;
{

	return(vdi_cmd(VRVON, 1, 0, BUF,
		       VAL, dev));
}

int
v_rvoff(dev)
short dev;
{

	return(vdi_cmd(VRVOFF, 1, 0, BUF,
		       VAL, dev));
}

int
vcur_color(dev, forereq, backreq, foresel, backsel)
short dev;
short forereq;
short backreq;
short *foresel;
short *backsel;
{

	return(vdi_cmd(VCRCOL, 5, 2, IMM,
		       VAL, dev,
		       VAL, forereq,
		       VAL, backreq,
		       OUT(1), foresel,
		       OUT(1), backsel));
}

int
v_rmcur(dev)
short dev;
{

	return(vdi_cmd(VREMCR, 1, 0, BUF,
		       VAL, dev));
}

int
vrd_curkeys(dev, inputmode, direction, key)
short dev;
short *direction;
short *key;
{

	return(vdi_cmd(VRDCKY, 4, 2, IMM,
		       VAL, dev,
		       VAL, inputmode,
		       OUT(1), direction,
		       OUT(1), key));
}

int
vs_penspeed(dev, speed)
short dev;
short speed;
{

	return(vdi_cmd(VPNSPD, 2, 0, BUF,
		       VAL, dev,
		       VAL, speed));
}

int
vs_editchars(dev, linedel, chardel)
short dev;
short linedel;
short chardel;
{

	return(vdi_cmd(VSEDCH, 3, 0, BUF,
		       VAL, dev,
		       VAL, linedel,
		       VAL, chardel));
}

int
vcur_att(dev, reqatt, selatt)
short dev;
short *reqatt;
short *selatt;
{

	return(vdi_cmd(VCRATT, 3, 8, IMM,
		       VAL, dev,
		       IN(4), reqatt,
		       OUT(4), selatt));
}

int
v_gtext(dev, x, y, str)
short dev;
short x;
short y;
char *str;
{

	return(vdi_cmd(VGTEXT, 4, STOW(str), BUF,
		       VAL, dev,
		       VAL, x,
		       VAL, y,
		       IN(STOW(str)), str));
}

int
v_fillarea(dev, count, xy)
short dev;
short count;
short *xy;
{

	return(vdi_cmd(VFAREA, 3, count*2, BUF,
		       VAL, dev,
		       VAL, count,
		       IN(count*2), xy));
}

int
v_cellarray(dev, xy, rlen, erow, nrow, wmod, colors)
short dev;
short *xy;
short rlen;
short erow;
short nrow;
short wmod;
short *colors;
{

	return(vdi_cmd(VCLARY, 7, 4+(rlen*nrow), BUF,
		       VAL, dev,
		       IN(4), xy,
		       VAL, rlen,
		       VAL, erow,
		       VAL, nrow,
		       VAL, wmod,
		       IN(rlen*nrow), colors));
}

int
v_bar(dev, xy)
short dev;
short *xy;
{

	return(vdi_cmd(VBAR, 2, 4, BUF,
		       VAL, dev,
		       IN(4), xy));
}

int
v_arc(dev, x, y, radius, startangle, endangle)
short dev;
short x;
short y;
short radius;
short startangle;
short endangle;
{

	return(vdi_cmd(VARC, 6, 0, BUF,
		       VAL, dev,
		       VAL, x,
		       VAL, y,
		       VAL, radius,
		       VAL, startangle,
		       VAL, endangle));
}

int
v_pieslice(dev, x, y, radius, startangle, endangle)
short dev;
short x;
short y;
short radius;
short startangle;
short endangle;
{

	return(vdi_cmd(VPIESL, 6, 0, BUF,
		       VAL, dev,
		       VAL, x,
		       VAL, y,
		       VAL, radius,
		       VAL, startangle,
		       VAL, endangle));
}

int
vsa_passthru(dev, mode)
short dev;
short mode;
{

	return(vdi_cmd(VSAPAS, 2, 0, BUF,
		       VAL, dev,
		       VAL, mode));
}

int
vsa_quality(dev, mode)
short dev;
short mode;
{

	return(vdi_cmd(VSAQL, 2, 0, BUF,
		       VAL, dev,
		       VAL, mode));
}

int
v_atext(dev, str, xout, yout)
short dev;
char *str;
short *xout;
short *yout;
{

	return(vdi_cmd(VATEXT, 4, STOW(str)+2, IMM,
		       VAL, dev,
		       IN(STOW(str)), str,
		       OUT(1), xout,
		       OUT(1), yout));
}

int
vst_alignment(dev, hreq, vreq, hreal, vreal)
short dev;
short hreq;
short vreq;
short *hreal;
short *vreal;
{

	return(vdi_cmd(VSTALN, 5, 2, IMM,
		       VAL, dev,
		       VAL, hreq,
		       VAL, vreq,
		       OUT(1), hreal,
		       OUT(1), vreal));
}

int
vsb_color(dev, index)
short dev;
short index;
{

	return(vdi_cmd(VSBCOL, 2, 0, BUF,
		       VAL, dev,
		       VAL, index));
}

int
vswr_mode(dev, mode)
short dev;
short mode;
{

	return(vdi_cmd(VSWRMD, 2, 0, BUF,
		       VAL, dev,
		       VAL, mode));
}

int
vsa_position(dev, xin, yin, xout, yout)
short dev;
short xin;
short yin;
short *xout;
short *yout;
{

	return(vdi_cmd(VSAPOS, 5, 2, IMM,
		       VAL, dev,
		       VAL, xin,
		       VAL, yin,
		       OUT(1), xout,
		       OUT(1), yout));
}

int
vsa_spacing(dev, spacing)
short dev;
short spacing;
{

	return(vdi_cmd(VSASPC, 2, 0, BUF,
		       VAL, dev,
		       VAL, spacing));
}

int
vsa_font(dev, font, size, cap)
short dev;
short font;
short size;
short *cap;
{

	return(vdi_cmd(VSAFNT, 4, 8, IMM,
		       VAL, dev,
		       VAL, font,
		       VAL, size,
		       OUT(8), cap));
}

int
vsa_color(dev, index)
short dev;
short index;
{

	return(vdi_cmd(VSACOL, 2, 0, BUF,
		       VAL, dev,
		       VAL, index));
}

int
vsa_supersub(dev, mode)
short dev;
short mode;
{

	return(vdi_cmd(VSASUB, 2, 0, BUF,
		       VAL, dev,
		       VAL, mode));
}

int
vsa_underline(dev, mode)
short dev;
short mode;
{

	return(vdi_cmd(VSAUND, 2, 0, BUF,
		       VAL, dev,
		       VAL, mode));
}

int
vsa_overstrike(dev, mode)
short dev;
short mode;
{

	return(vdi_cmd(VSAOVR, 2, 0, BUF,
		       VAL, dev,
		       VAL, mode));
}

int
vst_height(dev, hreq, charw, cellw, cellh)
short dev;
short hreq;
short *charw;
short *cellw;
short *cellh;
{

	return(vdi_cmd(VSTHGT, 5, 3, IMM,
		       VAL, dev,
		       VAL, hreq,
		       OUT(1), charw,
		       OUT(1), cellw,
		       OUT(1), cellh));
}

int
vst_rotation(dev, angle)
short dev;
short angle;
{

	return(vdi_cmd(VSTROT, 2, 0, BUF,
		       VAL, dev,
		       VAL, angle));
}

int
vs_color(dev, index, rgbin, rgbout)
short dev;
short index;
short *rgbin;
short *rgbout;
{

	return(vdi_cmd(VSCOLR, 4, 6, IMM,
		       VAL, dev,
		       VAL, index,
		       IN(3), rgbin,
		       OUT(3), rgbout));
}

int
vsl_type(dev, type)
short dev;
short type;
{

	return(vdi_cmd(VSLTYP, 2, 0, BUF,
		       VAL, dev,
		       VAL, type));
}

int
vsl_width(dev, width)
short dev;
short width;
{

	return(vdi_cmd(VSLWID, 2, 0, BUF,
		       VAL, dev,
		       VAL, width));
}

int
vsl_color(dev, index)
short dev;
short index;
{

	return(vdi_cmd(VSLCOL, 2, 0, BUF,
		       VAL, dev,
		       VAL, index));
}

int
vsm_type(dev, type)
short dev;
short type;
{

	return(vdi_cmd(VSMTYP, 2, 0, BUF,
		       VAL, dev,
		       VAL, type));
}

int
vsm_height(dev, height)
short dev;
short height;
{

	return(vdi_cmd(VSMHGT, 2, 0, BUF,
		       VAL, dev,
		       VAL, height));
}

int
vsm_color(dev, index)
short dev;
short index;
{

	return(vdi_cmd(VSMCOL, 2, 0, BUF,
		       VAL, dev,
		       VAL, index));
}

int
vst_font(dev, font)
short dev;
short font;
{

	return(vdi_cmd(VSTFNT, 2, 0, BUF,
		       VAL, dev,
		       VAL, font));
}

int
vst_color(dev, index)
short dev;
short index;
{

	return(vdi_cmd(VSTCOL, 2, 0, BUF,
		       VAL, dev,
		       VAL, index));
}

int
vsf_interior(dev, style)
short dev;
short style;
{

	return(vdi_cmd(VSFINT, 2, 0, BUF,
		       VAL, dev,
		       VAL, style));
}

int
vsf_style(dev, index)
short dev;
short index;
{

	return(vdi_cmd(VSFSTL, 2, 0, BUF,
		       VAL, dev,
		       VAL, index));
}

int
vsf_color(dev, index)
short dev;
short index;
{

	return(vdi_cmd(VSFCOL, 2, 0, BUF,
		       VAL, dev,
		       VAL, index));
}

int
vq_curmode(dev)
short dev;
{

	return(vdi_cmd(VQCURM, 1, 0, BUF,
		       VAL, dev));
}

int
vq_gclbu(dev)
short dev;
{

	return(vdi_cmd(VQGCM, 1, 0, BUF,
		       VAL, dev));
}

int
v_get_npels(dev, xy, src, n)
short dev;
short *xy;
short *src;
int n;
{

	return(vdi_cmd(VGTPEL, 3, 4+n, IMM,
		       VAL, dev,
		       IN(4), xy,
		       OUT(n), src));
}

int
v_copy_pels(dev, xy)
short dev;
short *xy;
{

	return(vdi_cmd(VCPPEL, 2, 6, BUF,
		       VAL, dev,
		       IN(6), xy));
}

int
v_put_npels(dev, xy, dest, n)
short dev;
short *xy;
short *dest;
int n;
{

	return(vdi_cmd(VPTPEL, 3, 2+n, BUF,
		       VAL, dev,
		       IN(2), xy,
		       IN(n), dest));
}

int
vc_page(dev, from, to)
short dev;
short from;
short to;
{

	return(vdi_cmd(VCPAGE, 3, 0, BUF,
		       VAL, dev,
		       VAL, from,
		       VAL, to));
}

int
vq_page(dev, gmode, cmode)
short dev;
short *gmode;
short *cmode;
{

	return(vdi_cmd(VQPAGE, 3, 6, IMM,
		       VAL, dev,
		       OUT(3), gmode,
		       OUT(3), cmode));
}

int
vs_page(dev, gmodein, cmodein, gmodeout, cmodeout)
short dev;
short *gmodein;
short *cmodein;
short *gmodeout;
short *cmodeout;
{

	return(vdi_cmd(VSPAGE, 5, 8, IMM,
		       VAL, dev,
		       IN(2), gmodein,
		       IN(2), cmodein,
		       OUT(2), gmodeout,
		       OUT(2), cmodeout));
}

int
vs_curmode(dev, mode)
short dev;
short mode;
{

	return(vdi_cmd(VSCURM, 2, 0, BUF,
		       VAL, dev,
		       VAL, mode));
}

int
vs_gclbu(dev, mode)
short dev;
short mode;
{

	return(vdi_cmd(VSGCM, 2, 0, BUF,
		       VAL, dev,
		       VAL, mode));
}
