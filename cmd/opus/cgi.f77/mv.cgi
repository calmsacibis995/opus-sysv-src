short vappl_ (dev_handle, function, data_cnt, app_data)
short * dev_handle;
char  * function;
short * data_cnt;
short * app_data;
{
	return(v_appl(* dev_handle, function,* data_cnt, app_data));
}

short vclrwk_ (dev_handle)
short * dev_handle;
{
	return(v_clrwk(* dev_handle));
}

short fdclos_ (fd)
short * fd;
{
	return(fd_close(* fd));
}

short vclswk_ (dev_handle)
short * dev_handle;
{
	return(v_clswk(* dev_handle));
}

short fdcon_ (directory)
char  directory;
{
	return(fd_connect( directory));
}

short vcpbit_ (dev_handle, bitmap_handle, source_rect, dest_orig)
short * dev_handle;
short * bitmap_handle;
{
	return(vcp_bitmap(* dev_handle,* bitmap_handle, source_rect, dest_orig));
}

short fdcopy_ (fd)
short * fd;
{
	return(fd_copy(* fd));
}

short vcbit_ (dev_handle, xy, type, bitmap_handle)
short * dev_handle;
long  xy;
short * type;
long  bitmap_handle;
{
	return(vc_bitmap(* dev_handle, xy,* type, bitmap_handle));
}

short vc_cursor_ (dev_handle, data_bitmap, mask_bitmap, x, y, cursor_handle)
short * dev_handle;
short * data_bitmap;
short * mask_bitmap;
short * x;
short * y;
short  cursor_handle;
{
	return(vc_cursor(* dev_handle,* data_bitmap,* mask_bitmap,* x,* y, cursor_handle));
}

short vcurdown_ (dev_handle)
short * dev_handle;
{
	return(v_curdown(* dev_handle));
}

short vcurhome_ (dev_handle)
short * dev_handle;
{
	return(v_curhome(* dev_handle));
}

short vcurleft_ (dev_handle)
short * dev_handle;
{
	return(v_curleft(* dev_handle));
}

short vcurright_ (dev_handle)
short * dev_handle;
{
	return(v_curright(* dev_handle));
}

short vcurup_ (dev_handle)
short * dev_handle;
{
	return(v_curup(* dev_handle));
}

short vd_bitmap_ (dev_handle, bitmap_handle)
short * dev_handle;
short * bitmap_handle;
{
	return(vd_bitmap(* dev_handle,* bitmap_handle));
}

short vd_cursor_ (dev_handle, cursor_handle)
short * dev_handle;
short * cursor_handle;
{
	return(vd_cursor(* dev_handle,* cursor_handle));
}

short fddelete_ (fd, name)
short * fd;
char  name;
{
	return(fd_delete(* fd, name));
}

short vs_curaddress_ (dev_handle, row, column)
short * dev_handle;
short * row;
short * column;
{
	return(vs_curaddress(* dev_handle,* row,* column));
}

short vdspcur_ (dev_handle, x, y)
short * dev_handle;
short * x;
short * y;
{
	return(v_dspcur(* dev_handle,* x,* y));
}

short venter_cur_ (dev_handle)
short * dev_handle;
{
	return(v_enter_cur(* dev_handle));
}

short veeol_ (dev_handle)
short * dev_handle;
{
	return(v_eeol(* dev_handle));
}

short veeos_ (dev_handle)
short * dev_handle;
{
	return(v_eeos(* dev_handle));
}

short vexit_cur_ (dev_handle)
short * dev_handle;
{
	return(v_exit_cur(* dev_handle));
}

long fdsize_ (fd)
short * fd;
{
	return(fd_size(* fd));
}

short vhardcopy_ (dev_handle)
short * dev_handle;
{
	return(v_hardcopy(* dev_handle));
}

short vq_chcells_ (dev_handle, rows, columns)
short * dev_handle;
short  rows;
short  columns;
{
	return(vq_chcells(* dev_handle, rows, columns));
}

short vqa_cap_ (dev_handle, alph_cap)
short * dev_handle;
short  alph_cap;
{
	return(vqa_cap(* dev_handle, alph_cap));
}

short vqa_cell_ (dev_handle, row, column, propflag, x_out, y_out)
short * dev_handle;
short * row;
short * column;
short  propflag;
short  x_out;
short  y_out;
{
	return(vqa_cell(* dev_handle,* row,* column, propflag, x_out, y_out));
}

short vqa_font_ (dev_handle, font_in, size_in, font_status)
short * dev_handle;
short * font_in;
short * size_in;
short  font_status;
{
	return(vqa_font(* dev_handle,* font_in,* size_in, font_status));
}

short vqa_position_ (dev_handle, x_out, y_out)
short * dev_handle;
short  x_out;
short  y_out;
{
	return(vqa_position(* dev_handle, x_out, y_out));
}

short vqa_length_ (dev_handle, string)
short * dev_handle;
char  string;
{
	return(vqa_length(* dev_handle, string));
}

short vqb_mode_ (dev_handle)
short * dev_handle;
{
	return(vqb_mode(* dev_handle));
}

short vqb_format_ (dev_handle)
short * dev_handle;
{
	return(vqb_format(* dev_handle));
}

short vqb_pixels_ (dev_handle, origin, width, height, valid_width, valid_height, pixels)
short * dev_handle;
short  origin;
short * width;
short * height;
short  valid_width;
short  valid_height;
char  pixels;
{
	return(vqb_pixels(* dev_handle, origin,* width,* height, valid_width, valid_height, pixels));
}

short vq_cellarray_ (dev_handle, xy, row_length, num_rows, el_per_row, rows_used, status, colors)
short * dev_handle;
short  xy;
short * row_length;
short * num_rows;
short  el_per_row;
short  rows_used;
short  status;
short  colors;
{
	return(vq_cellarray(* dev_handle, xy,* row_length,* num_rows, el_per_row, rows_used, status, colors));
}

short vq_error_ ()
{
	return(vq_error(* ));
}

short vqc_rectangle_ (dev_handle, clip_rect)
short * dev_handle;
short  clip_rect;
{
	return(vqc_rectangle(* dev_handle, clip_rect));
}

short vq_color_ (dev_handle, ind_in, set_flag, rgb)
short * dev_handle;
short * ind_in;
short * set_flag;
short  rgb;
{
	return(vq_color(* dev_handle,* ind_in,* set_flag, rgb));
}

short vq_curaddress_ (dev_handle, row, column)
short * dev_handle;
short  row;
short  column;
{
	return(vq_curaddress(* dev_handle, row, column));
}

short vqf_attributes_ (dev_handle, attrib)
short * dev_handle;
short  attrib;
{
	return(vqf_attributes(* dev_handle, attrib));
}

short vqt_attributes_ (dev_handle, attrib)
short * dev_handle;
short  attrib;
{
	return(vqt_attributes(* dev_handle, attrib));
}

short vql_attributes_ (dev_handle, attrib)
short * dev_handle;
short  attrib;
{
	return(vql_attributes(* dev_handle, attrib));
}

short vqm_attributes_ (dev_handle, attrib)
short * dev_handle;
short  attrib;
{
	return(vqm_attributes(* dev_handle, attrib));
}

short vq_cursor_ (dev_handle, cursor_handle, data_bitmap, mask_bitmap, x, y)
short * dev_handle;
short * cursor_handle;
short  data_bitmap;
short  mask_bitmap;
short  x;
short  y;
{
	return(vq_cursor(* dev_handle,* cursor_handle, data_bitmap, mask_bitmap, x, y));
}

short vq_displays_ (dev_handle, n, displays)
short * dev_handle;
short * n;
short  displays;
{
	return(vq_displays(* dev_handle,* n, displays));
}

short vqd_bitmap_ (dev_handle, bitmap_handle, xy)
short * dev_handle;
short  bitmap_handle;
short  xy;
{
	return(vqd_bitmap(* dev_handle, bitmap_handle, xy));
}

short fdinq_ (fd)
short * fd;
{
	return(fd_inq(* fd));
}

short vqf_representation_ (dev_handle, n, attrib)
short * dev_handle;
short * n;
short  attrib;
{
	return(vqf_representation(* dev_handle,* n, attrib));
}

short vqg_cursor_ (dev_handle, cursor_handle)
short * dev_handle;
short  cursor_handle;
{
	return(vqg_cursor(* dev_handle, cursor_handle));
}

short vqt_extent_ (dev_handle, xin, yin, string, xcon, ycon, rectangle, bool)
short * dev_handle;
short * xin;
short * yin;
char  string;
short  xcon;
short  ycon;
short  rectangle;
ushort  bool;
{
	return(vqt_extent(* dev_handle,* xin,* yin, string, xcon, ycon, rectangle, bool));
}

short vqtf_character_ (dev_handle, character, n, metrics)
short * dev_handle;
short * character;
short * n;
short  metrics;
{
	return(vqtf_character(* dev_handle,* character,* n, metrics));
}

short vqtf_description_ (dev_handle, n, description, font_name)
short * dev_handle;
short * n;
short  description;
char  font_name;
{
	return(vqtf_description(* dev_handle,* n, description, font_name));
}

short vqtf_metrics_ (dev_handle, n, metrics)
short * dev_handle;
short * n;
short  metrics;
{
	return(vqtf_metrics(* dev_handle,* n, metrics));
}

short vqt_representation_ (dev_handle, n, attrib)
short * dev_handle;
short * n;
short  attrib;
{
	return(vqt_representation(* dev_handle,* n, attrib));
}

short vqi_extent_ (dev_handle, rectangle)
short * dev_handle;
short  rectangle;
{
	return(vqi_extent(* dev_handle, rectangle));
}

short vqi_pixels_ (dev_handle, origin, width, height, valid_width, valid_height, pixels)
short * dev_handle;
short  origin;
short * width;
short * height;
short  valid_width;
short  valid_height;
short  pixels;
{
	return(vqi_pixels(* dev_handle, origin,* width,* height, valid_width, valid_height, pixels));
}

short vqo_pattern_ (dev_handle, xy)
short * dev_handle;
short  xy;
{
	return(vqo_pattern(* dev_handle, xy));
}

short vql_representation_ (dev_handle, n, attrib)
short * dev_handle;
short * n;
short  attrib;
{
	return(vql_representation(* dev_handle,* n, attrib));
}

short vqm_representation_ (dev_handle, n, attrib)
short * dev_handle;
short * n;
short  attrib;
{
	return(vqm_representation(* dev_handle,* n, attrib));
}

makelib: line 712: warning: mode specification required for pointer or structure
long cgi_load_ (where, bytes_avail)
unchar * where;
long * bytes_avail;
{
	return(cgi_load(* where,* bytes_avail));
}

short vmsg_ (dev_handle, message, wait)
short * dev_handle;
char  message;
short * wait;
{
	return(v_msg(* dev_handle, message,* wait));
}

short fdopen_ (fd, name, mode)
short * fd;
char  name;
short * mode;
{
	return(fd_open(* fd, name,* mode));
}

short vopnwk_ (work_in, dev_handle, work_out)
short  work_in;
short  dev_handle;
short  work_out;
{
	return(v_opnwk( work_in, dev_handle, work_out));
}

short vatext_ (dev_handle, string, x_out, y_out)
short * dev_handle;
char  string;
short  x_out;
short  y_out;
{
	return(v_atext(* dev_handle, string, x_out, y_out));
}

short varc_ (dev_handle, x, y, radius, begang, endang)
short * dev_handle;
short * x;
short * y;
short * radius;
short * begang;
short * endang;
{
	return(v_arc(* dev_handle,* x,* y,* radius,* begang,* endang));
}

short vbar_ (dev_handle, xy)
short * dev_handle;
short  xy;
{
	return(v_bar(* dev_handle, xy));
}

short vb_pixels_ (dev_handle, origin, width, height, valid_width, valid_height, pixels)
short * dev_handle;
short  origin;
short * width;
short * height;
short  valid_width;
short  valid_height;
char  pixels;
{
	return(vb_pixels(* dev_handle, origin,* width,* height, valid_width, valid_height, pixels));
}

short vcellarray_ (dev_handle, xy, row_length, el_per_row, num_rows, wr_mode, colors)
short * dev_handle;
short  xy;
short * row_length;
short * el_per_row;
short * num_rows;
short * wr_mode;
short  colors;
{
	return(v_cellarray(* dev_handle, xy,* row_length,* el_per_row,* num_rows,* wr_mode, colors));
}

short vcircle_ (dev_handle, x, y, radius)
short * dev_handle;
short * x;
short * y;
short * radius;
{
	return(v_circle(* dev_handle,* x,* y,* radius));
}

short vcurtext_ (dev_handle, string)
short * dev_handle;
char  string;
{
	return(v_curtext(* dev_handle, string));
}

short vellipse_ (dev_handle, x, y, x_radius, y_radius)
short * dev_handle;
short * x;
short * y;
short * x_radius;
short * y_radius;
{
	return(v_ellipse(* dev_handle,* x,* y,* x_radius,* y_radius));
}

short vel_arc_ (dev_handle, x, y, x_radius, y_radius, s_angle, e_angle)
short * dev_handle;
short * x;
short * y;
short * x_radius;
short * y_radius;
short * s_angle;
short * e_angle;
{
	return(vel_arc(* dev_handle,* x,* y,* x_radius,* y_radius,* s_angle,* e_angle));
}

short vel_pieslice_ (dev_handle, x, y, x_radius, y_radius, s_angle, e_angle)
short * dev_handle;
short * x;
short * y;
short * x_radius;
short * y_radius;
short * s_angle;
short * e_angle;
{
	return(vel_pieslice(* dev_handle,* x,* y,* x_radius,* y_radius,* s_angle,* e_angle));
}

short vfillarea_ (dev_handle, count, xy)
short * dev_handle;
short * count;
short  xy;
{
	return(v_fillarea(* dev_handle,* count, xy));
}

short vgtext_ (dev_handle, x, y, string)
short * dev_handle;
short * x;
short * y;
char  string;
{
	return(v_gtext(* dev_handle,* x,* y, string));
}

short vi_pixels_ (dev_handle, origin, width, height, valid_width, valid_height, pixels)
short * dev_handle;
short  origin;
short * width;
short * height;
short  valid_width;
short  valid_height;
short  pixels;
{
	return(vi_pixels(* dev_handle, origin,* width,* height, valid_width, valid_height, pixels));
}

short vpieslice_ (dev_handle, x, y, radius, begang, endang)
short * dev_handle;
short * x;
short * y;
short * radius;
short * begang;
short * endang;
{
	return(v_pieslice(* dev_handle,* x,* y,* radius,* begang,* endang));
}

short vpline_ (dev_handle, count, xy)
short * dev_handle;
short * count;
short  xy;
{
	return(v_pline(* dev_handle,* count, xy));
}

short vpmarker_ (dev_handle, count, xy)
short * dev_handle;
short * count;
short  xy;
{
	return(v_pmarker(* dev_handle,* count, xy));
}

short fdparse_ (qualified_name, buffer, space_available)
char  qualified_name;
char  buffer;
short  space_available;
{
	return(fd_parse( qualified_name, buffer, space_available));
}

short vrd_curkeys_ (dev_handle, input_mode, direction, key)
short * dev_handle;
short * input_mode;
short  direction;
char  key;
{
	return(vrd_curkeys(* dev_handle,* input_mode, direction, key));
}

short fddirectory_ (fd, name, buffer, space_available)
short * fd;
char  name;
char  buffer;
short  space_available;
{
	return(fd_directory(* fd, name, buffer, space_available));
}

short fdp_read_ (fd, count, buffer)
short * fd;
short * count;
char  buffer;
{
	return(fdp_read(* fd,* count, buffer));
}

short fdread_ (fd, count, buffer)
short * fd;
short * count;
char  buffer;
{
	return(fd_read(* fd,* count, buffer));
}

short cgi_kill_ ()
{
	return(cgi_kill(* ));
}

short vrmcur_ (dev_handle)
short * dev_handle;
{
	return(v_rmcur(* dev_handle));
}

short fdrename_ (fd, old_name, new_name)
short * fd;
char  old_name;
char  new_name;
{
	return(fd_rename(* fd, old_name, new_name));
}

short vrq_choice_ (dev_handle, ch_in, ch_out)
short * dev_handle;
short * ch_in;
short  ch_out;
{
	return(vrq_choice(* dev_handle,* ch_in, ch_out));
}

short vrq_locator_ (dev_handle, xy_in, ink, rubberband, echo_handle, xy_out, terminator)
short * dev_handle;
short  xy_in;
short * ink;
short * rubberband;
short * echo_handle;
short  xy_out;
char  terminator;
{
	return(vrq_locator(* dev_handle, xy_in,* ink,* rubberband,* echo_handle, xy_out, terminator));
}

short vrq_string_ (dev_handle, max_length, echo_mode, echo_xy, string)
short * dev_handle;
short * max_length;
short * echo_mode;
short  echo_xy;
char  string;
{
	return(vrq_string(* dev_handle,* max_length,* echo_mode, echo_xy, string));
}

short vrq_valuator_ (dev_handle, val_in, echo_handle, val_out)
short * dev_handle;
short * val_in;
short * echo_handle;
short  val_out;
{
	return(vrq_valuator(* dev_handle,* val_in,* echo_handle, val_out));
}

short vr_defaults_ (dev_handle)
short * dev_handle;
{
	return(vr_defaults(* dev_handle));
}

short vrvoff_ (dev_handle)
short * dev_handle;
{
	return(v_rvoff(* dev_handle));
}

short vrvon_ (dev_handle)
short * dev_handle;
{
	return(v_rvon(* dev_handle));
}

short vsm_choice_ (dev_handle, ch_out)
short * dev_handle;
short  ch_out;
{
	return(vsm_choice(* dev_handle, ch_out));
}

short vsm_locator_ (dev_handle, xy_in, xy_out, pressed, released, key_state)
short * dev_handle;
short  xy_in;
short  xy_out;
ushort  pressed;
ushort  released;
ushort  key_state;
{
	return(vsm_locator(* dev_handle, xy_in, xy_out, pressed, released, key_state));
}

short vsm_string_ (dev_handle, max_length, echo_mode, echo_xy, string)
short * dev_handle;
short * max_length;
short * echo_mode;
short  echo_xy;
char  string;
{
	return(vsm_string(* dev_handle,* max_length,* echo_mode, echo_xy, string));
}

short vsm_valuator_ (dev_handle, val_out)
short * dev_handle;
short  val_out;
{
	return(vsm_valuator(* dev_handle, val_out));
}

long fdseek_ (fd, position, whence)
short * fd;
long * position;
short * whence;
{
	return(fd_seek(* fd,* position,* whence));
}

short vsd_bitmap_ (dev_handle, bitmap_handle)
short * dev_handle;
short * bitmap_handle;
{
	return(vsd_bitmap(* dev_handle,* bitmap_handle));
}

short vsg_cursor_ (dev_handle, cursor_handle)
short * dev_handle;
short * cursor_handle;
{
	return(vsg_cursor(* dev_handle,* cursor_handle));
}

short vsa_color_ (dev_handle, ind_in)
short * dev_handle;
short * ind_in;
{
	return(vsa_color(* dev_handle,* ind_in));
}

short vsa_font_ (dev_handle, font_in, size_in, font_cap)
short * dev_handle;
short * font_in;
short * size_in;
short  font_cap;
{
	return(vsa_font(* dev_handle,* font_in,* size_in, font_cap));
}

short vsa_spacing_ (dev_handle, spac_in)
short * dev_handle;
short * spac_in;
{
	return(vsa_spacing(* dev_handle,* spac_in));
}

short vsa_overstrike_ (dev_handle, mode_in)
short * dev_handle;
short * mode_in;
{
	return(vsa_overstrike(* dev_handle,* mode_in));
}

short vsa_passthru_ (dev_handle, mode_in)
short * dev_handle;
short * mode_in;
{
	return(vsa_passthru(* dev_handle,* mode_in));
}

short vsa_position_ (dev_handle, x_in, y_in, x_out, y_out)
short * dev_handle;
short * x_in;
short * y_in;
short  x_out;
short  y_out;
{
	return(vsa_position(* dev_handle,* x_in,* y_in, x_out, y_out));
}

short vsa_quality_ (dev_handle, mode_in)
short * dev_handle;
short * mode_in;
{
	return(vsa_quality(* dev_handle,* mode_in));
}

short vsa_supersub_ (dev_handle, mode_in)
short * dev_handle;
short * mode_in;
{
	return(vsa_supersub(* dev_handle,* mode_in));
}

short vsa_underline_ (dev_handle, mode_in)
short * dev_handle;
short * mode_in;
{
	return(vsa_underline(* dev_handle,* mode_in));
}

short vsb_color_ (dev_handle, ind_in)
short * dev_handle;
short * ind_in;
{
	return(vsb_color(* dev_handle,* ind_in));
}

short vsb_mode_ (dev_handle, mode)
short * dev_handle;
short * mode;
{
	return(vsb_mode(* dev_handle,* mode));
}

short vsc_rectangle_ (dev_handle, clip_rectangle)
short * dev_handle;
short  clip_rectangle;
{
	return(vsc_rectangle(* dev_handle, clip_rectangle));
}

short vs_color_ (dev_handle, ind_in, rgb_in, rgb_out)
short * dev_handle;
short * ind_in;
short  rgb_in;
short  rgb_out;
{
	return(vs_color(* dev_handle,* ind_in, rgb_in, rgb_out));
}

short vsc_table_ (dev_handle, s_index, n, rgb)
short * dev_handle;
short * s_index;
short * n;
short  rgb;
{
	return(vsc_table(* dev_handle,* s_index,* n, rgb));
}

short vcur_att_ (dev_handle, req_att, sel_att)
short * dev_handle;
short  req_att;
short  sel_att;
{
	return(vcur_att(* dev_handle, req_att, sel_att));
}

short vcur_color_ (dev_handle, fore_requested, back_requested, fore_selected, back_selected)
short * dev_handle;
short * fore_requested;
short * back_requested;
short  fore_selected;
short  back_selected;
{
	return(vcur_color(* dev_handle,* fore_requested,* back_requested, fore_selected, back_selected));
}

short vsf_representation_ (dev_handle, n, attrib)
short * dev_handle;
short * n;
short  attrib;
{
	return(vsf_representation(* dev_handle,* n, attrib));
}

short vsf_color_ (dev_handle, ind_in)
short * dev_handle;
short * ind_in;
{
	return(vsf_color(* dev_handle,* ind_in));
}

short vsf_interior_ (dev_handle, styl_in)
short * dev_handle;
short * styl_in;
{
	return(vsf_interior(* dev_handle,* styl_in));
}

short vsf_style_ (dev_handle, ind_in)
short * dev_handle;
short * ind_in;
{
	return(vsf_style(* dev_handle,* ind_in));
}

short vst_alignment_ (dev_handle, hor_in, vert_in, hor_out, vert_out)
short * dev_handle;
short * hor_in;
short * vert_in;
short  hor_out;
short  vert_out;
{
	return(vst_alignment(* dev_handle,* hor_in,* vert_in, hor_out, vert_out));
}

short vst_height_ (dev_handle, rq_height, char_width, cell_width, cell_height)
short * dev_handle;
short * rq_height;
short  char_width;
short  cell_width;
short  cell_height;
{
	return(vst_height(* dev_handle,* rq_height, char_width, cell_width, cell_height));
}

short vst_color_ (dev_handle, ind_in)
short * dev_handle;
short * ind_in;
{
	return(vst_color(* dev_handle,* ind_in));
}

short vst_font_ (dev_handle, font_in)
short * dev_handle;
short * font_in;
{
	return(vst_font(* dev_handle,* font_in));
}

short vst_representation_ (dev_handle, n, attrib)
short * dev_handle;
short * n;
short  attrib;
{
	return(vst_representation(* dev_handle,* n, attrib));
}

short vst_rotation_ (dev_handle, ang_in)
short * dev_handle;
short * ang_in;
{
	return(vst_rotation(* dev_handle,* ang_in));
}

short vsi_extent_ (dev_handle, rectangle)
short * dev_handle;
short  rectangle;
{
	return(vsi_extent(* dev_handle, rectangle));
}

short vs_editchars_ (dev_handle, line_del, char_del)
short * dev_handle;
char * line_del;
char * char_del;
{
	return(vs_editchars(* dev_handle,* line_del,* char_del));
}

short vs_penspeed_ (dev_handle, speed)
short * dev_handle;
short * speed;
{
	return(vs_penspeed(* dev_handle,* speed));
}

short vsl_color_ (dev_handle, ind_in)
short * dev_handle;
short * ind_in;
{
	return(vsl_color(* dev_handle,* ind_in));
}

short vsl_cross_ (dev_handle, bitmap_handle)
short * dev_handle;
short * bitmap_handle;
{
	return(vsl_cross(* dev_handle,* bitmap_handle));
}

short vsl_type_ (dev_handle, type_in)
short * dev_handle;
short * type_in;
{
	return(vsl_type(* dev_handle,* type_in));
}

short vsl_width_ (dev_handle, wid_in)
short * dev_handle;
short * wid_in;
{
	return(vsl_width(* dev_handle,* wid_in));
}

short vsl_representation_ (dev_handle, n, attrib)
short * dev_handle;
short * n;
short  attrib;
{
	return(vsl_representation(* dev_handle,* n, attrib));
}

short vsm_color_ (dev_handle, ind_in)
short * dev_handle;
short * ind_in;
{
	return(vsm_color(* dev_handle,* ind_in));
}

short vsm_height_ (dev_handle, hgt_in)
short * dev_handle;
short * hgt_in;
{
	return(vsm_height(* dev_handle,* hgt_in));
}

short vsm_representation_ (dev_handle, n, attrib)
short * dev_handle;
short * n;
short  attrib;
{
	return(vsm_representation(* dev_handle,* n, attrib));
}

short vsm_type_ (dev_handle, type_in)
short * dev_handle;
short * type_in;
{
	return(vsm_type(* dev_handle,* type_in));
}

short vsul_type_ (dev_handle, pattern)
short * dev_handle;
long * pattern;
{
	return(vsul_type(* dev_handle,* pattern));
}

short vswr_mode_ (dev_handle, mode_in)
short * dev_handle;
short * mode_in;
{
	return(vswr_mode(* dev_handle,* mode_in));
}

short vupdwk_ (dev_handle)
short * dev_handle;
{
	return(v_updwk(* dev_handle));
}

short fdp_write_ (fd, buffer, count)
short * fd;
char  buffer;
short * count;
{
	return(fdp_write(* fd, buffer,* count));
}

short fdwrite_ (fd, buffer, count)
short * fd;
char  buffer;
short * count;
{
	return(fd_write(* fd, buffer,* count));
}

short vs_curmode_ (dev_handle, mode)
short * dev_handle;
short * mode;
{
	return(vs_curmode(* dev_handle,* mode));
}

short vq_curmode_ (dev_handle)
short * dev_handle;
{
	return(vq_curmode(* dev_handle));
}

short vs_gclbu_ (dev_handle, mode)
short * dev_handle;
short * mode;
{
	return(vs_gclbu(* dev_handle,* mode));
}

short vq_gclbu_ (dev_handle)
short * dev_handle;
{
	return(vq_gclbu(* dev_handle));
}

short vget_npels_ (dev_handle, xy, src, n)
short * dev_handle;
short  xy;
short  src;
short * n;
{
	return(v_get_npels(* dev_handle, xy, src,* n));
}

short vput_npels_ (dev_handle, xy, dest, n)
short * dev_handle;
short  xy;
short  dest;
short * n;
{
	return(v_put_npels(* dev_handle, xy, dest,* n));
}

short vcopy_npels_ (dev_handle, xy)
short * dev_handle;
short  xy;
{
	return(v_copy_npels(* dev_handle, xy));
}

short vc_page_ (dev_handle, from, to)
short * dev_handle;
short * from;
short * to;
{
	return(vc_page(* dev_handle,* from,* to));
}

short vq_page_ (dev_handle, gmode, cmode)
short * dev_handle;
short  gmode;
short  cmode;
{
	return(vq_page(* dev_handle, gmode, cmode));
}

short vs_page_ (dev_handle, gmodein, cmodein, gmodeout, cmodeout)
short * dev_handle;
short  gmodein;
short  cmodein;
short  gmodeout;
short  cmodeout;
{
	return(vs_page(* dev_handle, gmodein, cmodein, gmodeout, cmodeout));
}

